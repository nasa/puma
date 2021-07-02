#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
eval "$(conda shell.bash hook)"

# clean inside install folder
rm -rf cmake-build-release
rm -rf cmake-build-debug
rm -rf bin/pumaX*
rm -rf lib
rm -rf include
rm -rf TexGen
rm -rf miniconda
cd ..

# clean pumapy
conda activate pumapy
python setup.py clean

# clean root
find ../ -name '*~' -ls -delete

# clean initproject
rm -rf cpp/initproject/build

# clean GUI
cd gui/build
rm -f *.o
rm -f PuMA_V3_GUI
rm -f Makefile
