#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# this env activation only lasts inside bash script
conda activate puma

cd ../gui/build
qmake "BUILD_PREFIX=$CONDA_PREFIX" "INSTALL_PREFIX=$CONDA_PREFIX"
make -j2
make install
