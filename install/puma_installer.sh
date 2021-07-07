#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# creating puma conda env if it doesn't exist
if [ ! -d "$(conda info --base)/envs/puma" ]; then
    echo "Creating puma conda environment."

    if [ "$(uname)" == "Darwin" ]; then
        conda create --name puma --file env/puma-env-mac.lock 
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        conda create --name puma --file env/puma-env-linux.lock 
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi
fi;

# this env activation only lasts inside bash script
conda activate puma

# making sure that all the files are included
chmod +x ../cpp/src/createCMakeLists_src.sh
../cpp/src/createCMakeLists_src.sh
chmod +x ../cpp/test/createCMakeLists_test.sh
../cpp/test/createCMakeLists_test.sh

# build and install PuMA
mkdir -p cmake-build-release
cd cmake-build-release
cmake -D BUILD_PREFIX=$CONDA_PREFIX \
      -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX \
      ../../cpp
make -j
make install
