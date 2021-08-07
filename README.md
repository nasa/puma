<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/docs/source/puma_logo.png" width="35%"></img>
</p>

[![Documentation Status](https://readthedocs.org/projects/puma-nasa/badge/?version=latest)](https://puma-nasa.readthedocs.io/en/latest/?badge=latest)

-----

# Porous Microstructure Analysis (PuMA)

The stable releases of PuMA can be found on the [official NASA Github repository](https://github.com/nasa/puma), 
whereas the latest development can be found on the [development Gitlab repository](https://gitlab.com/jcfergus/puma-dev). Access the [PuMA documentation](https://puma-nasa.readthedocs.io) for detailed use of each function or to get started with the tutorials. 

The Porous Microstructure Analysis (PuMA) software has been developed to 
compute effective material properties and perform material response simulations on 
digitized microstructures of porous media. PuMA is able to import digital three-dimensional 
images obtained from X-ray microtomography or to generate artificial microstructures 
that mimic real materials. PuMA also provides a module for interactive 3D visualizations. 
Version 3 includes modules to compute simple morphological properties such as porosity, 
volume fractions, pore diameter, and specific surface area. Additional capabilities include 
the determination of effective thermal and electrical conductivity (including the ability 
to simulate local anisotropy), effective diffusivity and tortuosity from the continuum to 
the rarefied regime, and techniques to determine local material orientation.

Some examples of microstructures that have been run in the past are shown in the pictures below, together with PuMA's software architecture schematic.

<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/docs/source/puma_description.png" width="100%"></img>
</p>

## System requirements
UNIX (Tested on MacOS 10.14.1+, Ubuntu 12.04+, and RHEL) and Anaconda installation

Recommended specs:
- 8 GB of ram for small simulations (500<sup>3</sup> or smaller)
- 16-32 GB of ram for medium simulations (800<sup>3</sup> range)
- 32+ GB of ram for large simulations (above 1000<sup>3</sup>)

## Installation
  
Download the project and unzip it (or alternatively "git clone" it); then open a terminal and navigate into the project root folder. If on a Mac, execute "bash" to make sure the bash shell is active. Then run:

      ./installer.sh

Follow the instructions that might be printed. One the PuMA C++ library, pumapy python package and GUI are installed, the latter can be launched by running:

      conda activate puma; pumaGUI

PuMA relies on a conda environment in order to manage its software dependencies and environment variables. In order to link a C++ project to the PuMA library, refer to the next subsection. On the other hand, after activating the puma environment, open a python session and run "import pumapy" in order to import the pumapy python package.

The [jupyter notebooks](./python/tutorials) show the typical function use for pumapy.

### How to link your C++ project to the PuMA library
In the folder [initproject](./cpp/initproject) there is a simple example project already linked to the PuMA library. 
For beginners, it is recommended to start copying and pasting into the main.cpp the [C++ examples](./cpp/examples/examples.cpp) 
and compiling it by running "./makeAll.sh". Since the PuMA installation uses conda to manage the dependencies, 
the puma environment needs to be first loaded by running:

      conda activate puma

### How to setup PuMA on the NAS cluster:
In order to install PuMA on NAS, some modules need to be loaded and environment variables need to be set. This can be
achieved by adding the following commands to the ~/.profile file before running the installer: 

    module use -a /swbuild/analytix/tools/modulefiles
    module load miniconda3/v4
    module load /nasa/modulefiles/pkgsrc/sles12/gcc/6.2
    export CONDA_PKGS_DIRS=/nobackup/$USER/.conda/pkgs
    export CONDA_ENVS_PATH=/nobackup/$USER/.conda/envs
    conda config --append envs_dirs /nobackup/$USER/.conda/envs
    source activate

## Citing PuMA
If you use PuMA in your research, please use the following BibTeX entries to cite it:

```BibTeX
@article{puma2021,
  title = {Update 3.0 to “PuMA: The Porous Microstructure Analysis software”, (PII:S2352711018300281)},
  author={Joseph C. Ferguson and Federico Semeraro and John M. Thornton and Francesco Panerai and Arnaud Borner and Nagi N. Mansour},
  journal={SoftwareX},
  volume={15},
  pages={100775},
  year={2021},
  publisher={Elsevier}
}
@article{puma2018,
  title={PuMA: The porous microstructure analysis software},
  author={Ferguson, Joseph C and Panerai, Francesco and Borner, Arnaud and Mansour, Nagi N},
  journal={SoftwareX},
  volume={7},
  pages={81--87},
  year={2018},
  publisher={Elsevier}
}
```

See the [publications](./publications.md) file for a full list of papers on PuMA and its numerical methods.  

## Common errors and bug reporting
This is a list of the common errors encountered during the setup and how to solve them:

- If receiving the error message "Permission Denied" when attempting to run ./installer.sh, run "chmod +x installer.sh" and try again
- If an error "xcrun: error: invalid active developer path" is displayed on a Mac, the Xcode CommandLineTools need to be installed
- When importing pumapy, if an "MPI_Init_thread" error is displayed, add "export MPICH_INTERFACE_HOSTNAME=localhost" to ~/.bashrc (Linux) or ~/.bash_profile (Mac) 
- If an error "make: Warning: File ... has modification time ... s in the future" is displayed, then run "sudo apt install ntp" (or equivalent for your distribution)
- If an error "fatal error: GL/gl.h: No such file or directory" is displayed, then run "sudo apt-get install mesa-common-dev" (or equivalent for your distribution)

### Linux-specific requirements
Many linux distributions come with built-in development tools, but others to do. Before beginning the installation, make sure that the following are installed: 
- gcc
- g++
- make
- git

To see if they are installed, run each in a terminal window. If they are not installed, a message will appear saying 
"Command 'INPUT' not found". In Ubuntu, each of these can be installed by the following commands: 
- sudo apt-get install git
- sudo apt-get install build-essential

The build-essential installation will isntall gcc, g++, and make. On Fedora based distributions, they can be installed using yum:
- sudo yum group install git
- sudo yum group install "Development Tools"

It is also recommended that the following be run, in case your system does not have the necessary GL libraries: 
- sudo apt-get install mesa-common-dev (for Ubuntu)
- sudo yum install mesa-libGL-devel (For Fedora/CentOS)

If any bugs are found, or if the software crashes for any reason, please contact either of the authors mentioned below.

## Contributing to PuMA
Since PuMA is a free open-source code, any user is welcome to contribute or collaborate however they would like. 
If you are interested in significant development for your research goals, please contact either of the authors mentioned below.

## Authors
Creator:  Joseph Ferguson
[joseph.ferguson@stanford.edu](mailto:joseph.ferguson@stanford.edu)

Maintainer: Federico Semeraro
[federico.semeraro@nasa.gov](mailto:federico.semeraro@nasa.gov)

Contributors:
Francesco Panerai,
John M. Thornton,
Arnaud Borner, 
Jeremie B. Meurisse,
Nagi N. Mansour

## Legal / License
Copyright @ 2017, 2020, 2021 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. All Rights Reserved.
This software may be used, reproduced, and provided to others only as permitted under the terms of the agreement under which it was acquired from the U.S. Government. Neither title to, nor ownership of, the software is hereby transferred. This notice shall remain on all copies of the software.
This file is available under the terms of the NASA Open Source Agreement (NOSA), and further subject to the additional disclaimer below:
Disclaimer:
THE SOFTWARE AND/OR TECHNICAL DATA ARE PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL BE ERROR FREE, OR ANY WARRANTY THAT TECHNICAL DATA, IF PROVIDED, WILL CONFORM TO THE SOFTWARE. IN NO EVENT SHALL THE UNITED STATES GOVERNMENT, OR ITS CONTRACTORS OR SUBCONTRACTORS, BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE AND/OR TECHNICAL DATA, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE AND/OR TECHNICAL DATA.
THE UNITED STATES GOVERNMENT DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION, IF SAID THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION IS PRESENT IN THE NASA SOFTWARE AND/OR TECHNICAL DATA, AND DISTRIBUTES IT "AS IS."
RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND SUBCONTRACTORS, AND SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND SUBCONTRACTORS FOR ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES THAT MAY ARISE FROM RECIPIENT'S USE OF THE SOFTWARE AND/OR TECHNICAL DATA, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, THE USE THEREOF.
IF RECIPIENT FURTHER RELEASES OR DISTRIBUTES THE NASA SOFTWARE AND/OR TECHNICAL DATA, RECIPIENT AGREES TO OBTAIN THIS IDENTICAL WAIVER OF CLAIMS, INDEMNIFICATION AND HOLD HARMLESS, AGREEMENT WITH ANY ENTITIES THAT ARE PROVIDED WITH THE SOFTWARE AND/OR TECHNICAL DATA.
