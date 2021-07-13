#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails


if [ "$(uname)" == "Darwin" ]; then
    curl -k https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -o miniconda.sh
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh
else
    echo "Unrecongnized Operating System, PuMA cannot be installed."
    exit 1
fi
chmod +x ./miniconda.sh
./miniconda.sh -b -p ./miniconda
eval "$(./miniconda/bin/conda shell.bash hook)"
conda init
rm ./miniconda.sh
