#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails

eval "$(conda shell.bash hook)"

conda install sphinx

make clean && rm -rf source/files
sphinx-apidoc -fMT ../python/pumapy -o source/files --templatedir=source/api_template
