#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    // primitives: Matrix
    cout << endl << "Creating a matrix with user input size and printing a slice of it:"<< endl;

    puma::Matrix<double> mat(10,10,10, 3.5); // sets all the elements to 3.5
    // N.B. note that if a value is not specified, matrix just allocates the memory, does not set the matrix to zeros

    mat.set(0,-1, 4,4, 4,4, 1); // creating a central solid fiber
    // N.B. -1 specifies until the end of domain mat.X()-1

    mat.printRange(0,-1, 0,-1, 4,5);
    
    
    // primitives: Workspace
    cout << endl << "Creating a grayscale workspace:"<< endl;

    puma::Workspace grayWS(10,10,10,1e-6, false); // automatically initializes it to zeros
    // false prevents it from writing the log, unnecessary for these tutorials

    grayWS.matrix.set(0,-1, 4,4, 4,4, 255); // creating a central solid fiber

    grayWS.matrix.printRange(0,-1, 0,-1, 4,5);


    cout << endl << "Segmenting it into a segmented workspace:"<< endl;

    puma::Cutoff cutoff(0,89); // calling puma::Cutoff is equivalent to calling pair<int,int>
    grayWS.setMaterialID(cutoff,0);   // mask the segmented ws with the grayscale one, thresholding by assigning 0 to the range 0-89

    grayWS.setMaterialID(puma::Cutoff(90,255),1); // assigning 1 to the range 90-255, with the direct cutoff pair

    grayWS.matrix.printRange(0,-1, 0,-1, 4,5);
    
    // primitives: Vec3
    cout << endl << "Creating a vector with three components:"<< endl;

    puma::Vec3<double> vector(60,5,1);
    vector.x = 1; // reassign one of the components

    cout << "Computing magnitude of original vector: " << vector.magnitude() << endl; // compute the magnitude
    cout << "Computing magnitude of normalized vector: " << (vector.norm()).magnitude() << endl; // compute the magnitude

    puma::Vec3<double> vector2(50,0,0);
    vector.dot(vector2);   // dot product
    vector.cross(vector2); // cross product
    
    
    // primitives: Vec4
    cout << endl << "Creating a vector with three components and a time value" << endl;

    puma::Vec4<float> vec1(1.2, -3.4, 5.6);
    puma::Vec4<float> vec2(1.2, -3.4, 5.6, 1);
    
    
    // primitives: MatVec3
    cout << endl << "Creating a direction matrix for a single fiber running along x:" << endl;

    puma::MatVec3<double> directions(10,10,10); // initializes it to Vec3(0,0,0)
    directions.set(0,-1, 4,4, 4,4, puma::Vec3<double>(1,0,0));

    directions.printRange(0,-1, 0,-1, 4,5);
    
    return 0;
}
