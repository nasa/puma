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

# this is to fix a libGL path problem on linux
if [ "$(uname)" != "Darwin" ]; then
	VAR=$(conda list mesa-libgl-cos6-x86_64)
	if [[ $VAR != *"mesa-libgl-cos6-x86_64"* ]]; then
		echo -e "\n*** If a message 'The environment is inconsistent' is shown, let the conda solver resolve it ***\n"
	    conda install -y conda-forge::mesa-libgl-cos6-x86_64==11.0.7
	fi
	FILE="$CONDA_PREFIX"/x86_64-conda-linux-gnu/sysroot/usr/lib64/libGL.so.1
	if [ -f "$FILE" ]; then
	    cp -r "$CONDA_PREFIX"/x86_64-conda-linux-gnu/sysroot/usr/lib64 "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr
	    cp "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib64/libGL.so.1 "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib64/libGL.so
	fi
fi

cd ../gui/build
qmake "BUILD_PREFIX=$CONDA_PREFIX" "INSTALL_PREFIX=$CONDA_PREFIX"
make -j2
make install
