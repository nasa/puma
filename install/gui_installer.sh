#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

conda activate puma

cd "$PuMA_DIR"/gui/build
qmake
make -j
