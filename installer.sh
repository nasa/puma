#!/bin/bash
cd "${0%/*}" || exit 1    # run from this directory
set -e  # exit when any command fails

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
echo -e "\nStarting GUI installation.\n"
chmod +x ./gui_installer.sh
./gui_installer.sh

echo -e "\nThe PuMA C++ library, pumapy package and pumaGUI successfully installed."
echo -e "To use any of them, remember to first run 'conda activate puma'"
