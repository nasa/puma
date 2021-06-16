#include "all_cpp_files.h"

#include <Python.h>
#include <numpy/arrayobject.h>
#include <string>
#include <utility>


/* -------------------------------------------------------------------------- */
// Helper Functions (definition at the bottom of the file)
/* -------------------------------------------------------------------------- */
template <class WSorpumaMat>
void py3DArrayDouble_to_pumaData(PyArrayObject *arrayin, WSorpumaMat *something, npy_intp *dims, int numThreads);
void pyDict_to_map(PyObject *pyDict, std::map<int, double> *cMap);
void pyDict_to_map(PyObject *pyDict, std::map<int,std::vector<double>> *cMap);
void pumaMatrix_to_pyObject(puma::Matrix<double> *pumaMat, PyObject *pyObj, const npy_intp *dims);


/* ---------------------------------------------------------------------------------------- */
// The PuMA functions are wrapped into the first static void functions as required for use in Python.
// The second accompanying static PyObject functions read the arguments that are called in python as:
// The O refers to a PyObject or PyArrayObject as an argument (used for numpy arrays and dictionaries).
// O! PyDict_Type
// d refers to a double.
// i refers to an integer.
// s refers to a string.
// The PyObjects are used to derive corresponding C-Objects, which are used in calling the PuMA functions.
/* ---------------------------------------------------------------------------------------- */

static void electricalConductivityFV(puma::Workspace *WS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                     std::string sideBC, std::string solverType, char dir, double solverTol,
                                     int solverMaxIt, bool print, int numThreads, puma::Vec3<double> *cond) {

    puma::Vec3<double> k = puma::compute_FVElectricalConductivity(WS, T, matCond, std::move(sideBC), std::move(solverType), dir, solverTol,
                                                                  solverMaxIt, print, numThreads);
    *cond = k;
}

static PyObject *puma_electricalconductivityFV(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    PyObject *matCond_py;
    const char *sideBC_py;
    const char *solverType_py;
    const char *dir_py;
    double solverTol;
    int solverMaxIt;
    int print_py;
    int Tfield_py;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "OdO!sssdiiii", &WS_py, &voxelLength, &PyDict_Type, &matCond_py, &sideBC_py, &solverType_py,
                          &dir_py, &solverTol, &solverMaxIt, &print_py, &Tfield_py, &numThreads)) {
        return nullptr;
    }

    // Move Dictionary Data to Map
    std::map<int,double> matCond_c;
    pyDict_to_map(matCond_py, &matCond_c);

    // Change python data types for use in PuMA
    std::string sideBC_c = sideBC_py;
    std::string solverType_c = solverType_py;
    char dir_c = dir_py[0];
    bool print_c = print_py;
    bool Tfield_c = Tfield_py;

    puma::Vec3<double> k;
    puma::Matrix<double> T;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    electricalConductivityFV(&WS,&T,matCond_c,sideBC_c,solverType_c,dir_c,solverTol,solverMaxIt,print_c,numThreads,&k);

    if (Tfield_c)
    {
        // Move temperature field to python object
        PyObject* T_py = PyList_New(dims[0]*dims[1]*dims[2]);
        pumaMatrix_to_pyObject(&T, T_py, dims);

        // Move conductivity to a python tuple and return
        Py_ssize_t len = 4;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        PyTuple_SetItem(tup,3,T_py);
        return tup;
    }
    else
    {
        // Move conductivity to a python tuple and return
        Py_ssize_t len = 3;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        return tup;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void thermalConductivityFV(puma::Workspace *WS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                  std::string sideBC, std::string solverType, char dir, double solverTol,
                                  int solverMaxIt, bool print, int numThreads, puma::Vec3<double> *cond) {

    puma::Vec3<double> k = puma::compute_FVThermalConductivity(WS, T, matCond, std::move(sideBC), std::move(solverType), dir, solverTol,
                                                               solverMaxIt, print, numThreads);
    *cond = k;
}

static PyObject *puma_thermalconductivityFV(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    PyObject *matCond_py;
    const char *sideBC_py;
    const char *solverType_py;
    const char *dir_py;
    double solverTol;
    int solverMaxIt;
    int print_py;
    int Tfield_py;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "OdO!sssdiiii", &WS_py, &voxelLength, &PyDict_Type, &matCond_py, &sideBC_py, &solverType_py,
                          &dir_py, &solverTol, &solverMaxIt, &print_py, &Tfield_py, &numThreads)) {
        return nullptr;
    }

    // Move Dictionary Data to Map
    std::map<int,double> matCond_c;
    pyDict_to_map(matCond_py, &matCond_c);

    // Change python data types for use in PuMA
    std::string sideBC_c = sideBC_py;
    std::string solverType_c = solverType_py;
    char dir_c = dir_py[0];
    bool print_c = print_py;
    bool Tfield_c = Tfield_py;

    puma::Vec3<double> k;
    puma::Matrix<double> T;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    thermalConductivityFV(&WS,&T,matCond_c,sideBC_c,solverType_c,dir_c,solverTol,solverMaxIt,print_c,numThreads,&k);

    if (Tfield_c)
    {
        // Move temperature field to python object
        PyObject* T_py = PyList_New(dims[0]*dims[1]*dims[2]);
        pumaMatrix_to_pyObject(&T, T_py, dims);

        // Move conductivity to a python tuple and return
        Py_ssize_t len = 4;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        PyTuple_SetItem(tup,3,T_py);
        return tup;
    }
    else
    {
        // Move conductivity to a python tuple and return
        Py_ssize_t len = 3;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        return tup;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void tortuosityFV(puma::Workspace *WS, puma::Matrix<double> *C, int lowvoid, int highvoid,
                         std::string sideBC, std::string solverType, char dir, double solverTol,
                         int solverMaxIt, bool print, int numThreads, puma::Vec3<double> *tort) {

    puma::Vec3<double> tau = puma::compute_FVTortuosity(WS, C, puma::Cutoff(lowvoid, highvoid), std::move(sideBC), std::move(solverType), dir,
                                                        solverTol, solverMaxIt, print, numThreads);

    *tort = tau;
}

static PyObject *puma_tortuosityFV(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    int lowvoid;
    int highvoid;
    const char *sideBC_py;
    const char *solverType_py;
    const char *dir_py;
    double solverTol;
    int solverMaxIt;
    int print_py;
    int Cfield_py;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiisssdiiii", &WS_py, &voxelLength, &lowvoid, &highvoid, &sideBC_py, &solverType_py,
                          &dir_py, &solverTol, &solverMaxIt, &print_py, &Cfield_py, &numThreads)) {
        return nullptr;
    }

    // Change python data types for use in PuMA
    std::string sideBC_c = sideBC_py;
    std::string solverType_c = solverType_py;
    char dir_c = dir_py[0];
    bool print_c = print_py;
    bool Cfield_c = Cfield_py;

    puma::Vec3<double> tau;
    puma::Matrix<double> C;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    tortuosityFV(&WS,&C,lowvoid,highvoid,sideBC_c,solverType_c,dir_c,solverTol,solverMaxIt,print_c,numThreads,&tau);

    if (Cfield_c)
    {
        // Move temperature field to python object
        PyObject* C_py = PyList_New(dims[0]*dims[1]*dims[2]);
        pumaMatrix_to_pyObject(&C, C_py, dims);

        // Move conductivity to a python tuple and return
        Py_ssize_t len = 4;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(tau.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(tau.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(tau.z));
        PyTuple_SetItem(tup,3,C_py);
        return tup;
    }
    else
    {
        // Move conductivity to a python tuple and return
        Py_ssize_t len = 3;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(tau.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(tau.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(tau.z));
        return tup;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void tortuosityParticlesCube(puma::Workspace *WS, int lowCutoff, int highCutoff,
                                    int numParticles, double meanFreePath, double meanVelocity, int numThreads,
                                    int randomSeed, double totalLength, puma::Vec3<double> *tort) {

    puma::TortuosityReturn tau = puma::compute_particle_cuberille_Tortuosity(WS, puma::Cutoff(lowCutoff, highCutoff), numParticles, meanFreePath,
                                                                             meanVelocity, randomSeed, totalLength, numThreads);
    *tort = tau.tortuosity;
}

static PyObject *puma_tortuosityParticlesCube(PyObject *self, PyObject *args)
{

    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutoff;
    int highCutoff;
    int numParticles;
    double meanFreePath;
    double meanVelocity;
    int numThreads;
    int randomSeed;
    double totalLength;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiiiddiid", &WS_py, &voxelLength, &lowCutoff, &highCutoff, &numParticles,
                          &meanFreePath, &meanVelocity, &numThreads, &randomSeed, &totalLength)) {
        return nullptr;
    }

    puma::Vec3<double> tau;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    tortuosityParticlesCube(&WS,lowCutoff,highCutoff,numParticles,meanFreePath,meanVelocity,numThreads,randomSeed,totalLength,&tau);

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(tau.x));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(tau.y));
    PyTuple_SetItem(tup,2,PyFloat_FromDouble(tau.z));
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void diffusivityParticlesCube(puma::Workspace *WS, int lowCutoff, int highCutoff,
                                     int numParticles, double meanFreePath, double meanVelocity, int numThreads,
                                     int randomSeed, double totalLength, puma::Vec3<double> *diff) {

    puma::TortuosityReturn alpha = puma::compute_particle_cuberille_Tortuosity(WS, puma::Cutoff(lowCutoff, highCutoff), numParticles, meanFreePath, meanVelocity, randomSeed, totalLength,  numThreads);
    *diff = alpha.diffusivity;
}

static PyObject *puma_diffusivityParticlesCube(PyObject *self, PyObject *args)
{

    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutoff;
    int highCutoff;
    int numParticles;
    double meanFreePath;
    double meanVelocity;
    int numThreads;
    int randomSeed;
    double totalLength;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiiiddiid", &WS_py, &voxelLength, &lowCutoff, &highCutoff, &numParticles,
                          &meanFreePath, &meanVelocity, &numThreads, &randomSeed, &totalLength)) {
        return nullptr;
    }

    puma::Vec3<double> diff;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    diffusivityParticlesCube(&WS,lowCutoff,highCutoff,numParticles,meanFreePath,meanVelocity,numThreads,randomSeed,totalLength,&diff);

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(diff.x));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(diff.y));
    PyTuple_SetItem(tup,2,PyFloat_FromDouble(diff.z));
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void meanInterceptLength(puma::Workspace *WS, int lowCutoff, int highCutoff, int numThreads, puma::Vec3<double> *mil) {
    puma::Vec3<double> MIL = puma::compute_MeanInterceptLength(WS,puma::Cutoff(lowCutoff,highCutoff), numThreads);
    *mil = MIL;
}

static PyObject *puma_meaninterceptlength(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutoff;
    int highCutoff;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiii", &WS_py, &voxelLength, &lowCutoff, &highCutoff, &numThreads)) {
        return nullptr;
    }

    puma::Vec3<double> mil;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    meanInterceptLength(&WS,lowCutoff,highCutoff, numThreads, &mil);

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(mil.x));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(mil.y));
    PyTuple_SetItem(tup,2,PyFloat_FromDouble(mil.z));
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void surfaceAreaMC(puma::Workspace *grayWS, int lowCutoff, int highCutoff, bool interpVerts, std::pair<double,double> *sa) {
    std::pair<double, double> area = puma::compute_SurfaceAreaMarchingCubes(grayWS, puma::Cutoff(lowCutoff, highCutoff), interpVerts);
    *sa = area;
}

static PyObject *puma_surfaceareaTriGrid(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutOff;
    int highCutOff;
    int interpVerts_py;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiiii", &WS_py, &voxelLength, &lowCutOff, &highCutOff, &interpVerts_py, &numThreads)) {
        return nullptr;
    }

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    npy_intp *dims = PyArray_DIMS(WS_py);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Change python data types for use in PuMA
    bool interpVerts_c = interpVerts_py;

    // Run the PuMA Function
    std::pair<double, double> surfArea;
    surfaceAreaMC(&WS, lowCutOff, highCutOff, interpVerts_c, &surfArea);

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 2;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(surfArea.first));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(surfArea.second));
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void surfaceAreaV(puma::Workspace *grayWS, int lowCutoff, int highCutoff, std::pair<double,double> *sa) {
    std::pair<double, double> area = puma::compute_SurfaceAreaVoxels(grayWS, puma::Cutoff(lowCutoff, highCutoff));
    *sa = area;
}

static PyObject *puma_surfaceareaCubeGrid(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutOff;
    int highCutOff;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiii", &WS_py, &voxelLength, &lowCutOff, &highCutOff, &numThreads)) {
        return nullptr;
    }

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    npy_intp *dims = PyArray_DIMS(WS_py);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    std::pair<double, double> surfArea;
    surfaceAreaV(&WS, lowCutOff, highCutOff, &surfArea);

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 2;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(surfArea.first));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(surfArea.second));
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void volumeFraction(puma::Workspace *grayWS, int lowCutoff, int highCutoff, int numThreads, double *vf) {
    double VF = puma::compute_VolumeFraction(grayWS, puma::Cutoff(lowCutoff, highCutoff), numThreads);
    *vf = VF;
}

static PyObject *puma_volumefraction(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double voxelLength;
    int lowCutoff;
    int highCutoff;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Odiii", &WS_py, &voxelLength, &lowCutoff, &highCutoff, &numThreads)) {
        return nullptr;
    }

    double vf;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS(voxelLength);
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    volumeFraction(&WS,lowCutoff,highCutoff, numThreads, &vf);

    // Return as python double
    return PyFloat_FromDouble(vf);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void anisotropicThermalConductivityFV(puma::Workspace *WS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double>>& matCond,
                                             const std::string& method, const std::string& sideBC, const std::string& solverType, char dir, double solverTol,
                                             int solverMaxIt, bool print, int numThreads, puma::Vec3<double> *cond) { // puma::MatVec3<double> *direction

    puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(WS,T,q,matCond,method,sideBC,solverType,dir,solverTol,solverMaxIt,print,numThreads);
    *cond = k;
}

static PyObject *puma_anisotropicThermalConductivityFV(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    double solverTol_py;
    PyObject *matCond_py;
    int numThreads,solverMaxIt_py,print_py, Tfield_py;
    const char *method_py, *sideBC_py, *solverType_py, *dir_py;
//    puma::MatVec3<double> direction_c(1,1,1);

    // Reads the python arguments
//    int argCount = PyTuple_GET_SIZE(args);
//    if (argCount == 13) { // for Homogeneous material
    if (!PyArg_ParseTuple(args, "OO!ssssdiiii", &WS_py, &PyDict_Type, &matCond_py, &method_py, &sideBC_py,
                          &solverType_py, &dir_py, &solverTol_py, &solverMaxIt_py, &print_py, &Tfield_py, &numThreads)){
        return nullptr;
    }
//    } else { // for Heterogeneous material (direction matrix)
//        PyArrayObject *FibDirX_py, *FibDirY_py, *FibDirZ_py;
//        if (!PyArg_ParseTuple(args, "OidO!sssdiisOOOii:PuMA", &WS_py, &segmented_py, &voxelLength, &PyDict_Type, &matCond_py, &sideBC_py, &solverType_py,
//                              &dir_py, &solverTol_py, &solverMaxIt_py, &print_py, &method_py, &FibDirX_py, &FibDirY_py, &FibDirZ_py, &Tfield_py, &numThreads)){
//            return nullptr;
//        }
//        puma::Matrix< double > dirX_c,dirY_c,dirZ_c;
//        npy_intp *dims = PyArray_DIMS(WS_py);
//        py3DArrayDouble_to_pumaData(FibDirX_py, &dirX_c, dims, numThreads);
//        py3DArrayDouble_to_pumaData(FibDirY_py, &dirY_c, dims, numThreads);
//        py3DArrayDouble_to_pumaData(FibDirZ_py, &dirZ_c, dims, numThreads);
//        direction_c.resize(dims[0], dims[1], dims[2]);
//        for(int i=0; i<dims[0]*dims[1]*dims[2]; i++){
//            direction_c(i).x = dirX_c(i);
//            direction_c(i).y = dirY_c(i);
//            direction_c(i).z = dirZ_c(i);
//        }
//    }

    // Move Dictionary of lists to Map of vectors
    std::map<int,std::vector<double>> matCond_c;
    pyDict_to_map(matCond_py, &matCond_c);

    std::string sideBC_c = sideBC_py;
    char dir_c = dir_py[0];
    std::string solverType_c = solverType_py;
    bool print_c = print_py;
    bool Tfield_c = Tfield_py;
    std::string method_c = method_py;

    npy_intp *dims = PyArray_DIMS(WS_py);
    puma::Matrix<double> T(dims[0], dims[1], dims[2]);
    puma::MatVec3<double> q(dims[0], dims[1], dims[2]);

    puma::Vec3<double> k;

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS_c;
    py3DArrayDouble_to_pumaData(WS_py, &WS_c, dims, numThreads);

    // Run the PuMA Function
    anisotropicThermalConductivityFV(&WS_c, &T,  &q, matCond_c, method_c, sideBC_c, solverType_c, dir_c, solverTol_py, solverMaxIt_py, print_c, numThreads, &k); // &direction_c

    if (Tfield_c)
    {
        // Move temperature and flux to python object
        PyObject* T_py = PyList_New(dims[0]*dims[1]*dims[2]);
        pumaMatrix_to_pyObject(&T, T_py, dims);

        PyObject* qX_py = PyList_New(dims[0]*dims[1]*dims[2]);
        PyObject* qY_py = PyList_New(dims[0]*dims[1]*dims[2]);
        PyObject* qZ_py = PyList_New(dims[0]*dims[1]*dims[2]);
        for(long i=0;i<dims[0];i++){
            for(long j=0;j<dims[1];j++){
                for(long k=0;k<dims[2];k++){
                    PyList_SetItem(qX_py, dims[2]*(dims[1]*i+j)+k, PyFloat_FromDouble(q(i,j,k).x));
                    PyList_SetItem(qY_py, dims[2]*(dims[1]*i+j)+k, PyFloat_FromDouble(q(i,j,k).y));
                    PyList_SetItem(qZ_py, dims[2]*(dims[1]*i+j)+k, PyFloat_FromDouble(q(i,j,k).z));
                }
            }
        }

        // Move conductivity to a python tuple and return
        Py_ssize_t len = 7;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        PyTuple_SetItem(tup,3,T_py);
        PyTuple_SetItem(tup,4,qX_py);
        PyTuple_SetItem(tup,5,qY_py);
        PyTuple_SetItem(tup,6,qZ_py);
        return tup;
    }
    else
    {
        // Move conductivity to a python tuple and return
        Py_ssize_t len = 3;
        PyObject *tup = PyTuple_New(len);
        PyTuple_SetItem(tup,0,PyFloat_FromDouble(k.x));
        PyTuple_SetItem(tup,1,PyFloat_FromDouble(k.y));
        PyTuple_SetItem(tup,2,PyFloat_FromDouble(k.z));
        return tup;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void orientationComparison(puma::Workspace *ws, puma::MatVec3<double> *dirs, puma::MatVec3<double> *tangents, puma::Matrix<double> *error, int lowCutoff, int highCutoff, int numThreads, std::pair <double, double> *MeanSD) {

    *MeanSD = puma::compute_orientationComparison(ws, dirs, tangents, error, puma::Cutoff(lowCutoff, highCutoff), numThreads);
}

static PyObject *puma_orientationComparison(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py, *dirs_py, *tangents_py;
    int lowCutoff, highCutoff;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "OOOiii", &WS_py, &dirs_py, &tangents_py, &lowCutoff, &highCutoff, &numThreads)){
        return nullptr;
    }

    npy_intp *dims = PyArray_DIMS(WS_py);

    puma::Matrix<double> error;
    long size = dims[0]*dims[1]*dims[2];

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS_c;
    py3DArrayDouble_to_pumaData(WS_py, &WS_c, dims, numThreads);

    // Computed Mean and Standard Deviation
    std::pair <double, double> MeanSD;

    puma::MatVec3<double> dirs_c(dims[0],dims[1],dims[2]), tangents_c(dims[0],dims[1],dims[2]);

    // Passing numpy array arranged as dirs[x-y-z as 0-1-2][i,j,k] to puma::MatVec3
    double *data_c;
    data_c = (double *) dirs_py->data;

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long l=0; l<size; l++){
        dirs_c.at(l) = puma::Vec3<double>(data_c[l], data_c[size+l], data_c[size*2+l]);
    }

    data_c = (double *) tangents_py->data;

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long l=0; l<size; l++){
        tangents_c.at(l) = puma::Vec3<double>(data_c[l], data_c[size+l], data_c[size*2+l]);
    }

    // Run the PuMA Function
    orientationComparison(&WS_c,&dirs_c,&tangents_c,&error,lowCutoff,highCutoff,numThreads,&MeanSD);

    // Move temperature field to python object
    PyObject* error_py =  PyList_New(dims[0]*dims[1]*dims[2]);
    pumaMatrix_to_pyObject(&error, error_py, dims);

    // Move three direction matrices to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,PyFloat_FromDouble(MeanSD.first));
    PyTuple_SetItem(tup,1,PyFloat_FromDouble(MeanSD.second));
    PyTuple_SetItem(tup,2,error_py);
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void orientationRC(puma::Workspace *WS, int lowCutoff, int highCutoff, int initAccuracy, int degreeAccuracy,
                          puma::MatVec3<double> *direction, int print, int numThreads) {

    puma::compute_orientationRC(WS,puma::Cutoff(lowCutoff,highCutoff),initAccuracy,degreeAccuracy,direction,print,numThreads);
}

static PyObject *puma_orientationRC(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    int lowCutoff, highCutoff;
    int degreeAccuracy, initAccuracy;
    int print, numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Oiiiiii", &WS_py, &lowCutoff, &highCutoff, &initAccuracy, &degreeAccuracy, &print, &numThreads)){
        return nullptr;
    }

    npy_intp *dims = PyArray_DIMS(WS_py);
    puma::MatVec3<double> direction_c;

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS_c;
    py3DArrayDouble_to_pumaData(WS_py, &WS_c, dims, numThreads);

    // Run the PuMA Function
    orientationRC(&WS_c, lowCutoff, highCutoff, initAccuracy, degreeAccuracy, &direction_c, print, numThreads);

    // Move direction vectors to python object (different than other cases since vectors inside puma Matrix)
    long X=dims[0], Y=dims[1], Z=dims[2];
    PyObject* Xdir_py = PyList_New(X*Y*Z);
    PyObject* Ydir_py = PyList_New(X*Y*Z);
    PyObject* Zdir_py = PyList_New(X*Y*Z);
    for(long i=0;i<X;i++){
        for(long j=0;j<Y;j++){
            for(long k=0;k<Z;k++){
                PyList_SetItem(Xdir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).x));
                PyList_SetItem(Ydir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).y));
                PyList_SetItem(Zdir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).z));
            }
        }
    }

    // Move three direction matrices to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,Xdir_py);
    PyTuple_SetItem(tup,1,Ydir_py);
    PyTuple_SetItem(tup,2,Zdir_py);
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void orientationAF(puma::Workspace *WS, puma::MatVec3<double> *q, int lowCutoff, int highCutoff,
                          double solverTol, int solverMaxIt, int print, int numThreads) {

    puma::compute_orientationAF(WS, q, puma::Cutoff(lowCutoff, highCutoff), solverTol, solverMaxIt, print, numThreads);
}


static PyObject *puma_orientationAF(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    int lowCutoff, highCutoff;
    double solverTol;
    int solverMaxIt;
    int print;
    int numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Oiidiii", &WS_py, &lowCutoff, &highCutoff, &solverTol, &solverMaxIt, &print, &numThreads)){
        return nullptr;
    }

    puma::MatVec3<double> q, qY, qZ;
    npy_intp *dims = PyArray_DIMS(WS_py);

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS;
    py3DArrayDouble_to_pumaData(WS_py, &WS, dims, numThreads);

    // Run the PuMA Function
    orientationAF(&WS,&q,lowCutoff,highCutoff,solverTol,solverMaxIt,print,numThreads);

    // Move q vectors to python object (different than other cases since vectors inside puma Matrix)
    long X=dims[0], Y=dims[1], Z=dims[2];
    PyObject* qX_py = PyList_New(X*Y*Z);
    PyObject* qY_py = PyList_New(X*Y*Z);
    PyObject* qZ_py = PyList_New(X*Y*Z);
    for(long i=0;i<X;i++){
        for(long j=0;j<Y;j++){
            for(long k2=0;k2<Z;k2++){
                PyList_SetItem(qX_py, Z*(Y*i+j)+k2, PyFloat_FromDouble(q(i,j,k2).x));
                PyList_SetItem(qY_py, Z*(Y*i+j)+k2, PyFloat_FromDouble(q(i,j,k2).y));
                PyList_SetItem(qZ_py, Z*(Y*i+j)+k2, PyFloat_FromDouble(q(i,j,k2).z));
            }
        }
    }

    // Move conductivity to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,qX_py);
    PyTuple_SetItem(tup,1,qY_py);
    PyTuple_SetItem(tup,2,qZ_py);
    return tup;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void orientationST(puma::Workspace *WS, double dogSigma, double gausRho, int lowCutoff, int highCutoff,
                          puma::MatVec3<double> *direction, bool print, int numThreads) {

    puma::compute_orientationST(WS,dogSigma,gausRho,puma::Cutoff(lowCutoff,highCutoff),direction,print,numThreads);
}

static PyObject *puma_orientationST(PyObject *self, PyObject *args)
{
    PyArrayObject *WS_py;
    int lowCutoff, highCutoff;
    double dogSigma, gausRho;
    int print, numThreads;

    // Reads the python arguments
    if (!PyArg_ParseTuple(args, "Oddiiii", &WS_py, &dogSigma, &gausRho, &lowCutoff, &highCutoff, &print, &numThreads)){
        return nullptr;
    }

    npy_intp *dims = PyArray_DIMS(WS_py);
    puma::MatVec3<double> direction_c;

    // Move Numpy Data to PuMA Workspace
    puma::Workspace WS_c;
    py3DArrayDouble_to_pumaData(WS_py, &WS_c, dims, numThreads);

    // Run the PuMA Function
    orientationST(&WS_c, dogSigma, gausRho, lowCutoff, highCutoff, &direction_c, print, numThreads);

    // Move direction vectors to python object (different than other cases since vectors inside puma Matrix)
    long X=dims[0], Y=dims[1], Z=dims[2];
    PyObject* Xdir_py = PyList_New(X*Y*Z);
    PyObject* Ydir_py = PyList_New(X*Y*Z);
    PyObject* Zdir_py = PyList_New(X*Y*Z);
    for(long i=0;i<X;i++){
        for(long j=0;j<Y;j++){
            for(long k=0;k<Z;k++){
                PyList_SetItem(Xdir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).x));
                PyList_SetItem(Ydir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).y));
                PyList_SetItem(Zdir_py, Z*(Y*i+j)+k, PyFloat_FromDouble(direction_c(i,j,k).z));
            }
        }
    }

    // Move three direction matrices to a python tuple and return
    Py_ssize_t len = 3;
    PyObject *tup = PyTuple_New(len);
    PyTuple_SetItem(tup,0,Xdir_py);
    PyTuple_SetItem(tup,1,Ydir_py);
    PyTuple_SetItem(tup,2,Zdir_py);
    return tup;
}


/* ---------------------------------------------------------------------------------------- */
// Method Table
/* ---------------------------------------------------------------------------------------- */

static struct PyMethodDef methods[] = {
        {"electricalconductivityFV",                      (PyCFunction)puma_electricalconductivityFV,         METH_VARARGS, nullptr},
        {"thermalconductivityFV",                         (PyCFunction)puma_thermalconductivityFV,            METH_VARARGS, nullptr},
        {"tortuosityFV",                                  (PyCFunction)puma_tortuosityFV,                     METH_VARARGS, nullptr},
        {"tortuosityParticlesCube",                       (PyCFunction)puma_tortuosityParticlesCube,          METH_VARARGS, nullptr},
        {"diffusivityParticlesCube",                      (PyCFunction)puma_diffusivityParticlesCube,         METH_VARARGS, nullptr},
        {"meaninterceptlength",                           (PyCFunction)puma_meaninterceptlength,              METH_VARARGS, nullptr},
        {"surfaceareaTriGrid",                            (PyCFunction)puma_surfaceareaTriGrid,               METH_VARARGS, nullptr},
        {"surfaceareaCubeGrid",                           (PyCFunction)puma_surfaceareaCubeGrid,              METH_VARARGS, nullptr},
        {"volumefraction",                                (PyCFunction)puma_volumefraction,                   METH_VARARGS, nullptr},
        {"anisotropicThermalConductivityFV",              (PyCFunction)puma_anisotropicThermalConductivityFV, METH_VARARGS, nullptr},
        {"orientationComparison",                         (PyCFunction)puma_orientationComparison,            METH_VARARGS, nullptr},
        {"orientationRC",                                 (PyCFunction)puma_orientationRC,                    METH_VARARGS, nullptr},
        {"orientationAF",                                 (PyCFunction)puma_orientationAF,                    METH_VARARGS, nullptr},
        {"orientationST",                                 (PyCFunction)puma_orientationST,                    METH_VARARGS, nullptr},
        {nullptr,                            nullptr,                                           0,                  nullptr} /* sentinel */
};


/* ---------------------------------------------------------------------------------------- */
// Python3
// This is the default structure of a python module.
// For now we only add methods which is the PartitionData function.
/* ---------------------------------------------------------------------------------------- */

static struct PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "puma",       // m_name
        nullptr,         // m_doc
        -1,           // m_size
        methods,      // m_methods
        nullptr,         // m_reload
        nullptr,         // m_traverse
        nullptr,         // m_clear
        nullptr          // m_free
};


/* ---------------------------------------------------------------------------------------- */
// This constructs the PyModuleDef that is defined above.
/* ---------------------------------------------------------------------------------------- */

PyMODINIT_FUNC
PyInit_libPuMA(void)
{
    PyObject *m = nullptr;

    /* Module initialization  */
    m = PyModule_Create(&module);
    if (m == nullptr) goto bad;

    return m;

    bad:
    return nullptr;
}


/* -------------------------------------------------------------------------- */
// Helper Functions
/* -------------------------------------------------------------------------- */

// REMEMBER FOR DRAGONFLY PLUGIN
// Dragonfly npy data arranged differently than a PuMA WS or Matrix (Z,Y,X)

// Convert Numpy array into either a puma WS or Matrix
template <class WSorpumaMat>
void py3DArrayDouble_to_pumaData(PyArrayObject *arrayin, WSorpumaMat *something, npy_intp *dims, int numThreads)  {

    something->resize(dims[0],dims[1],dims[2]); // Set Workspace dimensions

    int itemsize = PyArray_ITEMSIZE(arrayin);
    if (itemsize==1){ // unsigned char case
        unsigned char *data_c;
        data_c = (unsigned char *) arrayin->data;

        // Fill PuMA Workspace with array values
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long l=0; l<dims[0]*dims[1]*dims[2]; l++){
            something->at(l) = data_c[l];
        }
    }
    else if (itemsize==2){ // unsigned short case
        unsigned short *data_c;
        data_c = (unsigned short *) arrayin->data;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long l=0; l<dims[0]*dims[1]*dims[2]; l++){
            something->at(l) = data_c[l];
        }
    }
    else if (itemsize==4){ // unsigned int case
        unsigned int *data_c;
        data_c = (unsigned int *) arrayin->data;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long l=0; l<dims[0]*dims[1]*dims[2]; l++){
            something->at(l) = data_c[l];
        }
    }
    else if (itemsize==8){ // double case
        double *data_c;
        data_c = (double *) arrayin->data;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long l=0; l<dims[0]*dims[1]*dims[2]; l++){
            something->at(l) = data_c[l];
        }
    }
}

// Convert a python Dictionary into a map (of doubles)
void pyDict_to_map(PyObject *pyDict, std::map<int, double> *cMap)  {

    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(pyDict,&pos,&key,&value)){
        auto key_c = PyLong_AsLong(key);
        double value_c = PyFloat_AsDouble(value);
        (*cMap)[key_c] = value_c;
    }
}

// Convert a python Dictionary into a map (of vector doubles)
void pyDict_to_map(PyObject *pyDict, std::map<int,std::vector<double>> *cMap)  {

    PyObject *key, *value;
    std::vector<double> value_c;
    Py_ssize_t pos = 0;
    while (PyDict_Next(pyDict,&pos,&key,&value)){
        long key_c = PyLong_AsLong(key);
        Py_ssize_t sizevalueindict = PyList_Size(value);
        value_c.resize(sizevalueindict);
        for(long i=0;i<sizevalueindict;i++) {
            Py_ssize_t i2 = i;
            value_c[i] = PyFloat_AsDouble(PyList_GetItem(value,i2));
        }
        (*cMap)[key_c] = value_c;
    }
}

// Convert puma Matrix into Numpy array
void pumaMatrix_to_pyObject(puma::Matrix<double> *pumaMat, PyObject *pyObj, const npy_intp *dims)  {

    for(long i=0;i<dims[0];i++){
        for(long j=0;j<dims[1];j++){
            for(long k=0;k<dims[2];k++){
                PyList_SetItem(pyObj, dims[2]*(dims[1]*i+j)+k, PyFloat_FromDouble(pumaMat->at(i,j,k)));
            }
        }
    }
}
