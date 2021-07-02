#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# this env activation only lasts inside bash script
conda activate puma

if [ ! -d "./TexGen" ]; then
    git clone --single-branch --branch py3 https://github.com/fsemerar/TexGen.git
fi

cd TexGen
mkdir -p bin
cd bin

if [ "$(uname)" = "Darwin" ]; then
    EXT=dylib
elif [ "$(expr substr "$(uname -s)" 1 5)" = "Linux" ]; then
    EXT=so
fi

PY_VERSION="$(python -c 'import sys; print(sys.version_info[1])')"
PY_VERSION_M=$PY_VERSION
if [ $PYNUM -le 7 ]; then
    PY_VERSION_M=$PY_VERSIONm
fi

# compilation options
cmake -D BUILD_PYTHON_INTERFACE=ON \
      -D CMAKE_INSTALL_PREFIX=../install \
      -D PYTHON_INCLUDE_DIR="$CONDA_PREFIX"/include/python3.$PY_VERSION_M \
      -D PYTHON_LIBRARY="$CONDA_PREFIX"/lib/libpython3.$PY_VERSION_M.$EXT \
      -D PYTHON_SITEPACKAGES_DIR="$CONDA_PREFIX"/lib/python3.$PY_VERSION/site-packages \
      -D BUILD_GUI=OFF \
      -D BUILD_RENDERER=OFF \
      -D CMAKE_MACOSX_RPATH=ON \
      -D CMAKE_INSTALL_RPATH="TexGen/install/lib" \
      -D BUILD_SHARED_LIBS=OFF ..

make -j
make install
