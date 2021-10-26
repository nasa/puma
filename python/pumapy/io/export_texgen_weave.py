from TexGen.Core import *
import numpy as np
from os import path


def export_weave_vtu(filename, weave, domain, max_dim_nvox, round_vox_up=True, export_orientation=True):
    """ Exporting weave to vtu, to be read by pumapy

        :param filename: filepath and name
        :type filename: string
        :param weave: weave object, as defined in TexGen
        :type weave: CTextile or child class of CTextile
        :param domain: domain size object, as defined in TexGen
        :type domain: CDomainPlanes
        :param max_dim_nvox: number of voxels to add in the largest domain dimension
        :type max_dim_nvox: int
        :param round_vox_up: for the shorter dimensions, round number of voxels up (for +/-1 vox)
        :type round_vox_up: bool
        :param export_orientation: specify whether to export orientation
        :type export_orientation: bool
        :return: filename of weave exported (input filename + dimensions)
        :rtype: string
    """

    if not isinstance(domain, CDomainPlanes):
        raise Exception("Domain needs to be of CDomainPlanes type.")
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    if not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + path.split(filename)[0] + " not found.")

    min_bounds = XYZ()
    max_bounds = XYZ()
    domain.GetBoxLimits(min_bounds, max_bounds)

    weave.AssignDomain(CDomainPlanes(min_bounds, max_bounds))

    lengths = np.array([max_bounds.x - min_bounds.x, max_bounds.y - min_bounds.y, max_bounds.z - min_bounds.z])
    max_len = np.max(lengths)

    mask = np.zeros(3, dtype=bool)
    mask[lengths == max_len] = True

    voxel_length = max_len / float(max_dim_nvox)

    nvox = np.zeros(3, dtype=int)
    nvox[mask] = max_dim_nvox
    nvox[~mask] = (lengths[~mask] / voxel_length).astype(int)  # truncates

    rem = np.zeros(3, dtype=float)
    rem[~mask] = lengths[~mask] - voxel_length * nvox[~mask]

    if round_vox_up:
        rem[~mask] = voxel_length - rem[~mask]
        max_bounds = XYZ(max_bounds.x + rem[0],
                         max_bounds.y + rem[1],
                         max_bounds.z + rem[2])
        nvox[~mask] += 1
    else:
        max_bounds = XYZ(max_bounds.x - rem[0], max_bounds.y - rem[1], max_bounds.z - rem[2])
    weave.AssignDomain(CDomainPlanes(min_bounds, max_bounds))

    mesh = CRectangularVoxelMesh()
    print("Exporting " + filename + ".vtu ... ", end='')
    filename += "_" + str(nvox[0]) + "_" + str(nvox[1]) + "_" + str(nvox[2])
    mesh.SaveVoxelMesh(weave, filename, int(nvox[0]), int(nvox[1]), int(nvox[2]), False, export_orientation,
                       MATERIAL_CONTINUUM, 0, VTU_EXPORT)
    print("Done")
    return filename
