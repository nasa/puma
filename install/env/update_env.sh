#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

# assumes you have installed the udpated env with: conda env create -f puma_env.yml
conda activate puma  

if [ "$(uname)" == "Darwin" ]; then
    conda list --explicit > "puma-env-mac.lock"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    conda list --explicit > "puma-env-linux.lock"
else
    conda list --explicit > "puma-env-win.lock"
fi
