#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails
eval "$(conda shell.bash hook)"

echo "Installing conda-lock"
echo -ne '\n' | conda install -c conda-forge conda-lock  # install conda-lock in base

python update_env.py -i puma_env.yml.meta -q -p  # this only creates .yml

if [ "$(uname)" == "Darwin" ]; then
    conda-lock -f puma_env.yml -p osx-64 --filename-template "puma-env-mac.lock"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    conda-lock -f puma_env.yml -p linux-64 --filename-template "puma-env-linux.lock"
else
    echo "Unrecongnized Operating System, PuMA cannot be installed."
    exit 1
fi
