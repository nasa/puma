#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# creating puma conda env if it doesn't exist
if ! conda info --envs | grep -w pumapy; then

    echo "Creating pumapy conda environment."

    if [ "$(uname)" == "Darwin" ]; then
        conda create --name pumapy --file env/pumapy-env-mac.lock 
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        conda create --name pumapy --file env/pumapy-env-linux.lock 
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi

    conda activate pumapy
    chmod +x ./env/env_variables.sh
    ./env/env_variables.sh
fi;

# this env activation only lasts inside bash script
conda activate pumapy

# check if environment variables have been created already
if [ -z ${PuMA_DIR+x} ]; then
    chmod +x ./env/env_variables.sh
    ./env/env_variables.sh
    conda activate pumapy  # load env variables
fi

# install texgen
chmod +x texgen_installer.sh
./texgen_installer.sh

# install pumapy
cd ..
python install/setup.py install
python -c "import pumapy"  # this line is to fully install pumapy (cython)
