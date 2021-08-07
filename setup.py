from setuptools import setup, Command, find_packages
from distutils.extension import Extension
import os


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
                            Extension("pumapy.generation.tpms_utils", ["python/pumapy/generation/tpms_utils.pyx"]),
                            Extension("pumapy.physicsmodels.isotropic_conductivity_utils", ["python/pumapy/physicsmodels/isotropic_conductivity_utils.pyx"]),
                            Extension("pumapy.physicsmodels.anisotropic_conductivity_utils", ["python/pumapy/physicsmodels/anisotropic_conductivity_utils.pyx"]),
                            Extension("pumapy.physicsmodels.elasticity_utils", ["python/pumapy/physicsmodels/elasticity_utils.pyx"]),
                            ])
except ImportError:
    extensions = [
                  Extension("pumapy.generation.tpms_utils", ["python/pumapy/generation/tpms_utils.c"]),
                  Extension("pumapy.physicsmodels.isotropic_conductivity_utils", ["python/pumapy/physicsmodels/isotropic_conductivity_utils.c"]),
                  Extension("pumapy.physicsmodels.anisotropic_conductivity_utils", ["python/pumapy/physicsmodels/anisotropic_conductivity_utils.c"]),
                  Extension("pumapy.physicsmodels.elasticity_utils", ["python/pumapy/physicsmodels/elasticity_utils.c"]),
                 ]

# add PuMA C++ library to the extensions
# env_dir = os.environ['CONDA_PREFIX']
# src_path = "./cpp/src"
# include_dirs = [x[0] for x in os.walk(os.path.abspath(src_path))]
# include_dirs.append(np.get_include())
# include_dirs.append(env_dir + "/include")
# include_dirs.append(env_dir + "/include/eigen3/Eigen")
# include_dirs.append(os.path.abspath(src_path))
# if platform == "darwin":
#     extra_compile_args = ["-Xpreprocessor", "-fopenmp", "--std=c++0x", "-Wno-format", "-Wno-literal-conversion",
#                           "-Wno-deprecated-register", "-Wno-return-type"]
# else:  # linux
#     extra_compile_args = ["-fopenmp", "--std=c++0x"]
# extensions.append(Extension('pumapy.utilities.libPuMA', sources=['/python/pumapy/utilities/puma_v3_wrapper.cpp'],
#                             libraries=["omp", "fftw3", "fftw3_threads"], include_dirs=include_dirs,
#                             extra_compile_args=extra_compile_args))

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# pumapy package setup
setup(
    name="pumapy",
    version="3.0",
    author="PuMA team",
    maintainer_email="federico.semeraro@nasa.gov, joseph.ferguson@stanford.edu",
    description="A package to compute material properties from micro-CT data.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://gitlab.com/jcfergus/puma-dev",
    project_urls={
        "Bug Tracker": "https://gitlab.com/jcfergus/puma-dev/issues",
    },
    platforms=["Linux", "Mac"],
    package_dir={"": "python"},
    packages=find_packages(where="python"),
    ext_modules=extensions,
    cmdclass={
        'clean': CleanCommand,
    },
    setup_requires=[
                    "setuptools",
                    "wheel",
                    "numpy",
    ],
    install_requires=[
        "numpy",
        "scikit-image",
        "scipy",
        "matplotlib",
        "pyevtk",
        "numpy-stl",
        "pyvista",
        "vtk",
    ],
)
