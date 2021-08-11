import os.path


def path_to_example_file(example_filename):
    file_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), example_filename)

    if not os.path.isfile(file_path):
        raise Exception("Example file not found.")

    return file_path
