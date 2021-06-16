from pyevtk.hl import imageToVTK
from TexGen.Core import *
import numpy as np
from skimage.io import imsave
import pickle
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.logger import print_warning
from pumapy.utilities.isosurface import generate_isosurface
from os import path


def export_vti(filename, dict_data, voxel_length=None):
    """ Export either a puma.Workspace or numpy array to vti

    :param filename: filepath and name
    :type filename: string
    :param dict_data: dictionary setup as {"name1": data1, "name2": data2 ...} containing either Workspaces or ndarrays
    :type dict_data: dict
    :param voxel_length: with voxel length to give to Numpy arrays (if any)
    :type voxel_length: float, optional
    :return: True if successful, False otherwise.
    :rtype: bool
    """

    # error path checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if filename[-4:] == ".vti":
        filename = filename[:-4]

    print("Exporting " + filename + ".vti ... ", end='')

    if isinstance(dict_data, Workspace) or isinstance(dict_data, np.ndarray):
        dict_data = {"data": dict_data}

    dict_to_export = dict()

    for name, data in dict_data.items():

        if isinstance(data, Workspace):
            mat = data.matrix.copy()  # need to copy to make sure data is "continuous"
            dict_to_export[name] = mat

            if data.orientation.shape[:3] == data.matrix.shape:
                orientation = (data.orientation[:, :, :, 0].copy(),
                               data.orientation[:, :, :, 1].copy(),
                               data.orientation[:, :, :, 2].copy())
                dict_to_export[name] = mat
                dict_to_export[name + "_orient"] = orientation

            if voxel_length is None:
                voxel_length = data.voxel_length

        elif isinstance(data, np.ndarray):
            if voxel_length is None:
                voxel_length = 1e-6
            if data.ndim == 2:  # 2D numpy array to 3D
                data = np.expand_dims(data.copy(), axis=2)
            elif data.ndim == 3:
                data = data.copy()
            elif data.ndim == 4 and data.shape[3] == 3:
                data = (data[:, :, :, 0].copy(),
                        data[:, :, :, 1].copy(),
                        data[:, :, :, 2].copy())
            else:
                raise Exception("Numpy array has to be either 3D for scalars or 4D with shape[3]=3.")

            dict_to_export[name] = data
        else:
            raise Exception("Data to export to vti needs to be either a pumapy.Workspace or Numpy array")

    imageToVTK(filename, spacing=[voxel_length] * 3, cellData=dict_to_export)

    print("Done")
    return True


def export_3Dtiff(filename, ws_or_nparray, to8bit=False):
    """ Export either a puma.Workspace or numpy array to 3Dtiff

    :param filename: filepath and name
    :type filename: string
    :param ws_or_nparray: to be exported
    :type ws_or_nparray: Workspace or ndarray
    :param to8bit: if True, it converts the image to 8bit, otherwise 16bit is exported
    :type to8bit: bool, optional
    :return: True if successful, False otherwise.
    :rtype: bool
    """

    # error checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if isinstance(ws_or_nparray, Workspace):
        data = ws_or_nparray.matrix.copy()
    elif isinstance(ws_or_nparray, np.ndarray):
        data = ws_or_nparray.copy()
    else:
        raise Exception("Data to export to 3Dtiff needs to be either a pumapy.Workspace or Numpy array")

    if filename[-4:] != '.tif' and filename[-5:] != '.tiff':
        filename += '.tif'

    if to8bit:
        if data.max() > 255:
            print_warning("Data max is more than 255, normalizing to range 0-255.")
            data *= 255.0 / data.max()
        data = data.astype(np.uint8)
    else:
        data = data.astype(np.uint16)

    print("Exporting " + filename + " ... ", end='')
    imsave(filename, data.transpose((2, 1, 0)))
    print("Done")
    return True


def export_bin(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

    :param filename: filepath and name
    :type filename: string
    :param ws: to be exported
    :type: Workspace
    :return: True if successful, False otherwise.
    :rtype: bool
    """

    # error checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to bin needs to be a pumapy.Workspace")

    if filename[-7:] != ".pumapy":
        filename += '.pumapy'

    print("Exporting " + filename + " ... ", end='')
    output = open(filename, 'wb')
    pickle.dump(ws, output)
    output.close()
    print("Done")
    return True


def export_sparta_implicit_surfaces(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

    :param filename: filepath and name
    :type filename: string
    :param ws: to be exported
    :type ws: Workspace
    :return: True if successful, False otherwise.
    :rtype: bool
    """

    # error checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to bin needs to be a pumapy.Workspace")

    if filename[-7:] != ".pumapy.isurf":
        filename += '.pumapy.isurf'

    print("Exporting " + filename + " ... ", end='')

    if ws.len_z() == 1:
        f = open(filename, 'w+b')
        byte_arr = [ws.len_x(), ws.len_y()]
        binary_format = bytearray(byte_arr)
        f.write(binary_format)
        pickle.dump(ws.matrix, f)
        f.close()

    print("Done")
    return True


def export_stl(filename, ws, cutoff, flag_closed_edges=True, flag_gaussian=False):
    """ Export a puma.Workspace to STL

    :param filename: filepath and name
    :type filename: string
    :param ws: to be exported
    :type ws: Workspace
    :param cutoff: specify cutoff to binarize material
    :type cutoff: tuple(int, int)
    :param flag_closed_edges: close the surface edges on the boundaries
    :type flag_closed_edges: bool, optional
    :param flag_gaussian: apply Gaussian filter before creating surface
    :type flag_gaussian: bool, optional
    :return: True if successful, False otherwise.
    :rtype: bool
    """

    # error checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to stl needs to be a pumapy.Workspace")

    if filename[-4:] != ".stl":
        filename += '.stl'

    print("Exporting " + filename + " ... ", end='')
    mesh = generate_isosurface(ws, cutoff, flag_closed_edges, flag_gaussian)
    mesh.save(filename)
    print("Done")
    return True


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
