import numpy as np
from skimage.io import imsave
import pickle
from os import path
import json
import struct
import pyvista as pv
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.logger import print_warning
from pumapy.utilities.isosurface import generate_isosurface
from pumapy.io.input import io_logs


def export_vti(filename, dict_data, voxel_length=1):
    """ Export either a puma.Workspace or numpy array to vti

        :param filename: filepath and name
        :type filename: string
        :param dict_data: dictionary setup as {"name1": data1, "name2": data2 ...} containing either Workspaces or ndarrays
        :type dict_data: dict or Workspace or np.ndarray
        :param voxel_length: with voxel length to give to Numpy arrays (if any)
        :type voxel_length: float

        :Example:
        >>> import pumapy as puma
        >>> ws_vtk = puma.import_vti(puma.path_to_example_file("fibers_with_orientation.vti"))
        Importing ...
        >>> puma.export_vti("fibers_with_orientation.vti", ws_vtk)
        Exporting ...
    """

    # error path checks
    if not isinstance(filename, str):
        raise Exception("Filename has to be a string.")
    filename_split = path.split(filename)
    if filename_split[0] != '' and not path.exists(path.split(filename)[0]):
        raise Exception("Directory " + filename_split[0] + " not found.")
    if filename[-4:] != ".vti":
        filename += ".vti"

    print("Exporting " + filename + ".vti ... ", end='')

    if isinstance(dict_data, dict):
        if len(dict_data) == 0:
            raise Exception("Dictionary is empty.")
    elif isinstance(dict_data, Workspace) or isinstance(dict_data, np.ndarray):
        dict_data = {"data": dict_data}
    else:
        raise Exception("Data to export to vti needs to be either a pumapy.Workspace or Numpy array or dictionary of them.")

    grid = pv.ImageData()
    dimensions = None

    for name, data in dict_data.items():
        
        mat, orient = None, None
        if not (isinstance(data, Workspace) or isinstance(data, np.ndarray)):
            raise Exception("Data to export to vti needs to be either a pumapy.Workspace or Numpy array")
        if isinstance(data, Workspace):
            mat = data.matrix
            if data.orientation.shape[:3] == data.matrix.shape:
                orient = data.orientation
            dimensions = mat.shape[:3]
            if voxel_length is None: voxel_length = data.voxel_length
        else:
            if data.ndim == 2:
                data = np.expand_dims(data, axis=2)
                mat = data
            elif data.ndim == 3:
                mat = data
            elif data.ndim == 4 and data.shape[3] == 3:
                orient = data
            else:
                raise Exception("Numpy array has to be either 3D for scalars or 4D with shape[3]=3.")

            dimensions = data.shape[:3]

        grid.dimensions = np.array(dimensions) + 1
        
        if mat is not None:
            grid.cell_data[name] = mat.flatten(order="F")
        
        if orient is not None:
            grid.cell_data[name + "_orient"] = orient.reshape(-1, 3, order='F')

    grid.spacing = [voxel_length] * 3
    grid.save(filename)

    print("Done")


def export_3Dtiff(filename, ws_or_nparray, to8bit=True):
    """ Export either a puma.Workspace or numpy array to 3Dtiff

        :param filename: filepath and name
        :type filename: string
        :param ws_or_nparray: to be exported
        :type ws_or_nparray: Workspace or ndarray
        :param to8bit: if True, it converts the image to 8bit, otherwise 16bit is exported
        :type to8bit: bool

        :Example:
        >>> import pumapy as puma
        >>> ws_tiff = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"), 1.3e-6, import_ws=True)
        Importing ...
        >>> puma.export_3Dtiff("50_artfibers.tif", ws_tiff)
        Exporting ...
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
            data = data * 255. / data.max()
        data = data.astype(np.uint8)
    else:
        data = data.astype(np.uint16)

    print("Exporting " + filename + " ... ", end='')
    if data.ndim == 2:
        imsave(filename, data.transpose((1, 0)), check_contrast=False)
    else:
        imsave(filename, data.transpose((2, 1, 0)), check_contrast=False)
    print("Done")


def export_bin(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type: pumapy.Workspace

        :Example:
        >>> import pumapy as puma
        >>> ws_binary = puma.import_bin(puma.path_to_example_file("fibers_with_orientation.pumapy"))
        Importing ...
        >>> puma.export_bin("fibers_with_orientation.vti", ws_binary)
        Exporting ...
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

        :Example:
        >>> import pumapy as puma
        >>> ws_imported = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        Importing ...
        >>> puma.export_stl('200_fiberform', ws_imported, cutoff=(100, 255), flag_closed_edges=True)
        Exporting ...
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


def export_for_chfem(filename, ws, analysis, solver=0, export_nf=True, export_json=True, tol=1.e-6, max_iter=10000):
    """ Export a puma.Workspace to run an analysis in
        CHFEM_GPU CUDA kernels (https://gitlab.com/cortezpedro/chfem_gpu) or 
        CHPACK Julia package (https://gitlab.com/lcc-uff/Chpack.jl)

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type ws: pumapy.Workspace
        :param type_of_analysis: 0 = conductivity, 1 = elasticity, 2 = permeability
        :type type_of_analysis: int
        :param type_of_solver:  0 = pcg (default), 1 = cg, 2 = minres
        :type type_of_solver: int
        :param export_nf: export .nf file with simulations inputs for CHFEM_GPU
        :type export_nf: bool
        :param export_json: export .json file with simulations inputs for CHFPACK
        :type export_json: bool
        :param tol: solver tolerance for simulation
        :type tol: float
        :param max_iter: maximum number of iterations
        :type max_iter: int

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        Importing ...
        >>> puma.experimental.export_for_chfem('200_fiberform', ws, 2, tol=1e-6, max_iter=100000)
    """

    domain = ws.matrix.astype(np.uint8)

    if filename[-4:] != '.raw':
        filename = filename + '.raw'

    materials = {}
    # Save image data in RAW format
    with open(filename, "bw") as file_raw:
        for k in range(domain.shape[2]):
            mat_i, cmat_i = np.unique(domain[:, :, k], return_counts=True)
            for i in range(len(mat_i)):
                if mat_i[i] in materials:
                    materials[mat_i[i]] += cmat_i[i]
                else:
                    materials[mat_i[i]] = cmat_i[i]
            (domain[:, :, k].T).tofile(file_raw)

    materials = dict(sorted(materials.items(), key=lambda x: x[0]))
    
    mat = np.array(list(materials.keys()))
    if analysis != 2:
        mat = np.vstack((mat, np.zeros((mat.shape[0]), dtype=int))).T
    else:
        mat = np.expand_dims(mat, axis=1)
        
    cmat = np.array(list(materials.values()))
    cmat = cmat * 100. / np.prod(domain.shape)
    
    jdata = {}
    jdata["type_of_analysis"] = analysis
    jdata["type_of_solver"] = solver
    jdata["type_of_rhs"] = 0
    jdata["voxel_size"] = ws.voxel_length
    jdata["solver_tolerance"] = tol
    jdata["number_of_iterations"] = max_iter
    jdata["image_dimensions"] = list(domain.shape)
    jdata["refinement"] = 1
    jdata["number_of_materials"] = mat.shape[0]
    jdata["properties_of_materials"] = mat.tolist()
    jdata["volume_fraction"] = list(np.around(cmat, 2))
    jdata["data_type"] = "uint8"

    if export_json:
        # Save image data in JSON format
        with open(filename[:len(filename) - 4] + ".json", 'w') as file_json:
            json.dump(jdata, file_json, sort_keys=False, indent=4, separators=(',', ': '))

    if export_nf:
        # Save image data in NF format
        with open(filename[:len(filename) - 4] + ".nf", 'w') as file_nf:
            sText = ''
            for k, v in jdata.items():
                sText += '%' + str(k) + '\n' + str(v) + '\n\n'
            sText = sText.replace('], ', '\n')
            sText = sText.replace('[', '')
            sText = sText.replace(']', '')
            sText = sText.replace(',', '')
            file_nf.write(sText)


def export_sparta_implicit_surfaces(filename, ws):
    """ Export a puma.Workspace to binary (.pumapy extension)

        :param filename: filepath and name
        :type filename: string
        :param ws: to be exported
        :type ws: pumapy.Workspace
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
    # need to set boundary values equal to zero, using temporary matrix
    temp = ws.matrix
    # 2D
    if ws.len_z() == 1: 
        for k in range(ws.len_z()):
            for j in range(ws.len_y()):
                    for i in range(ws.len_x()):
                        if i == 0 or i == ws.len_x() - 1 or j == 0 or j == ws.len_y() - 1: 
                            temp[i, j] = 0 
        xyz = struct.pack("2i", ws.len_x(), ws.len_y())
        temp = temp.transpose(1,0,2)
    # 3D
    else: 
        for k in range(ws.len_z()):
            for j in range(ws.len_y()):
                for i in range(ws.len_x()):
                    if i == 0 or i == ws.len_x() - 1 or j == 0 or j == ws.len_y() - 1 or k == 0 or k == ws.len_z() - 1: 
                        temp[i, j, k] = 0
        xyz = struct.pack("3i", ws.len_x(), ws.len_y(), ws.len_z())
        temp = temp.transpose(2,1,0)
    b = bytearray(temp)
    b = b[::2]
    f = open(filename, 'w+b')
    f.write(xyz + b)
    f.close()

    print("Done")
    io_logs(ws, filename, input=False)
