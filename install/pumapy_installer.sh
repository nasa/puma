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

# this is to fix a bug with OpenGL on MacOS Big Sur
if [ "$(uname)" == "Darwin" ]; then
    if [ -f "$CONDA_PREFIX"/lib/python3.7/site-packages/OpenGL/platform/ctypesloader.py ]; then
        sed -i '' 's/util.find_library( name )/"\/System\/Library\/Frameworks\/{}.framework\/{}".format(name,name)/g' "$CONDA_PREFIX"/lib/python3.7/site-packages/OpenGL/platform/ctypesloader.py
    fi
fi

# install texgen
chmod +x texgen_installer.sh
./texgen_installer.sh

# install pumapy
cd ..
python -m pip install .
python -c "import pumapy"  # this line is to fully install pumapy (cython)
