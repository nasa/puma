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

# this is to fix a bug with OpenGL on MacOS Big Sur
PY_VERSION="$(python -c 'import sys; print(sys.version_info[1])')"
if [ "$(uname)" == "Darwin" ]; then
    if [ -f "$CONDA_PREFIX"/lib/python3.$PY_VERSION/site-packages/OpenGL/platform/ctypesloader.py ]; then
        sed -i '' 's/util.find_library( name )/"\/System\/Library\/Frameworks\/{}.framework\/{}".format(name,name)/g' "$CONDA_PREFIX"/lib/python3.$PY_VERSION/site-packages/OpenGL/platform/ctypesloader.py
    fi
fi


# install pumapy
cd ..
python -m pip install .
python -c "import pumapy"  # this line is to fully install pumapy (cython)
