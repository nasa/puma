import os
import sys
sys.path.insert(0, os.path.abspath('../..'))
sys.path.insert(0, os.path.abspath('../../python'))
sys.path.insert(0, os.path.abspath('../../cpp'))

# updating tutorial list nblinks
import glob
filepathnames = glob.glob("../../tutorials/python/jupyter_notebooks/*.ipynb")
os.system("mkdir -p python_tutorials")
for filepathname in filepathnames:
    filename = os.path.split(filepathname)[1]
    with open(f'python_tutorials/{filename[:-5]}nblink', 'w') as f:
        f.write('{\n"path": "../../../tutorials/python/jupyter_notebooks/' + filename + '"\n}')

# compile .so for cython files
os.system("cd ../.. && python setup.py build_ext --inplace")

# run api-doc in terminal
os.system("sphinx-apidoc -fMT ../../python/pumapy -o python_api --templatedir=template")


# -- Project information -----------------------------------------------------

project = 'PuMA'
copyright = '2021, PuMA Team'
author = 'PuMA Team'

# The full version, including alpha/beta/rc tags
release = '3.0.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'sphinx.ext.mathjax',
    'sphinx.ext.viewcode',
    'nbsphinx',
    'nbsphinx_link',
    'myst_parser',
    'breathe',
    'exhale',
]

myst_enable_extensions = [
    "dollarmath",
    "amsmath",
    "deflist",
    "html_admonition",
    "html_image",
    "colon_fence",
    "smartquotes",
    "replacements",
    "substitution",
    "tasklist",
]

# avoid running the notebook's cells
nbsphinx_execute = 'never'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'python_api/pumapy.rst', '**.ipynb_checkpoints']

from unittest import mock
MOCK_MODULES = ['TexGen.Core', 'dolfin', 'matplotlib', 'pyvista', 
                'vtkmodules', 'vtkmodules.util', 'vtkmodules.util.numpy_support', 'vtkmodules.vtkIOXML', 'vtkmodules.vtkIOLegacy',
                'skimage', 'skimage.transform', 'skimage.filters', 'skimage.morphology', 'skimage.segmentation', 'skimage.io',  
                'scipy', 'scipy.ndimage', 'scipy.sparse', 'scipy.sparse.linalg', 
                'scipy.spatial', 'scipy.spatial.transform', 'scipy.optimize']

for module_name in MOCK_MODULES:
    sys.modules[module_name] = mock.Mock()


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = ['_static']
html_logo = "puma_logo.png"
html_theme_options = {
    'logo_only': True,
    'display_version': False,
}


# -- Breathe - Exhale configuration -------------------------------------------------

breathe_projects = {
    "PuMA": "./doxyoutput/xml"
}
breathe_default_project = "PuMA"

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./cpp_api",
    "rootFileName":          "cpp_library_root.rst",
    "rootFileTitle":         "Documentation tree",
    "doxygenStripFromPath":  "..",
    # Suggested optional arguments
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "exhaleDoxygenStdin":    "INPUT = ../../cpp/src",
}

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'
