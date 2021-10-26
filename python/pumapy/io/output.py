from pyevtk.hl import imageToVTK
import numpy as np
from skimage.io import imsave
import pickle
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.logger import print_warning
from pumapy.utilities.isosurface import generate_isosurface
from pumapy.io.input import io_logs
from os import path


def export_vti(filename, dict_data, voxel_length=None):
    """ Export either a puma.Workspace or numpy array to vti

        :param filename: filepath and name
        :type filename: string
        :param dict_data: dictionary setup as {"name1": data1, "name2": data2 ...} containing either Workspaces or ndarrays
        :type dict_data: dict
        :param voxel_length: with voxel length to give to Numpy arrays (if any)
        :type voxel_length: float
        :return: True if successful, False otherwise.
        :rtype: bool

        :Example:
        >>> import pumapy as puma
        >>> ws_vtk = puma.import_vti(puma.path_to_example_file("fibers_with_orientation.vti"))
        >>> puma.export_vti("fibers_with_orientation.vti", ws_vtk)
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
    counter = 0

    for name, data in dict_data.items():

        if isinstance(data, Workspace):
            if counter == 0:
                io_logs(data, filename, input=False)
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
            counter += 1

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
        :type to8bit: bool
        :return: True if successful, False otherwise.
        :rtype: bool

        :Example:
        >>> import pumapy as puma
        >>> ws_tiff = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"), 1.3e-6, import_ws=True)
        >>> puma.export_3Dtiff("50_artfibers.tif", ws_tiff)
    """

    # error checks
    if filename[-4:] != '.tif' and filename[-5:] != '.tiff':
        filename += '.tif'
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if isinstance(ws_or_nparray, Workspace):
        io_logs(ws_or_nparray, filename, input=False)
        data = ws_or_nparray.matrix.copy()
    elif isinstance(ws_or_nparray, np.ndarray):
        data = ws_or_nparray.copy()
    else:
        raise Exception("Data to export to 3Dtiff needs to be either a pumapy.Workspace or Numpy array")

    if to8bit:
        if data.max() > 255:
            print_warning("Data max is more than 255, normalizing to range 0-255.")
            data *= 255.0 / data.max()
        data = data.astype(np.uint8)
    else:
        data = data.astype(np.uint16)

    print("Exporting " + filename + " ... ", end='')
    if data.ndim == 2:
        imsave(filename, data.transpose((1, 0)), check_contrast=False)
    else:
        imsave(filename, data.transpose((2, 1, 0)), check_contrast=False)
    print("Done")
    return True


def export_bin(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type: pumapy.Workspace
        :return: True if successful, False otherwise.
        :rtype: bool

        :Example:
        >>> import pumapy as puma
        >>> ws_binary = puma.import_bin(puma.path_to_example_file("fibers_with_orientation.pumapy"))
        >>> puma.export_bin("fibers_with_orientation.vti", ws_binary)
    """

    # error checks
    if filename[-7:] != ".pumapy":
        filename += '.pumapy'
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to bin needs to be a pumapy.Workspace")

    print("Exporting " + filename + " ... ", end='')
    output = open(filename, 'wb')
    pickle.dump(ws, output)
    output.close()
    print("Done")
    io_logs(ws, filename, input=False)
    return True


def export_sparta_implicit_surfaces(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type ws: pumapy.Workspace
        :return: True if successful, False otherwise.
        :rtype: bool
    """

    # error checks
    if filename[-7:] != ".pumapy.isurf":
        filename += '.pumapy.isurf'
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to bin needs to be a pumapy.Workspace")

    print("Exporting " + filename + " ... ", end='')

    if ws.len_z() == 1:
        f = open(filename, 'w+b')
        byte_arr = [ws.len_x(), ws.len_y()]
        binary_format = bytearray(byte_arr)
        f.write(binary_format)
        pickle.dump(ws.matrix, f)
        f.close()

    print("Done")
    io_logs(ws, filename, input=False)
    return True


def export_stl(filename, ws, cutoff, flag_closed_edges=True, flag_gaussian=False, binary=True):
    """ Export a puma.Workspace to STL

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type ws: pumapy.Workspace
        :param cutoff: specify cutoff to binarize material
        :type cutoff: (int, int)
        :param flag_closed_edges: close the surface edges on the boundaries
        :type flag_closed_edges: bool
        :param flag_gaussian: apply Gaussian filter before creating surface
        :type flag_gaussian: bool
        :return: True if successful, False otherwise.
        :rtype: bool

        :Example:
        >>> import pumapy as puma
        >>> ws_imported = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        >>> puma.export_stl('200_fiberform', ws_imported, cutoff=(100, 255), flag_closed_edges=True)
    """

    # error checks
    if filename[-4:] != ".stl":
        filename += '.stl'
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if not isinstance(ws, Workspace):
        raise Exception("Data to export to stl needs to be a pumapy.Workspace")

    print("Exporting " + filename + " ... ", end='')
    mesh = generate_isosurface(ws, cutoff, flag_closed_edges, flag_gaussian)
    mesh.save(filename, binary)
    print("Done")
    io_logs(ws, filename, input=False)
    return True
