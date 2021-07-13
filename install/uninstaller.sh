#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
export PATH="$CONDA_PREFIX_1"/bin:$PATH  # if env is active, then sometimes conda not found
eval "$(conda shell.bash hook)"


# only removes compiled files
./cleanAll.sh

# remove puma conda environment
conda deactivate
echo -ne '\n' | conda remove --name puma --all
