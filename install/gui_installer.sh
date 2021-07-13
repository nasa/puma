#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
chmod +x ./env_installer.sh
./env_installer.sh  # check if puma env installed, if not it installs it
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"
conda activate puma  # this env activation only lasts inside bash script


# this is to fix a libGL path problem on linux
if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	VAR=$(conda list mesa-libgl-cos6-x86_64)
	if [[ $VAR != *"mesa-libgl-cos6-x86_64"* ]]; then
		conda config --add channels conda-forge
	    conda install -y mesa-libgl-cos6-x86_64==11.0.7
	fi
	FILE="$CONDA_PREFIX"/x86_64-conda-linux-gnu/sysroot/usr/lib64/libGL.so.1
	if [ -f "$FILE" ]; then
	    cp -r "$CONDA_PREFIX"/x86_64-conda-linux-gnu/sysroot/usr/lib64 "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr
	    cp "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib64/libGL.so.1 "$CONDA_PREFIX"/x86_64-conda_cos6-linux-gnu/sysroot/usr/lib64/libGL.so
	fi
fi

cd ../gui/build
qmake "BUILD_PREFIX=$CONDA_PREFIX" "INSTALL_PREFIX=$CONDA_PREFIX"
make -j
make install
