"""
The following function was copied and modified from the porespy project (distributed under the MIT License).
See https://github.com/PMEAL/porespy/blob/dev/porespy/generators/_imgen.py for more information.
See https://github.com/PMEAL/porespy/blob/dev/LICENSE for the license.
"""

from scipy.ndimage import distance_transform_edt as edt
import numpy as np
from pumapy.utilities.workspace import Workspace
from pumapy.materialproperties.volumefraction import compute_volume_fraction
import sys


def generate_random_fibers(shape, radius, nfibers=None, porosity=None, phi=0, theta=90, length=None, max_iter=3):
    """ Generates random fibers from number of fibers or porosity

    :param shape: the size of the workspace to generate in (Nx, Ny, Nz) where N is the number of voxels.
    :type shape: tuple(int, int, int)
    :param radius: the radius of the fibers in voxels
    :type radius: int
    :param nfibers: the number of fibers to add to the domain. Adjust this value to control the final porosity,
        which is not easily specified since cylinders overlap and intersect different fractions of the domain
    :type nfibers: int, optional
    :param porosity: the targeted value for the porosity of the generated mat. The function uses an
        algorithm for predicted the number of required number of cylinder, and refines this over a certain number of
        fractional insertions (according to the 'iterations' input)
    :type porosity: float, optional
    :param phi: a value between 0 and 90 that controls the amount that the fibers lie *out of* the XY plane,
        with 0 meaning all fibers lie in the XY plane, and 90 meaning that cylinders are randomly oriented out of the
        plane by as much as +/- 90 degrees
    :type phi: float, optional
    :param theta: a value between 0 and 90 that controls the amount of rotation *in the* XY plane,
        with 0 meaning all fibers point in the X-direction, and 90 meaning they are randomly rotated about the
        Z axis by as much as +/- 90 degrees
    :type theta: float, optional
    :param length: the length of the cylinders to add.  If ``None`` (default) then the cylinders will extend beyond the
        domain in both directions so no ends will exist. If a scalar value is given it will be interpreted as the
        Euclidean distance between the two ends of the cylinder.  Note that one or both of the ends *may* still lie
        outside the domain, depending on the randomly chosen center point of the cylinder
    :type length: float, optional
    :param max_iter: the number of fractional fiber insertions used to target the requested porosity. By default a
        value of 3 is used (and this is typically effective in getting very close to the targeted porosity),
        but a greater number can be input to improve the achieved porosity
    :type max_iter: int, optional
    :return: random fibers domain
    :rtype: Workspace

    :Example:
    >>> import pumapy as puma
    >>> ws_fibers = puma.generate_random_fibers(shape=(200, 200, 200), radius=8, porosity=0.8, phi=90, theta=90, length=200)
    >>> puma.render_contour(ws_fibers, cutoff=(1, 1))
    >>> ws_fibers = puma.generate_random_fibers(shape=(200, 200, 200), radius=8, nfibers=100, phi=90, theta=90, length=200)
    >>> puma.render_contour(ws_fibers, cutoff=(1, 1))
    """

    # error checks
    if nfibers is None and porosity is None:
        raise Exception("'nfibers' and 'porosity' can't be both None")
    if max_iter < 3:
        raise Exception("Iterations must be >= 3")

    # run solver for provided number of fibers
    if nfibers is not None:
        img = _generate_fibers(shape=shape, radius=radius, nfibers=nfibers, phi=phi, theta=theta, length=length)

    else:  # porosity provided
        vol_total = float(np.prod(shape))

        length_estimate = vol_total ** (1 / 3) if length is None else length

        vol_fiber = length_estimate * np.pi * radius * radius
        n_pixels_to_add = -np.log(porosity) * vol_total

        n_fibers_added = 0

        subdif = 0.8 / np.sum(np.arange(1, max_iter) ** 2)
        fractions = [0.2]
        for i in range(1, max_iter):
            fractions.append(fractions[i - 1] + (max_iter - i) ** 2 * subdif)

        img = np.ones(shape, dtype=bool)
        for i, frac in enumerate(fractions):
            n_fibers_total = n_pixels_to_add / vol_fiber
            n_fibers = int(np.ceil(frac * n_fibers_total) - n_fibers_added)
            if n_fibers > 0:
                img = img & _generate_fibers(shape, radius, n_fibers, phi, theta, length)
            n_fibers_added += n_fibers

            void_vf = np.sum(img == 1)
            porosity = void_vf / (np.sum(img == 0) + void_vf)
            vol_added = -np.log(porosity) * vol_total
            vol_fiber = vol_added / n_fibers_added

            sys.stdout.write("\rGenerating fibers ... {:.1f}% ".format((i + 1) / len(fractions) * 100))

    img = np.where(img, np.uint16(0), np.uint16(1))
    img = Workspace.from_array(img.astype(np.uint16))
    print("\nGenerated random fibers domain with porosity: {}".format(compute_volume_fraction(img, (0, 0))))
    return img


def _generate_fibers(shape, radius, nfibers, phi=0, theta=90, length=None):
    """ Generates random fibers given nfibers"""

    # error checks
    shape = np.array(shape)
    if np.size(shape) == 1:
        shape = np.full((3, ), int(shape))
    elif np.size(shape) == 2:
        raise Exception("Shape can only be 3D")

    H = np.sqrt(np.sum(np.square(shape))).astype(int)
    if length is None:
        length = 2 * H
    R = min(int(length / 2), 2 * H)

    if (phi > 90) or (phi < 0):
        raise Exception('phi_max must be betwen 0 and 90')
    if (theta > 90) or (theta < 0):
        raise Exception('theta_max must be betwen 0 and 90')

    im = np.zeros(shape, dtype=bool)
    n = 0
    L = min(H, R)

    while n < nfibers:
        x = np.random.rand(3) * (shape + 2 * L)

        phi_rand = (np.pi / 2 - np.pi * np.random.rand()) * phi / 90
        theta_rand = (np.pi / 2 - np.pi * np.random.rand()) * theta / 90
        x0 = R * np.array([np.cos(phi_rand) * np.cos(theta_rand),
                           np.cos(phi_rand) * np.sin(theta_rand),
                           np.sin(phi_rand)])
        [x0, x1] = [x + x0, x - x0]

        x0 = np.around(x0).astype(int)
        x1 = np.around(x1).astype(int)
        L = np.amax(np.absolute([[x1[0] - x0[0]], [x1[1] - x0[1]], [x1[2] - x0[2]]])) + 1
        crds = [np.rint(np.linspace(x0[0], x1[0], L)).astype(int),
                np.rint(np.linspace(x0[1], x1[1], L)).astype(int),
                np.rint(np.linspace(x0[2], x1[2], L)).astype(int)]

        lower = ~np.any(np.vstack(crds).T < [L, L, L], axis=1)
        upper = ~np.any(np.vstack(crds).T >= shape + L, axis=1)
        valid = upper * lower
        if np.any(valid):
            im[crds[0][valid] - L, crds[1][valid] - L, crds[2][valid] - L] = 1
            n += 1

    im = np.array(im, dtype=bool)
    dt = edt(~im) < radius
    return ~dt
