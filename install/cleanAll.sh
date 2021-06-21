#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory

rm -rf cmake-build-release
rm -rf cmake-build-debug
rm -rf bin/pumaX*
rm -rf lib
rm -rf include
rm -rf TexGen
rm -rf miniconda
find ../ -name '*~' -ls -delete
python setup.py clean
cd ../gui/build
rm -f *.o
rm -f PuMA_V3_GUI
rm -f Makefile
