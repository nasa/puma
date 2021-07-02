#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails

# checking if conda installed
if ! [ -x "$(command -v conda)" ]; then
    if [ "$(uname)" == "Darwin" ]; then
        curl -k https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -o ./install/miniconda.sh
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ./install/miniconda.sh
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi
    chmod +x ./install/miniconda.sh
    ./install/miniconda.sh -b -p ./install/miniconda
    eval "$(./install/miniconda/bin/conda shell.bash hook)"
    conda init
    rm ./install/miniconda.sh
fi

eval "$(conda shell.bash hook)"
cd install

####### PUMA INSTALLATION ######
echo -e "\nStarting PuMA installation.\n"
chmod +x ./puma_installer.sh
./puma_installer.sh

###### PUMAPY INSTALLATION ######
echo -e "\nStarting pumapy installation.\n"
chmod +x ./pumapy_installer.sh
./pumapy_installer.sh

###### PUMA GUI INSTALLATION ######
echo -e "\nStarting GUI installation.\n"
chmod +x ./gui_installer.sh
./gui_installer.sh

echo -e "\nThe PuMA C++ library, pumapy python package and GUI successfully installed."
echo -e "To run PuMA's main.cpp, run 'conda activate puma; pumaX_main'."
echo -e "To open the GUI, run 'conda activate puma; pumaGUI.sh'."
echo -e "To use pumapy, run 'conda activate puma; python' and then 'import pumapy'.\n"
