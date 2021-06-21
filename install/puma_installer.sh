#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# creating puma conda env if it doesn't exist
if ! conda info --envs | grep -w puma; then

    echo "Creating puma conda environment."

    if [ "$(uname)" == "Darwin" ]; then
        conda create --name puma --file env/puma-env-mac.lock 
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        conda create --name puma --file env/puma-env-linux.lock 
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi

    conda activate puma
    chmod +x ./env/env_variables.sh
    ./env/env_variables.sh
fi;

# this env activation only lasts inside bash script
conda activate puma

# check if environment variables have been created already
if [ -z ${PuMA_DIR+x} ]; then
    chmod +x ./env/env_variables.sh
    ./env/env_variables.sh
    conda activate puma  # load env variables
fi

# making sure that all the files are included
"$PuMA_DIR"/cpp/src/createCMakeLists_src.sh
"$PuMA_DIR"/cpp/test/createCMakeLists_test.sh

# build and install PuMA
mkdir -p cmake-build-release
cd cmake-build-release
cmake "$PuMA_DIR"/cpp
make -j
make install
cd "$PuMA_DIR"/install
