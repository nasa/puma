#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"
conda activate puma

# clean inside install folder
rm -rf cmake-build-release
rm -rf cmake-build-debug
rm $CONDA_PREFIX/bin/puma*
rm $CONDA_PREFIX/lib/libPuMA*
rm $CONDA_PREFIX/lib/libTexGenCore*
rm -rf TexGen/bin
rm miniconda.sh
cd ..

# clean pumapy
python setup.py clean
python -m pip uninstall -y pumapy

# clean root
find ../ -name '*~' -ls -delete

# clean initproject
rm -rf cpp/initproject/build

# clean GUI
cd gui/build
rm -f *.o
rm -f pumaGUI
rm -f Makefile
