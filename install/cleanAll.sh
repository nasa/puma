#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
eval "$(conda shell.bash hook)"

# clean inside install folder
rm -rf cmake-build-release
rm -rf cmake-build-debug
rm $CONDA_PREFIX/bin/puma*
rm $CONDA_PREFIX/lib/libPuMA*
rm $CONDA_PREFIX/lib/libTexGen*
rm -rf TexGen
rm -rf miniconda
cd ..

# clean pumapy
conda activate puma
python setup.py clean

# clean root
find ../ -name '*~' -ls -delete

# clean initproject
rm -rf cpp/initproject/build

# clean GUI
cd gui/build
rm -f *.o
rm -f pumaGUI
rm -f Makefile
