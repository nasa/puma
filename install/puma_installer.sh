#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
chmod +x ./env_installer.sh
./env_installer.sh  # check if puma env installed, if not it installs it
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"
conda activate puma  # this env activation only lasts inside bash script


# build and install PuMA
mkdir -p cmake-build-release
cd cmake-build-release
cmake -D CONDA_PREFIX=$CONDA_PREFIX \
      -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX \
      ../../cpp
make -j
make install
