from setuptools import setup, Command, find_packages
from distutils.extension import Extension
import os
import re
import ast


class CleanCommand(Command):
    """Custom clean command to tidy up the project root."""
    user_options = []
    def initialize_options(self):
        pass
    def finalize_options(self):
        pass
    def run(self):
        os.system('rm -vrf build dist *.pyc *.tgz python/*.egg-info')


# add cython code to the pumapy extensions
try:
    from Cython.Build import cythonize
    extensions = cythonize([
                            Extension("pumapy.generation.tpms_utils", 
                                      [os.path.join("python", "pumapy", "generation", "tpms_utils.pyx")]),
                            Extension("pumapy.physicsmodels.isotropic_conductivity_utils", [os.path.join("python", "pumapy", "physicsmodels", "isotropic_conductivity_utils.pyx")]),
                            Extension("pumapy.physicsmodels.anisotropic_conductivity_utils", [os.path.join("python", "pumapy", "physicsmodels", "anisotropic_conductivity_utils.pyx")]),
                            Extension("pumapy.physicsmodels.elasticity_utils", [os.path.join("python", "pumapy", "physicsmodels", "elasticity_utils.pyx")]),
                            ])
except ImportError:  # if cython not found, use existing C code
    extensions = [
                  Extension("pumapy.generation.tpms_utils", [os.path.join("python", "pumapy", "generation", "tpms_utils.c")]),
                  Extension("pumapy.physicsmodels.isotropic_conductivity_utils", [os.path.join("python", "pumapy", "physicsmodels", "isotropic_conductivity_utils.c")]),
                  Extension("pumapy.physicsmodels.anisotropic_conductivity_utils", [os.path.join("python", "pumapy", "physicsmodels", "anisotropic_conductivity_utils.c")]),
                  Extension("pumapy.physicsmodels.elasticity_utils", [os.path.join("python", "pumapy", "physicsmodels", "elasticity_utils.c")]),
                 ]

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# automatically update version according to __init__.py
_version_re = re.compile(r'__version__\s+=\s+(.*)')
with open('python/pumapy/__init__.py', 'rb') as f:
    version = str(ast.literal_eval(_version_re.search(
        f.read().decode('utf-8')).group(1)))

setup(
    name="pumapy",
    version=version,
    author="PuMA team",
    maintainer_email="federico.semeraro@nasa.gov, joseph.ferguson@stanford.edu",
    description="A package to compute material properties from micro-CT data.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nasa/puma",
    project_urls={
        "Bug Tracker": "https://github.com/nasa/puma/issues",
    },
    platforms=["Linux", "Mac", "Windows"],
    package_dir={"": "python"},
    packages=find_packages(where="python"),
    ext_modules=extensions,
    cmdclass={'clean': CleanCommand},
    install_requires=[  # TexGen also required, but it can be installed as add-on
        "numpy",
        "scikit-image",
        "scipy",
        "matplotlib",
        "pyevtk",
        "pyvista",
        "vtk",  # already in pyvista, but required in input.py
    ],
    package_data={'': [os.path.join('data', '*')]},  # copy over all the example data
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3',
        'Topic :: Scientific/Engineering',
        'Topic :: Scientific/Engineering :: Physics',
    ],
)
