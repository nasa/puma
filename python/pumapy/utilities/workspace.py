import pumapy
from pumapy.utilities.logger import Logger, print_warning
import skimage.transform as trans
import numpy as np
from copy import deepcopy
from scipy.ndimage import rotate


class Workspace:
    def __init__(self, **kwargs):
        """ Workspace class holding the domain as a numpy matrix

            :param kwargs: keyword arguments
            - No arguments --> zero matrix of shape (1,1,1)
            - 'shape' argument --> then set all to zeros
            - 'value' --> value set all to full of value
            - 'nparray' --> set Workspace matrix as input array
            - 'scalars' --> bool indicating if a Workspace contains scalars stored in matrix variable
            - 'vectors' --> bool indicating if a Workspace contains vectors stored in orientation variable
            :type kwargs: dict
        """
        self.log = Logger()
        self.voxel_length = 1e-6

        # 3D numpy array holding the geometry (scalar)
        self.matrix = np.zeros((1, 1, 1), dtype=np.uint16)

        # 3D numpy array holding the orientation field (vectors)
        self.orientation = np.zeros((1, 1, 1, 3), dtype=float)

        # setting matrix
        if len(kwargs) > 0:
            if 'shape' in kwargs:
                if isinstance(kwargs['shape'], tuple) and len(kwargs['shape']) == 3:
                    if 'value' in kwargs:
                        self.matrix = np.full(kwargs['shape'], kwargs['value'], dtype=np.uint16)
                    else:
                        self.matrix = np.zeros(kwargs['shape'], dtype=np.uint16)
                else:
                    raise Exception("Wrong shape, tuple with 3 dimensions required.")

            elif 'nparray' in kwargs:
                if isinstance(kwargs['nparray'], np.ndarray):
                    if kwargs['nparray'].ndim == 3:
                        self.matrix = kwargs['nparray'].copy().astype(np.uint16)
                    else:
                        raise Exception("Wrong nparray ndim, 3 dimensions required.")
                else:
                    raise Exception("Wrong nparray type.")
            else:
                raise Exception("Unrecognized keyword.")

        # setting orientation
        if 'vectors' in kwargs and kwargs['vectors']:
            if not isinstance(kwargs['vectors'], bool):
                raise Exception("Orientation input is not a bool.")
            else:
                if 'shape' in kwargs:
                    if isinstance(kwargs['shape'], tuple) and len(kwargs['shape']) == 3:
                        if 'vectorvalue' in kwargs:
                            self.orientation = np.tile(kwargs['vectorvalue'], list(kwargs['shape']) + [1]).astype(float)
                        else:
                            self.orientation = np.zeros(list(kwargs['shape']) + [3], dtype=float)
                    else:
                        raise Exception("Wrong shape, tuple with 3 dimensions required.")
                else:
                    raise Exception("Unrecognized keyword.")

    @classmethod
    def from_shape(cls, shape, orientation=False):
        """ Generate workspace from shape, all matrix value set to zero.

            :param shape: shape of workspace to be created
            :type shape: (int, int, int)
            :param orientation: specify if workspace contains orientation
            :type orientation: bool
            :return: new workspace
            :rtype: pumapy.Workspace

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape((10, 11, 12))
            >>> puma.render_volume(ws, style='edges')
        """
        return cls(shape=shape, vectors=orientation)

    @classmethod
    def from_shape_value(cls, shape, value, orientation=False):
        """ Generate workspace from shape, all matrix values set to the value passed.

            :param shape: shape of workspace to be created
            :type shape: (int, int, int)
            :param value: value to be assigned to the matrix variable
            :type value: int
            :param orientation: specify if workspace contains orientation
            :type orientation: bool
            :return: new workspace
            :rtype: pumapy.Workspace

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape_value((20, 31, 212), 1)
            >>> puma.render_volume(ws, style='edges')
        """
        return cls(shape=shape, value=value, vectors=orientation)

    @classmethod
    def from_shape_vector(cls, shape, vector):
        """ Generate workspace from shape, all orientation vectors set to the vector passed.

            :param shape: shape of workspace to be created
            :type shape: (int, int, int)
            :param vector: vector to be assigned to the orientation variable
            :type vector: (float, float, float)
            :return: new workspace with orientation
            :rtype: pumapy.Workspace

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape_vector((5, 6, 2), (0.4, 2, 5))
            >>> puma.render_orientation(ws)
        """
        return cls(shape=shape, vectorvalue=vector, vectors=True)

    @classmethod
    def from_shape_value_vector(cls, shape, value, vector):
        """ Generate workspace from shape, all matrix and orientation set to the values passed.

            :param shape: shape of workspace to be created
            :type shape: (int, int, int)
            :param value: value to be assigned to the matrix variable
            :type value: int
            :param vector: vector to be assigned to the orientation variable
            :type vector: (float, float, float)
            :return: new workspace with orientation
            :rtype: pumapy.Workspace

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape_value_vector((5, 6, 2), 1, (0.4, 2, 5))
            >>> puma.render_orientation(ws)
        """
        return cls(shape=shape, value=value, vectorvalue=vector, vectors=True)

    @classmethod
    def from_array(cls, nparray):
        """ Generate workspace matrix from numpy array.

            :param nparray: array of shape (X,Y,Z) to be assigned to the matrix variable. NB: array is turned into type uint16
            :type nparray: np.ndarray
            :return: new workspace
            :rtype: pumapy.Workspace

            :Example:
            >>> import pumapy as puma
            >>> array = np.random.randint(5, size=(10, 10, 10))
            >>> ws = puma.Workspace.from_array(array)
            >>> puma.render_volume(ws, style='edges')
        """
        return cls(nparray=nparray, vectors=False)

    def set_voxel_length(self, voxel_length):
        """ Set voxel size, which by default is set to 1e-6

            :param voxel_length: size of a voxel side
            :type voxel_length: float
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> array = np.random.randint(5, size=(10, 10, 10))
            >>> ws = puma.Workspace.from_array(array)
            >>> ws.set_voxel_length(1)  # equivalent to ws.voxel_length = 1
            >>> puma.render_volume(ws, style='edges')
        """
        if not isinstance(voxel_length, int) and not isinstance(voxel_length, float):
            raise Exception("Voxel_length needs to be an int or float, got " + str(type(voxel_length)))
        else:
            self.voxel_length = voxel_length

    def set_matrix(self, nparray):
        """ Set matrix with numpy array

            :param nparray: array of shape (X,Y,Z) to be assigned to the matrix variable
            :type nparray: np.ndarray
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace()
            >>> nparray = np.random.randint(5, size=(10, 10, 10))
            >>> ws.set_matrix(nparray)  # equivalent to ws.matrix = nparray.copy().astype(np.uint16)
            >>> puma.render_volume(ws, style='edges')
        """
        if isinstance(nparray, np.ndarray):
            if nparray.ndim == 3:
                self.matrix = nparray.copy().astype(np.uint16)
            else:
                raise Exception("Wrong nparray ndim, 3 dimensions required. Leaving matrix unchanged")
        else:
            print_warning("Wrong nparray type. Leaving matrix unchanged")

    def set_orientation(self, nparray):
        """ Set orientation with numpy array

            :param nparray: array of shape (X,Y,Z, 3) to be assigned to the orientation variable
            :type nparray: np.ndarray
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace()
            >>> ws.set_orientation(np.random.rand(10, 10, 10, 3))
            >>> puma.render_orientation(ws, solid_color=None)
        """
        if isinstance(nparray, np.ndarray):
            if nparray.ndim == 4 and nparray.shape[3] == 3:
                self.orientation = nparray.copy().astype(float)
            else:
                raise Exception("Wrong nparray ndim, 4 dimensions required as (x,y,z,3). Leaving orientation unchanged")
        else:
            raise Exception("Wrong nparray type. Leaving orientation unchanged")

    def copy(self):
        """ Create a copy of the workspace

            :return: copy of workspace
            :rtype: pumapy.Workspace
        """
        return deepcopy(self)

    def __getitem__(self, key):
        return self.matrix[key]

    def __setitem__(self, key, value):
        self.matrix[key] = value

    def get_size(self):
        """ Return size

            :return: number of voxels
            :rtype: int
        """
        return self.matrix.size

    def len_x(self):
        """ Return x dimension length

            :return: number of voxels in x dimension
            :rtype: int
        """
        return self.matrix.shape[0]

    def len_y(self):
        """ Return y dimension length

            :return: number of voxels in y dimension
            :rtype: int
        """
        return self.matrix.shape[1]

    def len_z(self):
        """ Return z dimension length

            :return: number of voxels in z dimension
            :rtype: int
        """
        return self.matrix.shape[2]

    def get_shape(self):
        """ Return shape of domain

            :return: shape of domain (matrix)
            :rtype: (int, int, int)
        """
        return self.matrix.shape

    def min(self):
        """ Return minimum of domain

            :return: minimum
            :rtype: int
        """
        return np.min(self.matrix)

    def max(self):
        """ Return maximum of domain

            :return: maximum
            :rtype: int
        """
        return np.max(self.matrix)

    def average(self):
        """ Return average of domain

            :return: grayscale average
            :rtype: float
        """
        return self.matrix.mean()

    def unique_values(self):
        """ Return unique values in domain matrix variable

            :return: unique values
            :rtype: np.ndarray
        """
        return np.unique(self.matrix)

    def unique_values_counts(self):
        """ Return unique values and their counts in domain matrix variable

            :return: unique values and counts
            :rtype: (np.ndarray, int)
        """
        return np.unique(self.matrix, return_counts=True)

    def orientation_magnitude(self):
        """ Return orientation vector's magnitude

            :return: orientation field magnitude
            :rtype: np.ndarray
        """
        return np.linalg.norm(self.orientation, axis=3)

    def resize_new_matrix(self, shape, value=None):
        """ Resize matrix numpy array

            :param shape: shape of workspace to be resized
            :type shape: (int, int, int)
            :param value: value to be assigned to the new resized matrix variable
            :type value: int

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace()
            >>> ws.resize_new_matrix((10, 10, 10), value=3)
            >>> puma.render_volume(ws, style='edges')
        """
        if isinstance(shape, tuple) and len(shape) == 3:
            if value is None:
                self.matrix = np.zeros(shape, dtype=np.uint16)
            else:
                self.matrix = np.full(shape, value, dtype=np.uint16)
        else:
            raise Exception("Wrong shape, tuple with 3 dimensions required.")

    def resize_new_orientation(self, shape, orientation_value=None):
        """ Resize orientation numpy array

            :param shape: shape of workspace to be resized
            :type shape: (int, int, int)
            :param orientation_value: vector to be assigned to the new resized orientation variable
            :type orientation_value: (float, float, float)

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace()
            >>> ws.resize_new_orientation((10, 10, 10), orientation_value=(1., 0., 0.))
            >>> puma.render_orientation(ws)
        """
        if isinstance(shape, tuple) and len(shape) == 3:
            if orientation_value is None:
                self.orientation = np.zeros(list(shape) + [3], dtype=float)
            else:
                if isinstance(orientation_value, tuple) and len(orientation_value) == 3:
                    self.orientation = np.tile(orientation_value, list(shape) + [1]).astype(float)
                else:
                    raise Exception("vectorvalue has to be (float, float, float).")
        else:
            raise Exception("Wrong shape, tuple with 3 dimensions required.")

    def create_orientation(self):
        """ Create orientation field of the same size as the matrix 

            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape((10, 10, 10))
            >>> ws.create_orientation()
            >>> print(ws.orientation.shape)
        """
        self.orientation = np.zeros(list(self.matrix.shape) + [3], dtype=float)

    def resize(self, shape, segmented, anti_aliasing=True, interpolation_order=1):
        """ Resize both matrix and orientation (if present) by rescaling the content to specified size

            :param shape: shape of workspace to be resized
            :type shape: (int, int, int)
            :param segmented: specifying whether the domain is already segmented (True) or grayscales (False)
            :type segmented: bool
            :param anti_aliasing: if aliasing is to be prevented applying a Gaussian filter to smooth
                before scaling. If domain is segmented, automatically set to False in order to preserve domain
            :type anti_aliasing: bool
            :param interpolation_order: order of the interpolation spline used.
                For segmented, it is enforced to be 0,which is 'nearest neighbor' to preserve the segmentation
            :type interpolation_order: int
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws2 = ws_fiberform.copy()
            >>> ws2.resize((100, 200, 200), segmented=False)
            >>> puma.compare_slices(ws_fiberform, ws2)
        """
        if isinstance(shape, tuple) and len(shape) == 3:
            if self.orientation.shape[:3] == self.matrix.shape:
                self.orientation = trans.resize(self.orientation, list(shape) + [3], order=0,
                                                preserve_range=True, anti_aliasing=False)
            if segmented:
                self.matrix = trans.resize(self.matrix, shape, order=0, anti_aliasing=False, preserve_range=True)
            else:
                self.matrix = trans.resize(self.matrix, shape, order=interpolation_order,
                                           anti_aliasing=anti_aliasing, preserve_range=True)
            self.matrix = self.matrix.astype('uint16')
        else:
            raise Exception("Wrong shape, tuple with 3 dimensions required.")

    def rescale(self, scale, segmented, anti_aliasing=True, interpolation_order=1):
        """ Rescale both matrix and orientation (if present) by rescaling the content by a specified factor

            :param scale: specifying the scaling factor
            :type scale: float
            :param segmented: specifying whether the domain is already segmented (True) or grayscales (False)
            :type segmented: bool
            :param anti_aliasing: if aliasing is to be prevented applying a Gaussian filter
                to smooth before scaling. If domain is segmented, automatically set to False in order to preserve domain
            :type anti_aliasing: bool
            :param interpolation_order: order of the interpolation spline used.
                For segmented, it is enforced to be 0, which is 'nearest neighbor' to preserve the segmentation
            :type interpolation_order: int
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws2 = ws_fiberform.copy()
            >>> ws2.rescale(0.5, segmented=False)
            >>> puma.compare_slices(ws_fiberform, ws2)  # pay attention to the shape
        """

        unit_dim_check = None
        if min(self.get_shape()) == 1:
            if self.len_x() == 1:
                unit_dim_check = 0
            elif self.len_y() == 1:
                unit_dim_check = 1
            elif self.len_z() == 1:
                unit_dim_check = 2
            self.matrix = np.squeeze(self.matrix)

        if self.orientation.shape[:3] == self.matrix.shape:
            self.orientation = trans.rescale(self.orientation, scale, order=0, multichannel=True,
                                             preserve_range=True, anti_aliasing=False)
        if segmented:
            self.matrix = trans.rescale(self.matrix, scale, order=0, anti_aliasing=False, preserve_range=True)
        else:
            self.matrix = trans.rescale(self.matrix, scale, order=interpolation_order,
                                        anti_aliasing=anti_aliasing, preserve_range=True)

        if unit_dim_check is not None:
            self.matrix = np.expand_dims(self.matrix, axis=unit_dim_check)

        self.matrix = self.matrix.astype('uint16')
        self.voxel_length /= scale
        print("Rescaled workspace size: {}".format(self.get_shape()))

    def set(self, matrix_value=None, orientation_value=None):
        """ Set all elements in matrix equal to value (and orientation to vectorvalue is passed)

            :param matrix_value: value to fill to the matrix variable. NB this value will be turned into np.uint16
            :type matrix_value: int
            :param orientation_value: vector to fill to the orientation variable
            :type orientation_value: ((float, float, float))
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape_value((10, 10, 10), 5)
            >>> print(ws.unique_values())
            >>> ws.set(matrix_value=4)
            >>> print(ws.unique_values())
        """
        check = True
        if matrix_value is not None:
            if np.issubdtype(type(matrix_value), np.integer) and 0 <= matrix_value <= 65535:
                self.matrix.fill(np.uint16(matrix_value))
                check = False
            else:
                raise Exception("matrix_value has to be np.uint16 (i.e. 0 <= matrix_value <= 65535).")
        if orientation_value is not None:
            if isinstance(orientation_value, tuple) and len(orientation_value) == 3:
                if self.orientation.shape[:3] != self.matrix.shape:
                    self.resize_new_orientation(self.matrix.shape, orientation_value=orientation_value)
                else:
                    self.orientation[:] = orientation_value
                check = False
            else:
                raise Exception("orientation_value has to be (float, float, float).")
        if check:
            print_warning("No changes have been made to the Workspace.")

    def apply_mask(self, mask, apply_to_orientation=False):
        """ Apply mask of same size as the matrix by leaving the mask's 1s unchanged and setting mask's 0s to 0

            :param mask: array of type bool with same size as matrix
            :type mask: np.ndarray
            :param apply_to_orientation: specifying whether the mask is to be applied to the orientation (if present)
            :type apply_to_orientation: bool
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.Workspace.from_shape_value_vector((10, 10, 10), 1, (0.4, 2, 5))
            >>> mask = np.random.randint(255, size=(10, 10, 10)) > 100
            >>> ws.apply_mask(mask, apply_to_orientation=True)
            >>> puma.render_volume(ws, style='edges')
        """
        if isinstance(mask, np.ndarray) and mask.dtype == 'bool':
            if mask.shape == self.matrix.shape:
                self.matrix[~mask] = 0
                if self.orientation.shape[:3] == self.matrix.shape and apply_to_orientation:
                    self.orientation[~mask] = 0
            else:
                raise Exception("The mask has to be of the same size as the Workspace matrix.")
        else:
            raise Exception("The mask has to be a Numpy array of type bool.")

    def set_material_id(self, cutoff, value):
        """ Threshold the workspace according to cutoff (i.e. tuple with inclusive range to set)

            :param cutoff: convert a range of grayscale values specified by the cutoff into an single ID number
            :type cutoff: (int, int)
            :param value: ID number to assign to range
            :type value: int
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws2 = ws_fiberform.copy()
            >>> ws2.set_material_id((0, 100), 0)    # NB the order of these operations is important!
            >>> ws2.set_material_id((100, 255), 1)  # this is why binarize and binarize_range should be preferred
            >>> puma.compare_slices(ws_fiberform, ws2)
        """
        if value < 0:
            raise Exception("Value ID can only be positive. Leaving matrix unchanged.")
        if value > 1000000:
            raise Exception("Value ID cannot be > 1000000. Leaving matrix unchanged.")

        self.matrix = np.where(np.logical_and(self.matrix >= cutoff[0], self.matrix <= cutoff[1]), np.uint16(value), self.matrix)

        self.log.log_section("Set Material ID")
        self.log.log_line(str(cutoff) + " -> " + str(value))
        self.log.write_log()

    def binarize(self, threshold):
        """ Binarize the workspace according to threshold, inclusive for higher range set to 1, lower to 0

            :param threshold: grayscale value dividing the domain into 0s and 1s (threshold turns into 1)
            :type threshold: int
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws2 = ws_fiberform.copy()
            >>> ws2.binarize(100)
            >>> puma.compare_slices(ws_fiberform, ws2)
        """
        self.matrix = np.where(self.matrix < threshold, np.uint16(0), np.uint16(1))

        self.log.log_section("Binarize domain")
        self.log.log_line(">" + str(threshold) + " -> 1; 0 otherwise")
        self.log.write_log()

    def binarize_range(self, ones_cutoff):
        """ Binarize the workspace according to range within cutoff, inclusive for cutoff ints set to 1, rest to 0

            :param ones_cutoff: convert a range of grayscale values specified by the cutoff into 1s, rest into 0s
            :type ones_cutoff: (int, int)
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws2 = ws_fiberform.copy()
            >>> ws2.binarize_range((100, 255))
            >>> puma.compare_slices(ws_fiberform, ws2)
        """
        self.matrix = np.where(np.logical_and(self.matrix >= ones_cutoff[0], self.matrix <= ones_cutoff[1]), np.uint16(1), np.uint16(0))

        self.log.log_section("Binarize domain")
        self.log.log_line(str(ones_cutoff[0]) + " < matrix < " + str(ones_cutoff[1]) + " -> 1; 0 otherwise")
        self.log.write_log()

    def rotate(self, degrees, around_axis, reshape=True, boundary_mode='reflect', apply_to_orientation=True):
        """ Rotate domain by specified degrees

            :param degrees: degrees to rotate domain
            :type degrees: float
            :param around_axis: specify around what axis to perform the rotation. It can be 'x', 'y' or 'z'
            :type around_axis: string
            :param reshape: specify whether to reshape the domain (and therefore contain every voxel - reshape=True)
                            or keep its original size (reshape=False)
            :type reshape: bool
            :param boundary_mode: specifying what to do with the boundaries.
                Options: ‘reflect’, ‘constant’, ‘nearest’, ‘mirror’, ‘wrap’
            :type boundary_mode: string
            :param apply_to_orientation: specify whether to apply rotation to the orientation, if present
            :type apply_to_orientation: bool
            :return: None

            :Example:
            >>> import pumapy as puma
            >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
            >>> ws_copy = ws_fiberform.copy()
            >>> ws_copy.rotate(45, 'z', reshape=False, boundary_mode='reflect')
            >>> puma.compare_slices(ws_fiberform, ws_copy)
            >>> ws_copy = ws_fiberform.copy()
            >>> ws_copy.rotate(45, 'z', reshape=True, boundary_mode='constant')
            >>> puma.compare_slices(ws_fiberform, ws_copy)
            >>> ws_copy = ws_fiberform.copy()
            >>> ws_copy.rotate(45, 'z', reshape=True, boundary_mode='reflect')
            >>> puma.compare_slices(ws_fiberform, ws_copy)
        """
        if around_axis == 'x':
            axes = (1, 2)
        elif around_axis == 'y':
            axes = (0, 2)
        elif around_axis == 'z':
            axes = (0, 1)
        else:
            raise Exception("Axis not recognized: around_axis can only be 'x', 'y' or 'z'")

        self.matrix = rotate(self.matrix, angle=degrees, axes=axes, mode=boundary_mode, reshape=reshape)

        if self.orientation.shape[:3] == self.matrix.shape and apply_to_orientation:
            from scipy.spatial.transform import Rotation
            rotation_degrees = degrees
            rotation_radians = np.radians(rotation_degrees)
            if around_axis == 'x':
                rotation_axis = np.array([1, 0, 0])
            elif around_axis == 'y':
                rotation_axis = np.array([0, 1, 0])
            else:
                rotation_axis = np.array([0, 0, 1])
            rotation_vector = rotation_radians * rotation_axis
            rotation = Rotation.from_rotvec(rotation_vector)
            for i in range(self.len_x()):
                for j in range(self.len_y()):
                    for k in range(self.len_z()):
                        self.orientation[i, j, k] = rotation.apply(self.orientation[i, j, k])

            self.orientation = rotate(self.orientation, angle=degrees, axes=axes, mode=boundary_mode,
                                      reshape=reshape, order=0)

    def porosity(self, cutoff=(0, 0)):
        """ Compute porosity of domain

            :param cutoff: void cutoff
            :type cutoff: (int, int)
            :return: volume fraction
            :rtype: float

            :Example:
            >>> import pumapy as puma
            >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
            >>> ws.porosity(cutoff=(0, 89)))
        """
        return pumapy.compute_volume_fraction(self, cutoff)


    def show_matrix(self):
        """ Print content of matrix domain's variable """
        if isinstance(self, Workspace):
            x, y, z = self.matrix.shape
        elif isinstance(self, np.ndarray):
            x, y, z = self.shape
        else:
            raise Exception("Print can only be called on a Workspace or Numpy array.")
        print()
        print("3D Workspace:")
        # Printing coordinate system used
        print("  o---> y")
        print("  |")
        print("x v")
        print('[', end='')
        for k in range(z):
            print("(:,:,{})".format(k))
            print('[', end='')
            for i in range(x):
                print('[', end='')
                for j in range(y):
                    print(self[i, j, k], end='')
                    if j != y - 1:
                        print(' ', end='')
                print(']', end='')
                if i != x - 1:
                    print()
            if k != z - 1:
                print()
                print()
        print(']')

    def show_orientation(self, dec=1):
        """ Print content of orientation domain's variable """
        if isinstance(self, Workspace):
            dims = self.matrix.shape
        elif isinstance(self, np.ndarray):
            if self.shape[3] == 3:
                dims = self.shape
            else:
                raise Exception("Numpy array has to be of size (x,y,z,3,...).")
        else:
            raise Exception("Print can only be called on a Workspace or Numpy array.")
        print()
        print("3D Orientation:")
        # Printing coordinate system used
        print("  o---> y")
        print("  |")
        print("x v")
        print('[', end='')
        if isinstance(self, Workspace):
            for k in range(dims[2]):
                print("(:,:,{})".format(k))
                print('[', end='')
                for i in range(dims[0]):
                    print('[', end='')
                    for j in range(dims[1]):
                        print('({:.{}f}, {:.{}f}, {:.{}f})'.format(self.orientation[i, j, k, 0], dec,
                                                                   self.orientation[i, j, k, 1], dec,
                                                                   self.orientation[i, j, k, 2], dec), end='')
                        if j != dims[1] - 1:
                            print(' ', end='')
                    print(']', end='')
                    if i != dims[0] - 1:
                        print()
                if k != dims[2] - 1:
                    print()
                    print()
        else:
            for k in range(dims[2]):
                print("(:,:,{})".format(k))
                print('[', end='')
                for i in range(dims[0]):
                    print('[', end='')
                    for j in range(dims[1]):
                        print('({}, {}, {})'.format(self[i, j, k, 0],
                                                    self[i, j, k, 1],
                                                    self[i, j, k, 2]), end='')
                        if j != dims[1] - 1:
                            print(' ', end='')
                    print(']', end='')
                    if i != dims[0] - 1:
                        print()
                if k != dims[2] - 1:
                    print()
                    print()
        print(']')
