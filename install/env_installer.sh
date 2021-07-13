#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"


# checking if conda installed
if [ "$(command -v conda)" != "conda" ]; then
    echo "conda not installed. Install it by running 'chmod +x ./install/conda_installer.sh && ./install/conda_installer.sh'"
    exit 1
fi
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
fi
