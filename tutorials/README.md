# How to run the PuMA tutorial
This folder contains notebook tutorial that show the basic workings of PuMA and pumapy. 

## Python Tutorials (pumapy)
There are three ways to run the pumapy tutorials: 
1. Running the jupyter notebooks on google colab
1. Running the python tutorial scripts (.py files) locally
1. Running the jupyter notebooks locally


### Run online in Colab
You can click on the following links to run the tutorial directly on Google Colab. Each link contains a tutorial for one of the code features. 
1. [Workspace Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/workspace_tutorial.ipynb)
1. [Import Export Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/import_export_tutorial.ipynb)
1. [Material Generation Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/generation_tutorial.ipynb)
1. [Visualization Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/visualization_tutorial.ipynb)
1. [Filters Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/filters_tutorial.ipynb)
1. [Pore Analysis Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/pore_identification_tutorial.ipynb)
1. [Surface Area Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/surface_area_tutorial.ipynb)
1. [Orientation Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/orientation_tutorial.ipynb)
1. [Elasticity Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/elasticity_tutorial.ipynb)
1. [Permeability Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/permeability_tutorial.ipynb)
1. [Thermal Conductivity Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/thermal_conductivity_tutorial.ipynb)
1. [Tortuosity Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/tortuosity_tutorial.ipynb)
1. [Multiphase Material Tutorial](https://colab.research.google.com/github/nasa/puma/blob/main/tutorials/python/jupyter_notebooks/multiphase_tutorial.ipynb)


### Run Jupyter Notebooks locally
In order to run the notebooks locally, you must have downloaded the PuMA repository to your computer. 
If you did not install from source, you will have to install the following extra dependencies into the conda environment as: 

    conda activate puma
    conda install -c conda-forge jupyterlab ipympl ipyvtklink

On the other hand, if you installed puma by running "./installer.sh", then these dependencies are already part of the puma environment. 

To follow the tutorial, open a terminal, navigate into the puma/tutorials/python/jupyter_notebooks folder and run:

    conda activate puma
    jupyter lab
    
If you installed the software using conda, then you will have to download the jupyter notebook files (contained in the puma/tutorials/python/jupyter_notebooks folder) from the puma gitlab or puma github. 


### Run Python Scripts locally
The pumapy tutorials can also be run directly as python scripts. The advantage of this format is that the tutorials can serve as templates for creating python scripts for your own simulation needs. 

To run the python tutorials, navigate in a terminal to the puma/tutorials/python/python_scripts folder. From here, you can run the tutorials direclty in python (remember to first activate the puma conda environment). 

As an example, execute the following to run the visualization tutorial: 

    conda activate puma
    cd visualization
    python visualization_tutorial.py



## C++ PuMA tutorials
The C++ code is typically interfaced using the pumaGUI. To launch the PuMA GUI after installation, execute the following in a terminal: 

    conda activate puma
    pumaGUI
    
The C++ code can also be used by importing the C++ library into a project and directly calling the C++ API. Examples of this can be seen in the puma/tutorials/cpp folder. Each subfolder contains examples and tutorials of the functionality of the C++ puma software. Each case also includes a CMakeLists and bash script that provides an example of how to import and compile a puma project that references the PuMA API.

Each of the puma tutorials is run the same way: Open a terminal, activate the puma conda environment, navigate to the example folder, run the makeAll.sh script, and run the resulting executable (located in the build folder). As an example, the following will run the import_export tutorial: 

    conda activate puma
    cd puma/tutorials/cpp/import_export
    ./makeAll.sh
    cd build
    ./import_export
    



----------------- Disclaimer: -----------------

THE SOFTWARE AND/OR TECHNICAL DATA ARE PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL CONFORM TO  SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM  INFRINGEMENT, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL BE ERROR FREE, OR ANY WARRANTY THAT  TECHNICAL DATA, IF PROVIDED, WILL CONFORM TO THE SOFTWARE.  IN NO EVENT SHALL THE UNITED STATES GOVERNMENT, OR ITS  CONTRACTORS OR SUBCONTRACTORS, BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR  CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE AND/OR TECHNICAL DATA, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR  PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE AND/OR TECHNICAL DATA.
 
THE UNITED STATES GOVERNMENT DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD PARTY COMPUTER SOFTWARE,  DATA, OR DOCUMENTATION, IF  SAID THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION IS PRESENT IN THE NASA SOFTWARE  AND/OR TECHNICAL DATA, AND DISTRIBUTES IT "AS IS."

RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS, AND SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS FOR ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES THAT MAY ARISE FROM RECIPIENT'S USE OF THE SOFTWARE AND/OR TECHNICAL DATA, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, THE USE THEREOF.

IF RECIPIENT FURTHER RELEASES OR DISTRIBUTES THE NASA SOFTWARE AND/OR TECHNICAL DATA, RECIPIENT AGREES TO OBTAIN THIS IDENTICAL WAIVER OF CLAIMS, INDEMNIFICATION AND HOLD HARMLESS AGREEMENT WITH ANY ENTITIES THAT ARE PROVIDED WITH THE SOFTWARE  AND/OR TECHNICAL DATA.
