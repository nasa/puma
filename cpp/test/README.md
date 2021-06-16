# PuMA Test

Testing Framework, developed for the PuMA C++ library.

## Getting Started
Compile and install the project as specified in the general README.md. If the default installation option is followed, the unit-testing framework is installed in the install/bin folder as the pumaX_testing executable

## Running Tests
To run all the testsuites, the executable can be run with no arguments, or -a.

To run a subset of the testsuites, suite indices can be passed as arguments.   
   - Example: "pumaX_testing 0 1 2"  
   - Example: "pumaX_testing 0-2"  
   - Example: "pumaX_testing 0-2 4"  

To see a list of available testsuites and their indices, run "pumaX_testing -h"

## List of Tests
- 0: Example_Test
- 1: Matrix_Test
- 2: Vector3D_Test
- 3: Vector4D_Test
- 4: VolumeFraction_test
- 5: Triangle_Test
- 6: IsoSurface_Test
- 7: Import3DTiff_Test
- 8: Export_3DTiff_Test
- 9: Export_STL_Test
- 10: Export_textfile_Test
- 11: SurfaceArea_Test
- 12: MedianFilter3D_Test
- 13: MeanFilter3D_Test
- 14: BilateralFilter_Test
- 15: Generate_RandomFibers_Test
- 16: Generate_RandomSpheres_Test
- 17: Generate_TPMS_Test
- 18: PoreSpace_Test
- 19: GenerateWeave_Test
- 20: MeanInterceptLength_Test
- 21: PString_Test
- 22: FVThermalConductivity_Test
- 23: FVanisotropicThermalConductivity_Test
- 24: FVElectricalConductivity_Test
- 25: EJThermalConductivity_Test
- 26: EJElectricalConductivity_Test
- 27: ImportBin_Test
- 28: ExportBin_Test
- 29: Export_vtk_Test
- 30: ParticlesCuberilleTortuosity_Test
- 31: Orientation_Test

## Adding a Custom Test Suite
To add a custom test suite:   
1. Create test suite class in a .cpp file in the testsuites/ directory   
    -use example_test.cpp   
2. Each test is defined by a static function, and a function pointer in the testsuite constructor   
3. Add the new testsuite class to the mastertest.cpp class
    3a. Include at top of file (see example for example_test)  
    3b. Create instance of testsuite class in the mastertest constructor (see example for example_test)  
4. Recompile by running "cmake .. && make -j && make install" from the build folder


----------------- Disclaimer: -----------------

THE SOFTWARE AND/OR TECHNICAL DATA ARE PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL CONFORM TO  SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM  INFRINGEMENT, ANY WARRANTY THAT THE SOFTWARE AND/OR TECHNICAL DATA WILL BE ERROR FREE, OR ANY WARRANTY THAT  TECHNICAL DATA, IF PROVIDED, WILL CONFORM TO THE SOFTWARE.  IN NO EVENT SHALL THE UNITED STATES GOVERNMENT, OR ITS  CONTRACTORS OR SUBCONTRACTORS, BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR  CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE AND/OR TECHNICAL DATA, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR  PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE AND/OR TECHNICAL DATA.
 
THE UNITED STATES GOVERNMENT DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD PARTY COMPUTER SOFTWARE,  DATA, OR DOCUMENTATION, IF  SAID THIRD PARTY COMPUTER SOFTWARE, DATA, OR DOCUMENTATION IS PRESENT IN THE NASA SOFTWARE  AND/OR TECHNICAL DATA, AND DISTRIBUTES IT "AS IS."

RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS, AND SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT AND ITS CONTRACTORS AND  SUBCONTRACTORS FOR ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES THAT MAY ARISE FROM RECIPIENT'S USE OF THE SOFTWARE AND/OR TECHNICAL DATA, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, THE USE THEREOF.

IF RECIPIENT FURTHER RELEASES OR DISTRIBUTES THE NASA SOFTWARE AND/OR TECHNICAL DATA, RECIPIENT AGREES TO OBTAIN THIS IDENTICAL WAIVER OF CLAIMS, INDEMNIFICATION AND HOLD HARMLESS AGREEMENT WITH ANY ENTITIES THAT ARE PROVIDED WITH THE SOFTWARE  AND/OR TECHNICAL DATA.
