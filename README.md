<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png" width="35%"></img>
</p>

[![Documentation Status](https://readthedocs.org/projects/puma-nasa/badge/?version=latest)](https://puma-nasa.readthedocs.io/en/latest/?badge=latest)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/version.svg)](https://anaconda.org/conda-forge/puma)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/latest_release_date.svg)](https://anaconda.org/conda-forge/puma)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/platforms.svg)](https://anaconda.org/conda-forge/puma)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/license.svg)](https://anaconda.org/conda-forge/puma)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/downloads.svg)](https://anaconda.org/conda-forge/puma)

-----

# Porous Microstructure Analysis (PuMA)

The stable releases of PuMA can be found on the [official NASA Github repository](https://github.com/nasa/puma),
whereas the latest development can be found on the
[development Gitlab repository](https://gitlab.com/jcfergus/puma-dev). Access the
[PuMA documentation](https://puma-nasa.readthedocs.io) for detailed use of each function or to get started with the tutorial.

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

Some examples of microstructures that have been run in the past are shown in the pictures below,
together with PuMA's software architecture schematic.

<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/doc/source/puma_description.png" width="100%"></img>
</p>

## System requirements
UNIX (Tested on MacOS 10.14.1+, Ubuntu 12.04+, RHEL, and CentOS)

Recommended specs:
- 8 GB of ram for small simulations (500<sup>3</sup> or smaller)
- 16-32 GB of ram for medium simulations (800<sup>3</sup> range)
- 32+ GB of ram for large simulations (above 1000<sup>3</sup>)

## Installation

### Installation from source

Installing PuMA from source is currently the most stable installation method. It has been tested successfully on MacOS systems, and a variety of Linux distributions. In the future, this installation method will be replaced by the conda-forge installation, detailed in the next section.

The installation is roughly equivalent for MacOS and Linux systems and is broken into three sections:
1. Installation of basic dependencies that may be missing from your system
1. Downloading the repository from Github
1. Installing the software via. Conda

Step 1 of the installation varies slightly based on the system, so we have split the installation into three separate sections based on the system on which you are installing PuMA.

#### MacOS Installation

Open a terminal, navigate the the directory you would like PuMA installed, and execute the following:

    xcode-select --install
    git clone https://github.com/nasa/puma.git
    cd puma; chmod +x installer.sh; ./installer.sh


#### Debian (Ubuntu) Installation

Open a terminal, navigate the the directory you would like PuMA installed, and execute the following:

    sudo apt-get install git build-essential mesa-common-dev
    git clone https://github.com/nasa/puma.git
    cd puma; chmod +x installer.sh; ./installer.sh

#### Fedora (CentOS, RHEL) Installation

Open a terminal, navigate the the directory you would like PuMA installed, and execute the following:

    sudo yum group install "Development Tools" 
    sudo yum install git mesa-libGL-devel
    git clone https://github.com/nasa/puma.git
    cd puma; chmod +x installer.sh; ./installer.sh


#### Running PuMA

After installation, close the terminal and open a new terminal. The PuMA GUI can then be launched by running:

    conda activate puma; pumaGUI

The [jupyter notebook](https://github.com/nasa/puma/tree/main/tutorial) shows the typical function usage for both PuMA C++ and pumapy.
This can be run directly in Google Colaboratory by following [this link](https://colab.research.google.com/github/nasa/puma/blob/main/tutorial/puma_tutorial.ipynb).


### Installation from conda-forge

PuMA can also be directly installed from conda-forge, without the need to clone the repository from gitlab or github. The conda-forge installation currently works on MacOS systems and some linux distributions, but is less tested than the source installation.

In order to install PuMA from conda-forge, a conda distribution must be installed on your machine. To test whether conda is installed, run "conda" from a terminal to see if the command is recognized. Conda can be installed by following the instructions here: https://docs.anaconda.com/anaconda/install/index.html.

Once conda is installed, PuMA can be installed by executing the following command in a terminal:

    conda create -yn puma conda-forge::puma

This installs the PuMA C++ library, pumapy python package and GUI in a conda environment called "puma".
PuMA relies on a conda environment in order to manage its software dependencies and environment variables.
It is therefore important to always activate the environment before using any of PuMA's functionalities.

After installation, the PuMA GUI can be launched by running:

    conda activate puma; pumaGUI

If the conda-forge installation does not work, try again with the source installation. If issues persist, reference the "common errors" section, then reach out to the authors listed at the bottom of the README.

### Uninstalling PuMA

To uninstall PuMA and all the installed dependencies, execute the following

    conda remove --name puma --all



### How to setup PuMA on the NAS cluster:
In order to install PuMA on the NASA supercomputing cluster, some modules need to be loaded and environment
variables need to be set. This can be achieved by adding the following commands to the ~/.profile file before the installation:

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
  title={Update 3.0 to “PuMA: The Porous Microstructure Analysis software”,(PII: S2352711018300281)},
  author={Ferguson, Joseph C and Semeraro, Federico and Thornton, John M and Panerai, Francesco and Borner, Arnaud and Mansour, Nagi N},
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

See the [publications](https://github.com/nasa/puma/blob/main/publications.md) file for a full list of papers on PuMA and its numerical methods.  

## Common errors and bug reporting
This is a list of the common errors encountered during the setup and how to solve them:

- If PuMA was partially installed but was interrupted, this can cause errors when trying to install the software. To fix this, first follow the instructions to uninstall puma, and then repeat the installation procedure
- When importing pumapy, if an "MPI_Init_thread" error is displayed, add "export MPICH_INTERFACE_HOSTNAME=localhost"
  to ~/.bashrc (Linux) or ~/.bash_profile (Mac)
- If an error "make: Warning: File ... has modification time ... s in the future" is displayed, then run "sudo apt install ntp" (or equivalent for your distribution)


If any bugs are found, or if the software crashes for any reason, please open an issue at [this link](https://github.com/nasa/puma/issues)
and/or contact either of the authors mentioned below.

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
