import sys
import os
import numpy as np
from pyevtk.hl import pointsToVTK
import itertools
from pumapy.utilities.workspace import Workspace


class RayCasting:
    def __init__(self, workspace, degree_accuracy, source_locations, valid_phase, boundary_behavior=0,
                 exportparticles_filepathname=''):
        self.ws = workspace
        self.degree_accuracy = degree_accuracy
        self.source_locations = source_locations
        self.valid_phase = valid_phase
        self.boundary_behavior = boundary_behavior  # 0=kill particles, 1=periodic BC
        self.exportparticles_filepathname = exportparticles_filepathname

        self.particles_number = int((180. / self.degree_accuracy - 1) * (360. / self.degree_accuracy) + 2)
        print("Number of particles in Ray Tracing simulation: {}".format(self.particles_number))

        self.rays_distances = None
        self.spherical_walkers = None
        self.X = None
        self.Y = None
        self.Z = None
        self.counter = itertools.count()

    def generate_spherical_walkers(self):
        # spherical_walkers arranged as:
        # dir=0,1,2; vox=3,4,5; pos=6,7,8; face_intersected=9; intersection=10, distance=11, particleID=12
        # x_distance=13, y_distance=14, z_distance=15
        self.spherical_walkers = np.zeros((self.particles_number, 16))

        # top and bottom particles in sphere
        self.spherical_walkers[[0, -1], 0] = np.sin([0, np.pi]) * np.cos([0, 2. * np.pi])  # dir_x
        self.spherical_walkers[[0, -1], 1] = np.sin([0, np.pi]) * np.sin([0, 2. * np.pi])  # dir_y
        self.spherical_walkers[[0, -1], 2] = np.cos([0, np.pi])  # dir_z

        # uniformly sampling a sphere by angles theta (arc from 0-180) and phi (circumference)
        arc_particles = int(180 / self.degree_accuracy) - 1
        arcs = int(360 / self.degree_accuracy)
        theta = np.linspace(self.degree_accuracy, 180 - self.degree_accuracy, arc_particles)
        theta = np.tile(theta, arcs)
        phi = np.linspace(0, 360 - self.degree_accuracy, arcs)
        phi = np.repeat(phi, arc_particles)
        self.spherical_walkers[1:-1, 0] = np.sin(theta * (np.pi/180.)) * np.cos(phi * (np.pi/180.))  # dir_x
        self.spherical_walkers[1:-1, 1] = np.sin(theta * (np.pi/180.)) * np.sin(phi * (np.pi/180.))  # dir_y
        self.spherical_walkers[1:-1, 2] = np.cos(theta * (np.pi/180.))  # dir_z

        # give particles IDs
        self.spherical_walkers[:, 12] = np.arange(self.particles_number)

    def expand_sources(self):
        # iterative loop through sources
        for i in range(self.source_locations.shape[0]):
            sys.stdout.write("\rShooting particles from sources ... {:.1f}% "
                             .format(max(i, 1) / max((self.source_locations.shape[0] - 1), 1) * 100))
            self.__reset_walkers(i)
            self.__execute_walks(i)
            self.rays_distances[i * self.particles_number:(i + 1) * self.particles_number] = self.spherical_walkers[:, 13:]
        print("Done")

    def __reset_walkers(self, i):
        # reset walkers positions to the center of source voxel
        self.spherical_walkers[:, 3:6] = self.source_locations[i]  # vox
        self.spherical_walkers[:, 6:9] = self.source_locations[i] + 0.5  # pos
        self.spherical_walkers[:, 9:12] = np.zeros((self.particles_number, 3))  # intersection, distance
        self.spherical_walkers[:, 13:] = np.zeros((self.particles_number, 3))

    def __execute_walks(self, source_number):

        # valid particles
        valid_mask = np.ones(self.spherical_walkers.shape[0], dtype=bool)

        # max travelled distance: diagonal of rectangular prism
        max_distance_void = 2. * np.sqrt(self.X**2 + self.Y**2 + self.Z**2)

        # keep walking until an intersection
        # dir=0,1,2; vox_index=3,4,5; pos=6,7,8; face_intersected=9; intersection=10, distance=11, particleID=12,
        # x_distance=13, y_distance=14, z_distance=15
        while np.sum(valid_mask) > 0:

            # save old position to compute distance increment
            old_pos = self.spherical_walkers[:, 6:9].copy()

            # compute new position:6,7,8 and face intersected:9 and new voxel indices:3,4,5
            self.__next_face_intersected(valid_mask)

            # compute distances
            self.spherical_walkers[valid_mask, 11] += np.linalg.norm(self.spherical_walkers[valid_mask, 6:9] - old_pos[valid_mask], axis=1)
            self.spherical_walkers[valid_mask, 13] += np.abs(self.spherical_walkers[valid_mask, 6] - old_pos[valid_mask, 0])
            self.spherical_walkers[valid_mask, 14] += np.abs(self.spherical_walkers[valid_mask, 7] - old_pos[valid_mask, 1])
            self.spherical_walkers[valid_mask, 15] += np.abs(self.spherical_walkers[valid_mask, 8] - old_pos[valid_mask, 2])

            # identify particles outside domain
            mask2 = (((np.any(self.spherical_walkers[:, 3:6] < 0, axis=1)) | (self.spherical_walkers[:, 3] > self.X - 1)) |
                     (self.spherical_walkers[:, 4] > self.Y - 1)) | (self.spherical_walkers[:, 5] > self.Z - 1)

            # boundary behavior: 0:kill particle, 1:periodic domain
            if self.boundary_behavior == 1:  # periodic BC for particles: re-enter the other side
                self.spherical_walkers[mask2, 3] %= self.X  # voxel indices
                self.spherical_walkers[mask2, 4] %= self.Y
                self.spherical_walkers[mask2, 5] %= self.Z
                zeros = self.spherical_walkers[:, 6] == 0  # particle positions
                self.spherical_walkers[zeros, 6] = self.X
                self.spherical_walkers[~zeros, 6] %= self.X
                zeros = self.spherical_walkers[:, 7] == 0
                self.spherical_walkers[zeros, 7] = self.Y
                self.spherical_walkers[~zeros, 7] %= self.Y
                zeros = self.spherical_walkers[:, 8] == 0
                self.spherical_walkers[zeros, 8] = self.Z
                self.spherical_walkers[~zeros, 8] %= self.Z
            else:  # kill particles
                self.spherical_walkers[mask2, 10] = 2

            # check material of next voxels
            mask2 = (~mask2) & (valid_mask)
            self.spherical_walkers[mask2, 10] = self.ws[self.spherical_walkers[mask2, 3].astype(int),
                                                        self.spherical_walkers[mask2, 4].astype(int),
                                                        self.spherical_walkers[mask2, 5].astype(int)] != self.valid_phase

            # valid particles: not collided i.e. 10=0 and travelled distance (11) less than 2x diagonal
            valid_mask = (self.spherical_walkers[:, 10] == 0) & (self.spherical_walkers[:, 11] < max_distance_void)

            if self.exportparticles_filepathname != '':
                mask_wallcollisions = self.spherical_walkers[:, 10] != 2
                if self.spherical_walkers[mask_wallcollisions].shape[0] != 0:
                    pointsToVTK(self.exportparticles_filepathname + "" + str(source_number) + "" + str(next(self.counter)),
                                self.spherical_walkers[mask_wallcollisions, 6].copy(),
                                self.spherical_walkers[mask_wallcollisions, 7].copy(),
                                self.spherical_walkers[mask_wallcollisions, 8].copy(),
                                data={"collision": self.spherical_walkers[mask_wallcollisions, 10].copy(),
                                      "distance": self.spherical_walkers[mask_wallcollisions, 11].copy(),
                                      "id": self.spherical_walkers[mask_wallcollisions, 12].copy()})

    def __next_face_intersected(self, valid_mask):
        # dir=0,1,2; vox=3,4,5; pos=6,7,8; face_intersected=9; intersection=10, distance=11
        n = self.spherical_walkers[:, 3:6].copy()  # next intersecting face in each direction
        t = np.full_like(n, 1e10)  # time to intersecting face in each direction

        for i in range(3):
            # Step 1: Setting nX, nY, and nZ to be the next interfaces reached for each direction
            n[(self.spherical_walkers[:, i] > 0) & (valid_mask), i] += 1

            # Step 2: Setting tX, tY, and tZ to be the time to reach the next interface in each direction
            mask = (self.spherical_walkers[:, i] != 0) & (valid_mask)
            t[mask, i] = (n[mask, i] - self.spherical_walkers[mask, 6 + i]) / self.spherical_walkers[mask, i]

        # Step 3: Setting the next position of the spherical_walker based on the nearest interface
        mask = ((t[:, 0] <= t[:, 1]) & (t[:, 0] <= t[:, 2])) & (valid_mask)
        self.spherical_walkers[mask, 6:10] = np.column_stack((n[mask, 0],
                                                              self.spherical_walkers[mask, 7] + self.spherical_walkers[mask, 1] * t[mask, 0],
                                                              self.spherical_walkers[mask, 8] + self.spherical_walkers[mask, 2] * t[mask, 0],
                                                              np.zeros((np.sum(mask), 1))))

        mask2 = ((t[:, 1] <= t[:, 2]) & (t[:, 1] <= t[:, 0])) & (valid_mask)
        self.spherical_walkers[mask2, 6:10] = np.column_stack((self.spherical_walkers[mask2, 6] + self.spherical_walkers[mask2, 0] * t[mask2, 1],
                                                               n[mask2, 1],
                                                               self.spherical_walkers[mask2, 8] + self.spherical_walkers[mask2, 2] * t[mask2, 1],
                                                               np.ones((np.sum(mask2), 1))))

        mask = ((~mask) & (~mask2)) & (valid_mask)
        self.spherical_walkers[mask, 6:10] = np.column_stack((self.spherical_walkers[mask, 6] + self.spherical_walkers[mask, 0] * t[mask, 2],
                                                              self.spherical_walkers[mask, 7] + self.spherical_walkers[mask, 1] * t[mask, 2],
                                                              n[mask, 2],
                                                              np.full((np.sum(mask), 1), 2)))

        # update voxel indices
        for i in range(3):
            mask = ((self.spherical_walkers[:, 9] == np.arange(3)[i]) & (self.spherical_walkers[:, i] > 0)) & (valid_mask)
            self.spherical_walkers[mask, 3 + i] += 1
            mask = ((self.spherical_walkers[:, 9] == np.arange(3)[i]) & (self.spherical_walkers[:, i] <= 0)) & (valid_mask)
            self.spherical_walkers[mask, 3 + i] -= 1

    def error_check(self):
        if not isinstance(self.ws, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        else:
            self.X = self.ws.matrix.shape[0]
            self.Y = self.ws.matrix.shape[1]
            self.Z = self.ws.matrix.shape[2]

        if 180 % self.degree_accuracy != 0:
            raise Exception("Ray separation can only be an exact divider of 180°")

        if not isinstance(self.source_locations, np.ndarray) or self.source_locations.shape[1] != 3:
            raise Exception("Source locations has to be a Numpy array of shape (NumberOfSource, 3).")
        self.rays_distances = np.zeros((self.source_locations.shape[0] * self.particles_number, 3))

        if self.exportparticles_filepathname != '':
            if not os.path.exists(os.path.split(self.exportparticles_filepathname)[0]):
                raise Exception("Directory " + os.path.split(self.exportparticles_filepathname)[0] + " not found.")

        if np.count_nonzero(self.ws.matrix == self.valid_phase) == 0:
            raise Exception("No valid voxels detected (i.e. ID={}), cannot run radiation ray tracing.".format(self.valid_phase))
