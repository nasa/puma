import os.path
import ntpath


def path_to_example_file(example_filename):
    """ Path to example data that is installed with pumapy (used for testing and tutorial)

        :param example_filename: name of the example file (check python/pumapy/data for a list of example files)
        :type example_filename: str
        :return: path to the example file, which can be used to import it
        :rtype: str

        :Example:
        >>> import pumapy as puma
        >>> ws_example = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif")) # import example file
        >>> puma.plot_slices(ws_example) # visualize example file
    """

    path = ntpath.split(os.path.dirname(os.path.realpath(__file__)))[0]
    file_path = os.path.join(path, 'data', example_filename)

    if not os.path.isfile(file_path):
        raise Exception(f"Example file not found, available files are: {[i for i in list_example_files()]}.")

    return file_path


def list_example_files():
    """ List all example files available inside the folder python/pumapy/data

        :return: List of example file names
        :rtype: list
    """

    path = ntpath.split(os.path.dirname(os.path.realpath(__file__)))[0]
    file_path = os.path.join(path, 'data')

    file_list = os.listdir(file_path)
    return_list = []
    for file in file_list:
        if file[-4:] == ".tif" or file[-7:] == ".pumapy" or file[-4:] == ".vti":
            return_list.append(file)

    return return_list
