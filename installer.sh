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

###### PUMAPY INSTALLATION ######
echo -e "\nStarting pumapy installation.\n"
chmod +x ./pumapy_installer.sh
./pumapy_installer.sh

####### PUMA C++ INSTALLATION ######
echo -e "\nStarting PuMA installation.\n"
chmod +x ./puma_installer.sh
./puma_installer.sh

###### TEXGEN INSTALLATION ######
echo -e "\nStarting texgen installation.\n"
chmod +x ./texgen_installer.sh
./texgen_installer.sh

###### PUMA GUI INSTALLATION ######
echo -e "\nStarting GUI installation."
echo -e "\n*** If a message 'The environment is inconsistent' is shown, let the conda solver resolve it ***\n"
chmod +x ./gui_installer.sh
./gui_installer.sh

echo -e "\nThe PuMA C++ library, pumapy package and pumaGUI successfully installed."
echo -e "To use any of them, remember to first run 'conda activate puma'"
echo -e "It is recommended to open a new shell session in order for the changes to have effect."
