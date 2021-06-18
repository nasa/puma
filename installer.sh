#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails

# checking if conda installed
if ! [ -x "$(command -v conda)" ]; then
    if [ "$(uname)" == "Darwin" ]; then
        curl -k https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -o ./install/miniconda.sh
        chmod +x ./install/miniconda.sh
        ./install/miniconda.sh
        source ~/.bash_profile
        sed -n '/# >>> conda initialize >>>/,/# <<< conda initialize <<</p' ~/.bash_profile >> ./install/.condarc
        source ./install/.condarc
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ./install/miniconda.sh
        chmod +x ./install/miniconda.sh
        ./install/miniconda.sh
        source ~/.bashrc
        sed -n '/# >>> conda initialize >>>/,/# <<< conda initialize <<</p' ~/.bashrc >> ./install/.condarc
        source ./install/.condarc
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi
    rm ./install/miniconda.sh
fi

eval "$(conda shell.bash hook)"
cd install

####### PUMA INSTALLATION ######
chmod +x ./puma_installer
./puma_installer

###### PUMAPY INSTALLATION ######
chmod +x ./pumapy_installer
./pumapy_installer

###### PUMA GUI INSTALLATION ######
chmod +x ./bin/pumagui_installer
./pumagui_installer
