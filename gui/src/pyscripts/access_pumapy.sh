#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails

eval "$(conda shell.bash hook)"

conda activate puma
cd $PuMA_DIR

if [ $# -eq 0 ]; then
	if [ "$(uname)" == "Darwin" ]; then
        osascript -e 'tell application "Terminal" to do script "conda activate puma; cd $PuMA_DIR; ipython -i gui/src/pyscripts/vis.py"'
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        gnome-terminal --title="pumapy" --command="ipython -i gui/src/pyscripts/vis.py"
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi
else 
	python $*  # this transfers all the arguments to python
fi
