#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
chmod +x ./env_installer.sh
./env_installer.sh  # check if puma env installed, if not it installs it
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"
conda activate puma  # this env activation only lasts inside bash script


# this is to fix a bug with OpenGL on MacOS Big Sur
if [ "$(uname)" == "Darwin" ]; then
    if [ -f "$CONDA_PREFIX"/lib/python3.7/site-packages/OpenGL/platform/ctypesloader.py ]; then
        sed -i '' 's/util.find_library( name )/"\/System\/Library\/Frameworks\/{}.framework\/{}".format(name,name)/g' "$CONDA_PREFIX"/lib/python3.7/site-packages/OpenGL/platform/ctypesloader.py
    fi
fi

# install pumapy
cd ..
python setup.py install
python -c "import pumapy"  # this line is to fully install pumapy (cython)
