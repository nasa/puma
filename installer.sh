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
    ./install/miniconda.sh -b -p $HOME/miniconda
    eval "$($HOME/miniconda/bin/conda shell.bash hook)"
    conda init
    rm ./install/miniconda.sh
fi

cd install
eval "$(conda shell.bash hook)"

echo "  _______  _______  _______  _______"
echo " ____                   ______       "
echo "/\  _\`\         /'\_/\`\/\  _  \      "
echo "\ \ \L\ \__  __/\      \ \ \L\ \     "
echo " \ \ ,__/\ \/\ \ \ \__\ \ \  __ \    "
echo "  \ \ \/\ \ \_\ \ \ \_/\ \ \ \/\ \   "
echo "   \ \_\ \ \____/\ \_\\ \_\ \_\ \_\  "
echo "    \/_/  \/___/  \/_/ \/_/\/_/\/_/  "
echo " _______  _______  _______  _______  "                          
                    

# creating puma conda env if it doesn't exist
if [ ! -d "$(conda info --base)/envs/puma" ]; then

    echo "Creating puma conda environment."
    
    if [ "$(uname)" == "Darwin" ]; then
        curl -Ls https://micro.mamba.pm/api/micromamba/osx-64/latest | tar -xvj bin/micromamba
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        curl -Ls https://micro.mamba.pm/api/micromamba/linux-64/latest | tar -xvj bin/micromamba
    else
        echo "Unrecongnized Operating System, PuMA cannot be installed."
        exit 1
    fi

    mv bin/micromamba .
    
    ./micromamba create -n puma python numpy scikit-image scipy matplotlib  pyevtk pyvista fftw eigen openmp cmake qt=5.15.4 swig pip cython scikit-umfpack jupyterlab ipyvtklink ipympl -c conda-forge -y
    
fi

####### PUMA C++ INSTALLATION ######
echo -e "\nStarting PuMA installation.\n"
chmod +x ./puma_installer.sh
./puma_installer.sh

###### TEXGEN INSTALLATION ######
echo -e "\nStarting texgen installation.\n"
chmod +x ./texgen_installer.sh
./texgen_installer.sh

###### PUMAPY INSTALLATION ######
echo -e "\nStarting pumapy installation.\n"
chmod +x ./pumapy_installer.sh
./pumapy_installer.sh

###### PUMA GUI INSTALLATION ######
echo -e "\nStarting GUI installation."
chmod +x ./gui_installer.sh
./gui_installer.sh

echo -e "\nThe PuMA C++ library, pumapy package and pumaGUI successfully installed."
echo -e "To use any of them, remember to first run 'conda activate puma'"
echo -e "It is recommended to open a new shell session in order for the changes to have effect."
