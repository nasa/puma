#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

./cleanAll.sh

conda deactivate
echo -ne '\n' | conda remove --name puma --all
