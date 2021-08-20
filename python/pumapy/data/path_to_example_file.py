import os.path


def path_to_example_file(example_filename):
    """ Path to example data that is installed with pumapy (used for testing and tutorial)

    :param example_filename: name of the example file (check python/pumapy/data for a list of example files)
    :type example_filename: str
    :return: path to the example file, which can be used to import it
    :rtype: str
    """
    file_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), example_filename)

    if not os.path.isfile(file_path):
        raise Exception("Example file not found.")

    return file_path
