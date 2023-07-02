<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png" width="35%"></img>
</p>

[![Documentation Status](https://readthedocs.org/projects/puma-nasa/badge/?version=latest)](https://puma-nasa.readthedocs.io/en/latest/?badge=latest)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/puma/badges/version.svg)](https://anaconda.org/conda-forge/puma)
![pumapy Tests](https://github.com/nasa/puma/actions/workflows/test-pumapy.yml/badge.svg)
[![Gitter](https://badges.gitter.im/puma-nasa/community.svg)](https://gitter.im/puma-nasa/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gl/jcfergus%2Fpuma-dev/main)

-----

# Porous Microstructure Analysis (PuMA)

The stable releases of PuMA can be found on the [official NASA Github repository](https://github.com/nasa/puma),
whereas the latest development can be found on the
[development Gitlab repository](https://gitlab.com/jcfergus/puma-dev). 

A few useful resources: 
- Install [TomoSAM](https://github.com/fsemerar/SlicerTomoSAM) to perform tomography segmentation within 3D Slicer
- [Documentation](https://puma-nasa.readthedocs.io) detailing the use of each function 
- [Community chat](https://gitter.im/puma-nasa/community?utm_source=share-link&utm_medium=link&utm_campaign=share-link)
  to ask questions or to get in touch
- [YouTube channel](https://www.youtube.com/channel/UCBp_QC6ctwzdyxfZn7uHj0w/playlists) with the video tutorials
- [Tutorials](https://github.com/nasa/puma/tree/main/tutorials) with examples for both python and C++

## Software description

PuMA, winner of the [2022 NASA Software of the Year prize](https://www.nasa.gov/offices/otps/Winning_NASA_Technologies_Land_Spacecraft_Share_Agency_Innovations), has been developed to compute effective material properties and perform material response simulations on
digitized microstructures of porous media. PuMA is able to import digital three-dimensional
images obtained from X-ray microtomography or to generate artificial microstructures
that mimic real materials. PuMA also provides a module for interactive 3D visualizations.
Version 3 includes modules to compute simple morphological properties such as porosity,
volume fractions, pore diameter, and specific surface area. Additional capabilities include
the determination of effective thermal and electrical conductivity (both radiative and solid conduction - 
including the ability to simulate local anisotropy for the latter); effective diffusivity and 
tortuosity from the continuum to the rarefied regime; techniques to determine the local material orientation, 
as well as the mechanical properties (elasticity coefficient), and the permeability of a material.

Some examples of microstructures that have been run in the past are shown in the pictures below,
together with PuMA's software architecture schematic. See the [publications](https://github.com/nasa/puma/blob/main/publications.md) 
for more information about the software and its capabilities.

<p align="center">
  <img src="https://github.com/nasa/puma/raw/main/doc/source/puma_description.png" width="100%"></img>
</p>

## System requirements
UNIX (tested on MacOS 10.14.1+, Ubuntu 12.04+, RHEL, and CentOS). 
On Windows, only the python distribution (pumapy) is available.

Indicative recommended specs (varies depending on the material property):
- 8 GB of ram for small simulations (500<sup>3</sup> or smaller)
- 16-32 GB of ram for medium simulations (800<sup>3</sup> range)
- 32+ GB of ram for large simulations (above 1000<sup>3</sup>)

## Installation

To install PuMA, a conda distribution must be installed on your machine. 
To test whether conda is installed, run "conda" from a terminal to see if the command is recognized. 
If not, conda can be installed by following the instructions 
[here](https://docs.anaconda.com/anaconda/install/index.html). It is highly recommended to 
[install libmamba](https://www.anaconda.com/blog/a-faster-conda-for-a-growing-community) to speed up the conda solver.

### Binaries (UNIX and Windows)

This is the recommended installation for PuMA users or for developers of the pumapy Python package.
Once the conda command is working, all the PuMA components can be installed by executing 
the following commands in a terminal (note that the "solving environment" step can sometimes take up to 15 mins):

    conda create -y --name puma -c conda-forge puma

On **UNIX** (i.e. Mac or Linux), the conda command installs the PuMA C++ library, pumapy python package and GUI. 
On **Windows**, only the pumapy python package is available.

Note: TexGen (C++ library to create artificial weaves) is only installed on UNIX, because it needs more 
advanced compilation. A friendly warning will appear when TexGen is not installed and pumapy is imported. 

For **developers** of the python modules in pumapy, it is recommended to create the puma environment with the above command
and then to run the following: 

    conda activate puma
    git clone https://github.com/nasa/puma.git
    cd puma
    pip install -e .

Note that, in order to track the latest development, it is recommended to git clone the puma-dev Gitlab repo instead.
These commands will link the local python files to the pumapy installation in the puma conda environment. 
Changes to the local python files will therefore take immediate effect.

### Build from source (UNIX-only)

This is the recommended installation for developers that need to make modifications to the C++ part of PuMA.
The installation is broken into two sections:

1. Installation of basic dependencies that may be missing from your system
2. Download the repository, build the source code, and install the binaries

Step 1 of the installation varies slightly based on the system. 
Open a terminal, navigate to the directory you would like PuMA installed, and execute **one** of the following lines:

    xcode-select --install                                                                 # run this on MacOS
    sudo apt-get install git build-essential mesa-common-dev                               # Debian (Ubuntu)
    sudo yum group install "Development Tools"; sudo yum install git gcc mesa-libGL-devel  # Fedora (CentOS, RHEL)

Note: If XCode command line tools are already installed, the command will result in an error, which is not a problem. 

Now that the necessary dependencies have been installed, you can go ahead with Step 2 of the installation:

    git clone https://github.com/nasa/puma.git
    cd puma; chmod +x installer.sh; ./installer.sh

After installation, close the terminal and open a new one.

### Uninstalling PuMA

To uninstall PuMA and all the installed dependencies, execute the following command to delete the puma environment:

    conda remove -y --name puma --all

In order to update PuMA, it is recommended to run the above command and recreate the environment from scratch. 

## Running PuMA

PuMA relies on a conda environment in order to manage its software dependencies and environment variables.
It is therefore important to always activate the environment before using any of PuMA's functionalities. 
Once the installation is complete, the PuMA GUI can be launched by running:

    conda activate puma
    pumaGUI

You can follow the [jupyter notebook tutorials](https://github.com/nasa/puma/tree/main/tutorials), 
which show the typical function usage for both PuMA C++ and pumapy. These can also be run directly in a browser through Binder.

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

Note that, as systems are constantly updated, these may need to be updated.

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

See the [publications](https://github.com/nasa/puma/blob/main/publications.md) file for a full list of papers on PuMA 
and its numerical methods.  

## Common errors and bug reporting
This is a list of the common errors encountered during the setup and how to solve them:

- If PuMA was partially installed but was interrupted, this can cause errors when trying to install the software. 
  To fix this, first follow the instructions to uninstall puma, and then repeat the installation procedure
- If an error "make: Warning: File ... has modification time ... s in the future" is displayed, 
  then run "sudo apt install ntp" (or equivalent for your distribution)
- If the creation of the conda environment hangs for a long time (>30') at the "Solving environment" step, then 
  it is recommended to try to install puma using [micromamba](https://mamba.readthedocs.io/en/latest/user_guide/micromamba.html), 
  a faster (but less mature) executable that is able to create conda-environments. 

If any bugs are found, or if the software crashes for any reason, please open an issue at [this link](https://github.com/nasa/puma/issues)
and/or contact either of the authors mentioned below.

## Contributing to PuMA
Since PuMA is a free open-source code, any user is welcome to contribute or collaborate however they would like.
If you are interested in significant development for your research goals, please contact either of the authors mentioned 
below, either by email or on the community chat.

## Authors
Creator:  Joseph Ferguson
[joseph.ferguson@stanford.edu](mailto:joseph.ferguson@stanford.edu)

Maintainer: Federico Semeraro
[federico.semeraro@nasa.gov](mailto:federico.semeraro@nasa.gov)

Contributors:
John M. Thornton,
Francesco Panerai,
Arnaud Borner,
Jeremie B. Meurisse,
Sergio Fraile Izquierdo,
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
