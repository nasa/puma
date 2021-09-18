#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# creating puma conda env if it doesn't exist
if [ ! -d "$(conda info --base)/envs/puma" ]; then
    echo "Creating puma conda environment."
    conda env create
fi

# this env activation only lasts inside bash script
conda activate puma

# build and install PuMA
mkdir -p cmake-build-release
cd cmake-build-release
cmake -D CONDA_PREFIX=$CONDA_PREFIX \
      -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX \
      ../../cpp
make -j
make install
