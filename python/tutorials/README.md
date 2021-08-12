# pumapy Tutorials
This folder contains tutorials that show the basic workings of pumapy. 

## Run online in Colab
You can click on the following links to run them directly on Google Colab:

- [Workspace manipulation](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_workspace_manipulation.ipynb)
- [Import/Export data](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_import_export.ipynb)
- [Visualization](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_visualization.ipynb)
- [Multi-phase material](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_multi_phase.ipynb)
- [Generate random fibers](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_generate_random_fibers.ipynb)
- [Generate random spheres](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_generate_random_spheres.ipynb)
- [Generate TPMS](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_generate_tpms.ipynb)
- [Generate weaves](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_generate_weaves.ipynb)
- [Generate STL](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_generate_STL.ipynb)
- [Compute surface area](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_surface_area.ipynb)
- [Compute thermal conductivity](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_thermalconductivity.ipynb)
- [Compute tortuosity](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_tortuosity_continuum.ipynb)
- [Compute elasticity](https://colab.research.google.com/github/nasa/puma/blob/main/python/tutorials/tutorial_elasticity.ipynb)

## Run locally
In order to run the notebooks locally, you must have downloaded the PuMA repository to your computer. If you installed puma by running "conda create -n puma fsemerar::puma", you will have to install the following extra dependencies into the conda environment as: 

    conda activate puma
    conda install anaconda::jupyter conda-forge::ipympl conda-forge::ipyvtklink

On the other hand, if you installed puma by running "./installer.sh", then these dependencies are already part of the puma environment. 

To follow the tutorials, from the tutorials folder run:

    jupyter notebook


----------------- Disclaimer: -----------------

THE SOFTWARE AND/OR TECHNICAL DATA ARE PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL CONFORM TO  SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM  INFRINGEMENT, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL BE ERROR FREE, OR ANY WARRANTY THAT  TECHNICAL DATA, IF PROVIDED, WILL CONFORM TO THE SOFTWARE.  IN NO EVENT SHALL THE UNITED STATES GOVERNMENT, OR ITS  CONTRACTORS OR SUBCONTRACTORS, BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR  CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE AND/OR TECHNICAL DATA, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR  PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE AND/OR TECHNICAL DATA.
 
THE UNITED STATES GOVERNMENT DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD PARTY COMPUTER SOFTWARE,  DATA, OR DOCUMENTATION, IF  SAID THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION IS PRESENT IN THE NASA SOFTWARE  AND/OR TECHNICAL DATA, AND DISTRIBUTES IT "AS IS."

RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS, AND SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS FOR ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES THAT MAY ARISE FROM RECIPIENT'S USE OF THE SOFTWARE AND/OR TECHNICAL DATA, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, THE USE THEREOF.

IF RECIPIENT FURTHER RELEASES OR DISTRIBUTES THE NASA SOFTWARE AND/OR TECHNICAL DATA, RECIPIENT AGREES TO OBTAIN THIS IDENTICAL WAIVER OF CLAIMS, INDEMNIFICATION AND HOLD HARMLESS AGREEMENT WITH ANY ENTITIES THAT ARE PROVIDED WITH THE SOFTWARE  AND/OR TECHNICAL DATA.
