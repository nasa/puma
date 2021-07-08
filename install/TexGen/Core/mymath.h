/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once

#include <cmath>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <ostream>
#include <istream>
#include <algorithm>

#define PI 3.1415926535897932384626433832795
/// Namespace containing a series of customised math operations not found in the standard c++ library
namespace TexGen
{
	struct XYZ;

	/// Struct for representing a quaternion
	struct WXYZ
	{
		double w, x, y, z;
		/// Set all coordinates to 0 as default constructor
		WXYZ() {w=x=y=z=0.0;}
		/// Set coordinates to those specified in the constructor
		WXYZ(double W, double X, double Y, double Z) {w=W;x=X;y=Y;z=Z;}
		/// Set coordinates to those specified in the constructor
		WXYZ(double Coords[4]) {w=Coords[0];x=Coords[1];y=Coords[2];z=Coords[3];}
		/// Create rotation with axis and angle (radians)
		WXYZ(XYZ Axis, double dAngle);
		/// Create rotation with new coordinate system
		WXYZ(XYZ X, XYZ Y, XYZ Z);
		/// Create rotation with 3 rotation angles (radians)
		WXYZ(double dAzimuth, double dElevation, double dTilt);
	};

	/// Struct for representing points in 3D space
	struct XYZ
	{
		double x, y, z;
		/// Set all coordinates equal to value
		double operator =(double right)
		{
			x = right;
			y = right;
			z = right;
			return right;
		}
		/// Check if all the coordinates are 0
		bool operator !() const
		{
			return (!x && !y && !z);
		}
		operator bool() const
		{
			return (x || y || z);
		}
		double &operator [](int i)
		{
			return *(&x+i);
		}
		const double &operator [](int i) const
		{
			return *(&x+i);
		}
		/// Reverse all coordinates
		XYZ operator -()
		{
			XYZ ReturnVal;
			ReturnVal.x = -x;
			ReturnVal.y = -y;
			ReturnVal.z = -z;
			return ReturnVal;
		}

		/// Set all coordinates to 0 as default constructor
		XYZ() {x=y=z=0.0;}
		/// Set coordinates to those specified in the constructor
		XYZ(double X, double Y, double Z) {x=X;y=Y;z=Z;}
		/// Set coordinates to those specified in the constructor
		XYZ(double Coords[3]) {x=Coords[0];y=Coords[1];z=Coords[2];}
	};

	/// Struct for representing points in 2D space
	struct XY
	{
		double x, y;
		/// Set all coordinates to 0 as default constructor
		XY() {x=y=0;}
		/// Set coordinates to those specified in the constructor
		XY(double X, double Y) {x=X; y=Y;}
		/// Set coordinates to those specified in the constructor
		XY(double Coords[2]) {x=Coords[0];y=Coords[1];}
		/// Overload to see if two XY coordinates are the same
		bool operator ==(const XY &right) const
		{
			return x == right.x && y == right.y;
		}
		/// Check if all the coordinates are 0
		bool operator !() const
		{
			return (!x && !y);
		}
		operator bool() const
		{
			return (x || y);
		}
		double &operator [](int i)
		{
			return *(&x+i);
		}
		const double &operator [](int i) const
		{
			return *(&x+i);
		}

		/// Reverse all coordinates
		XY operator -()
		{
			XY ReturnVal;
			ReturnVal.x = -x;
			ReturnVal.y = -y;
			return ReturnVal;
		}
	};

	inline XY Convert(const XYZ &Val)
	{
		return XY(Val.x, Val.y);
	}

	inline bool operator == (const XYZ &left, const XYZ &right)
	{
		return (left.x==right.x && left.y==right.y && left.z==right.z);
	}

	inline bool operator == (const XYZ &left, const double &value)
	{
		return (left.x==value && left.y==value && left.z==value);
	}

	inline bool operator == (const XYZ &left, const int &value)
	{
		return (left.x==value && left.y==value && left.z==value);
	}

	inline WXYZ operator + (const WXYZ &left, const WXYZ &right)
	{
		WXYZ ReturnVal;
		ReturnVal.w = left.w + right.w;
		ReturnVal.x = left.x + right.x;
		ReturnVal.y = left.y + right.y;
		ReturnVal.z = left.z + right.z;
		return ReturnVal;
	}

	inline XYZ operator + (const XYZ &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x + right.x;
		ReturnVal.y = left.y + right.y;
		ReturnVal.z = left.z + right.z;
		return ReturnVal;
	}

	inline XY operator + (const XY &left, const XY &right)
	{
		XY ReturnVal;
		ReturnVal.x = left.x + right.x;
		ReturnVal.y = left.y + right.y;
		return ReturnVal;
	}

	inline WXYZ &operator += (WXYZ &left, const WXYZ &right)
	{
		left.w += right.w;
		left.x += right.x;
		left.y += right.y;
		left.z += right.z;
		return left;
	}

	inline XYZ &operator += (XYZ &left, const XYZ &right)
	{
		left.x += right.x;
		left.y += right.y;
		left.z += right.z;
		return left;
	}

	inline XY &operator += (XY &left, const XY &right)
	{
		left.x += right.x;
		left.y += right.y;
		return left;
	}

	inline WXYZ operator - (const WXYZ &left, const WXYZ &right)
	{
		WXYZ ReturnVal;
		ReturnVal.w = left.w - right.w;
		ReturnVal.x = left.x - right.x;
		ReturnVal.y = left.y - right.y;
		ReturnVal.z = left.z - right.z;
		return ReturnVal;
	}

	inline XYZ operator - (const XYZ &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x - right.x;
		ReturnVal.y = left.y - right.y;
		ReturnVal.z = left.z - right.z;
		return ReturnVal;
	}

	inline XY operator - (const XY &left, const XY &right)
	{
		XY ReturnVal;
		ReturnVal.x = left.x - right.x;
		ReturnVal.y = left.y - right.y;
		return ReturnVal;
	}

	inline WXYZ &operator -= (WXYZ &left, const WXYZ &right)
	{
		left.w -= right.w;
		left.x -= right.x;
		left.y -= right.y;
		left.z -= right.z;
		return left;
	}

	inline XYZ &operator -= (XYZ &left, const XYZ &right)
	{
		left.x -= right.x;
		left.y -= right.y;
		left.z -= right.z;
		return left;
	}

	inline XY &operator -= (XY &left, const XY &right)
	{
		left.x -= right.x;
		left.y -= right.y;
		return left;
	}

	/// Grassmann product multiplication between two quaternions
	inline WXYZ operator * (const WXYZ &left, const WXYZ &right)
	{
		WXYZ Quaternion;
		Quaternion.w = left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z;
		Quaternion.x = left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y;
		Quaternion.y = left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z;
		Quaternion.z = left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x;
		return Quaternion;
	}

	inline XYZ operator * (const XYZ &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x * right.x;
		ReturnVal.y = left.y * right.y;
		ReturnVal.z = left.z * right.z;
		return ReturnVal;
	}

	inline XY operator * (const XY &left, const XY &right)
	{
		XY ReturnVal;
		ReturnVal.x = left.x * right.x;
		ReturnVal.y = left.y * right.y;
		return ReturnVal;
	}

	/// Do a simple divide of all members, perhaps not mathematically correct but usefull
	inline XYZ operator / (const XYZ &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x / right.x;
		ReturnVal.y = left.y / right.y;
		ReturnVal.z = left.z / right.z;
		return ReturnVal;
	}

	/// Grassmann product multiplication between two quaternions
	inline WXYZ &operator *= (WXYZ &left, const WXYZ &right)
	{
		WXYZ Quaternion;
		Quaternion.w = left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z;
		Quaternion.x = left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y;
		Quaternion.y = left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z;
		Quaternion.z = left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x;
		left = Quaternion;
		return left;
	}

	inline XYZ &operator *= (XYZ &left, const XYZ &right)
	{
		left.x *= right.x;
		left.y *= right.y;
		left.z *= right.z;
		return left;
	}

	inline XY &operator *= (XY &left, const XY &right)
	{
		left.x *= right.x;
		left.y *= right.y;
		return left;
	}

	inline XYZ operator * (const XYZ &left, const double &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x * right;
		ReturnVal.y = left.y * right;
		ReturnVal.z = left.z * right;
		return ReturnVal;
	}

	inline XYZ operator * (const XYZ &left, const float &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x * right;
		ReturnVal.y = left.y * right;
		ReturnVal.z = left.z * right;
		return ReturnVal;
	}

	inline XYZ operator * (const XYZ &left, const int &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x * right;
		ReturnVal.y = left.y * right;
		ReturnVal.z = left.z * right;
		return ReturnVal;
	}

	inline XY operator * (const XY &left, const double &right)
	{
		XY ReturnVal;
		ReturnVal.x = left.x * right;
		ReturnVal.y = left.y * right;
		return ReturnVal;
	}

	inline XYZ operator * (const double &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = right.x * left;
		ReturnVal.y = right.y * left;
		ReturnVal.z = right.z * left;
		return ReturnVal;
	}

	inline XYZ operator * (const float &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = right.x * left;
		ReturnVal.y = right.y * left;
		ReturnVal.z = right.z * left;
		return ReturnVal;
	}

	inline XYZ operator * (const int &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = right.x * left;
		ReturnVal.y = right.y * left;
		ReturnVal.z = right.z * left;
		return ReturnVal;
	}

	inline XY operator * (const double &left, const XY &right)
	{
		XY ReturnVal;
		ReturnVal.x = right.x * left;
		ReturnVal.y = right.y * left;
		return ReturnVal;
	}

	inline XYZ operator / (const XYZ &left, const double &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x / right;
		ReturnVal.y = left.y / right;
		ReturnVal.z = left.z / right;
		return ReturnVal;
	}

	inline XY operator / (const XY &left, const double &right)
	{
		XY ReturnVal;
		ReturnVal.x = left.x / right;
		ReturnVal.y = left.y / right;
		return ReturnVal;
	}

	inline WXYZ &operator /= (WXYZ &left, const double &right)
	{
		double dDivisor = 1/right;
		left.w *= dDivisor;
		left.x *= dDivisor;
		left.y *= dDivisor;
		left.z *= dDivisor;
		return left;
	}

	inline XYZ &operator /= (XYZ &left, const double &right)
	{
		double dDivisor = 1/right;
		left.x *= dDivisor;
		left.y *= dDivisor;
		left.z *= dDivisor;
		return left;
	}

	inline XY &operator /= (XY &left, const double &right)
	{
		double dDivisor = 1/right;
		left.x *= dDivisor;
		left.y *= dDivisor;
		return left;
	}

	inline XYZ &operator *= (XYZ &left, const double &right)
	{
		left.x = left.x * right;
		left.y = left.y * right;
		left.z = left.z * right;
		return left;
	}

	inline XY &operator *= (XY &left, const double &right)
	{
		left.x = left.x * right;
		left.y = left.y * right;
		return left;
	}

	inline XYZ operator / (const XYZ &left, const int &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.x / right;
		ReturnVal.y = left.y / right;
		ReturnVal.z = left.z / right;
		return ReturnVal;
	}

	inline std::ostream &operator << (std::ostream &output, const WXYZ &Quat)
	{
		output << Quat.w << ", " << Quat.x << ", " << Quat.y << ", " << Quat.z;
		return output;
	}

	inline std::ostream &operator << (std::ostream &output, const XYZ &Vector)
	{
		output << Vector.x << ", " << Vector.y << ", " << Vector.z;
		return output;
	}

	inline std::ostream &operator << (std::ostream &output, const XY &Vector)
	{
		output << Vector.x << ", " << Vector.y;
		return output;
	}

	inline std::istream &operator >> (std::istream &input, WXYZ &Quat)
	{
		input >> Quat.w; input.ignore(); // ignore the comma
		input >> Quat.x; input.ignore(); // ignore the comma
		input >> Quat.y; input.ignore(); // ignore the comma
		input >> Quat.z;
		return input;
	}

	inline std::istream &operator >> (std::istream &input, XYZ &Vector)
	{
		input >> Vector.x; input.ignore(); // ignore the comma
		input >> Vector.y; input.ignore(); // ignore the comma
		input >> Vector.z;
		return input;
	}

	inline std::istream &operator >> (std::istream &input, XY &Vector)
	{
		input >> Vector.x; input.ignore(); // ignore the comma
		input >> Vector.y;
		return input;
	}

	/// Get the dot product of two vectors
	inline double DotProduct(const XYZ &left, const XYZ &right)
	{
		return (left.x*right.x + left.y*right.y + left.z*right.z);
	}

	/// Get the dot product of two vectors
	inline double DotProduct(const XY &left, const XY &right)
	{
		return (left.x*right.x + left.y*right.y);
	}

	/// Get the cross product of two vectors
	inline XYZ CrossProduct(const XYZ &left, const XYZ &right)
	{
		XYZ ReturnVal;
		ReturnVal.x = left.y*right.z - right.y*left.z;
		ReturnVal.y = left.z*right.x - right.z*left.x;
		ReturnVal.z = left.x*right.y - right.x*left.y;
		return ReturnVal;
	}

	/// Generate a random number between the limits given
	inline double RandomNumber(double dMin, double dMax)
	{
		return ((double(rand())/RAND_MAX)*(dMax-dMin))+dMin;
	}

	/// Get the length between two points
	inline double GetLength(const XYZ &Point1, const XYZ &Point2)
	{
		return sqrt((Point2.x-Point1.x)*(Point2.x-Point1.x)+
					(Point2.y-Point1.y)*(Point2.y-Point1.y)+
					(Point2.z-Point1.z)*(Point2.z-Point1.z));
	}

	/// Get the length squared between two points
	inline double GetLengthSquared(const XYZ &Point1, const XYZ &Point2)
	{
		return ((Point2.x-Point1.x)*(Point2.x-Point1.x)+
				(Point2.y-Point1.y)*(Point2.y-Point1.y)+
				(Point2.z-Point1.z)*(Point2.z-Point1.z));
	}

	/// Get the length between two points
	inline double GetLength(const XY &Point1, const XY &Point2)
	{
		return sqrt((Point2.x-Point1.x)*(Point2.x-Point1.x)+
					(Point2.y-Point1.y)*(Point2.y-Point1.y));
	}

	/// Get the length squared between two points
	inline double GetLengthSquared(const XY &Point1, const XY &Point2)
	{
		return ((Point2.x-Point1.x)*(Point2.x-Point1.x)+
					(Point2.y-Point1.y)*(Point2.y-Point1.y));
	}

	/// Get the length of a quaternion
	inline double GetLength(const WXYZ &Quaternion)
	{
		return sqrt((Quaternion.w)*(Quaternion.w)+
					(Quaternion.x)*(Quaternion.x)+
					(Quaternion.y)*(Quaternion.y)+
					(Quaternion.z)*(Quaternion.z));
	}

	/// Get the length of a vector
	inline double GetLength(const XYZ &Vector)
	{
		return sqrt((Vector.x)*(Vector.x)+
					(Vector.y)*(Vector.y)+
					(Vector.z)*(Vector.z));
	}

	/// Get the length of a vector
	inline double GetLength(const XY &Vector)
	{
		return sqrt((Vector.x)*(Vector.x)+
					(Vector.y)*(Vector.y));
	}

	/// Get the length squared of a vector
	inline double GetLengthSquared(const XYZ &Point)
	{
		return ((Point.x)*(Point.x)+
				(Point.y)*(Point.y)+
				(Point.z)*(Point.z));
	}

	/// Get the length squared of a vector
	inline double GetLengthSquared(const XY &Point)
	{
		return ((Point.x)*(Point.x)+
				(Point.y)*(Point.y));
	}

	/// Normalise the quaternion and return it
	inline WXYZ &Normalise(WXYZ &Quaternion)
	{
		double dLength = GetLength(Quaternion);
		if (dLength)
			Quaternion /= dLength;
		else
			assert(false);
		return Quaternion;
	}

	/// Normalise the vector and return it
	inline XYZ &Normalise(XYZ &Vector)
	{
		double dLength = GetLength(Vector);
		if (dLength)
			Vector /= dLength;
		else
			assert(false);
		return Vector;
	}

	/// Normalise the vector and return it
	inline XY &Normalise(XY &Vector)
	{
		double dLength = GetLength(Vector);
		if (dLength)
			Vector /= dLength;
		else
			assert(false);
		return Vector;
	}

	/// Get maximum element of vector and return it
	inline double Max( XYZ &Vector )
	{
		double max = Vector.x;
		if ( Vector.y > max )
			max = Vector.y;
		if ( Vector.z > max )
			max = Vector.z;
		return max;
	}

	/// Create a rotation Quaternion out of a normalised Axis and angle
	inline WXYZ::WXYZ(XYZ Axis, double dAngle)
	{
		w = cos(0.5*dAngle);
		double dCoeff = sin(0.5*dAngle);
		x = Axis.x*dCoeff;
		y = Axis.y*dCoeff;
		z = Axis.z*dCoeff;
	}

	/// Create a rotation Quaternion out of a new coordinate system
	inline WXYZ::WXYZ(XYZ X, XYZ Y, XYZ Z)
	{
		double dTrace = 1 + X.x + Y.y + Z.z;
		double S;
		if (dTrace > 1e-6)
		{
			S = 1/(2*sqrt(dTrace));
			w = 1/(4*S);
			x = (Y.z - Z.y)*S;
			y = (Z.x - X.z)*S;
			z = (X.y - Y.x)*S;
		}
		else
		{
			// NOTE: This part has not been tested, may not work correctly
			if ((X.x > Y.y)&&(X.x > Z.z))
			{ 
				S = sqrt( 1.0 + X.x - Y.y - Z.z ) * 2; // S=4*qx 
				x = 0.25 * S;
				y = (Y.x + X.y ) / S; 
				z = (Z.x + X.z ) / S; 
				w = (Y.z - Z.y ) / S;
			}
			else if (Y.y > Z.z)
			{ 
				S = sqrt( 1.0 + Y.y - X.x - Z.z ) * 2; // S=4*qy
				x = (Y.x + X.y ) / S; 
				y = 0.25 * S;
				z = (Z.y + Y.z ) / S; 
				w = (Z.x - X.z ) / S;
			}
			else
			{ 
				S = sqrt( 1.0 + Z.z - X.x - Y.y ) * 2; // S=4*qz
				x = (Z.x + X.z ) / S; 
				y = (Z.y + Y.z ) / S; 
				z = 0.25 * S;
				w = (X.y - Y.x ) / S;
			} 
		}
	}

	/// Create a rotation Quaternion out of euler angles
	/**
	Rotations are applied in the following order: dAzimuth, dElevation, dTilt
	\param dAzimuth Rotation about z axis
	\param dElevation Rotation about y axis
	\param dTilt Rotation about x axis
	*/
	inline WXYZ::WXYZ(double dAzimuth, double dElevation, double dTilt)
	{
		double c1 = cos(dAzimuth / 2);
		double c2 = cos(dElevation / 2);
		double c3 = cos(dTilt / 2);
		double s1 = sin(dAzimuth / 2);
		double s2 = sin(dElevation / 2);
		double s3 = sin(dTilt / 2);
		w = c1*c2*c3 - s1*s2*s3;
		x = s1*s2*c3 + c1*c2*s3;
		y = c1*s2*c3 - s1*c2*s3;
		z = s1*c2*c3 + c1*s2*s3;
	}

	/// Rotate a vector by given Quaternion
	inline XYZ operator * (const WXYZ &left, const XYZ &right)
	{
		// nVidia SDK implementation
		XYZ uv, uuv;
		XYZ qvec(left.x, left.y, left.z);
		uv = CrossProduct(qvec, right);
		uuv = CrossProduct(qvec, uv);
		uv *= (2.0f * left.w);
		uuv *= 2.0f;

		return right + uv + uuv;
	}

	/// Check if two spheres intersect given their center points and combined radii
	inline bool SphereSphereIntersect(const XYZ &p1, const XYZ &p2, double dRadii)
	{
		return (GetLength(p1, p2) <= dRadii);
	}

	/// http://http://paulbourke.net/geometry/pointlineplane/
	/// Find the shortest distance between a point and a line. Return the closest point on the line
	/// and set dU to be the fraction along the line where the intersection occurs
	inline XYZ ShortestDistPointLine(const XYZ &Point, const XYZ &LineStart, const XYZ &LineEnd, double &dU)
	{
		double dLineLengthSquared;
		XYZ Intersection;

		dLineLengthSquared = GetLengthSquared(LineEnd, LineStart);

		dU = DotProduct(Point-LineStart, LineEnd-LineStart) / ( dLineLengthSquared );

		Intersection = LineStart + dU * ( LineEnd - LineStart );

		return Intersection;
	}

	/// http://astronomy.swin.edu.au/~pbourke/geometry/pointline/
	/// Find if a sphere and rounded cylinder intersect given the center of the sphere, the centers of the two end points
	/// of the cylinder and the combined radii of the sphere and clyinder. Return true if there is intersection else false.
	/// pdUReturn will be set to the fraction along the line at which the intersection occurs, this can be set to NULL if
	/// the information is not needed.
	inline bool SphereCylinderIntersect(const XYZ &Point, const XYZ &LineStart, const XYZ &LineEnd, double dRadii, double* pdUReturn = NULL)
	{
		double dU;
		XYZ Intersection;

		Intersection = ShortestDistPointLine(Point, LineStart, LineEnd, dU);

 		// If the closest point on the line to the sphere is outside the line segment
		// do an intersection test with the sphere created at the end or start
		// of the line segment depending on which side it lies.
		if (dU < 0)
		{
			if (pdUReturn)
				*pdUReturn = 0;
			return SphereSphereIntersect(LineStart, Point, dRadii);
		}
		if (dU > 1)
		{
			if (pdUReturn)
				*pdUReturn = 1;
			return SphereSphereIntersect(LineEnd, Point, dRadii);
		}

		if (pdUReturn)
			*pdUReturn = dU;
		// Check that the distance between the intersection and the point is less than the radius
		return SphereSphereIntersect(Intersection, Point, dRadii);
	}

	inline bool LineLineIntersect2D(const XY &p1, const XY &p2, const XY &p3, const XY &p4, double &dU1, double &dU2)
	{
		double denom;
		denom = (p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y);
		if (!denom)
			return false;
		dU1 = (p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x);
		dU2 = (p2.x-p1.x)*(p1.y-p3.y)-(p2.y-p1.y)*(p1.x-p3.x);
		dU1 /= denom;
		dU2 /= denom;
		if (dU1 < 0 || dU1 > 1 ||
			dU2 < 0 || dU2 > 1)
			return false;
		return true;
	}

	/// http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
	/// Find the shortest distance between two lines given two points on each line p1, p2 and p3, p4. dU1 is the fraction along the length
	/// of the first line which is closest to the second line, similarly dU2 is the fraction along the length of the second line closest to the first line.
	/// Returns true if the function succeded, returns false if there was an error such as lines are parallel. pa and pb return the two closest points
	/// lying on the lines 1 and 2 respectively.
	inline bool ShortestDistLineLine(const XYZ &p1, const XYZ &p2, const XYZ &p3, const XYZ &p4, double &dU1, double &dU2, XYZ &pa, XYZ &pb)
	{
	//	XYZ pa, pb;
		XYZ p13,p43,p21;
		double d1343,d4321,d1321,d4343,d2121;
		double numer,denom;
	//	double mua, mub;

		p13 = p1 - p3;
		p43 = p4 - p3;
		// If the length of line 2 is 0 return false
		if (!p43)
			return false;
		p21 = p2 - p1;
		// If the length of line 1 is 0 return false
		if (!p21)
			return false;

		d1343 = DotProduct(p13, p43);
		d4321 = DotProduct(p43, p21);
		d1321 = DotProduct(p13, p21);
		d4343 = DotProduct(p43, p43);
		d2121 = DotProduct(p21, p21);

		denom = d2121 * d4343 - d4321 * d4321;
		// If the denominator is 0 then the lines are parallel in which case
		if (!denom)
			return false;

		numer = d1343 * d4321 - d1321 * d4343;

		dU1 = numer / denom;
		dU2 = (d1343 + d4321 * dU1) / d4343;

		pa = p1 + dU1 * (p2 - p1);
		pb = p3 + dU2 * (p4 - p3);

		return true;
	}

	/// http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
	/// Finds the intersection between two rounded cylinders given the center of the ends of each cylinder and the combined radii of the two cylinders.
	/// pdU1 is set to the fraction along the length of cylinder 1 where intersection occurs, similarly pdU2 is set to the fraction along the length of
	/// cylinder2 where the intersection occurs. Either or both of these parameters can be set to null/omitted if the information is not needed.
	inline bool RoundedCylinderIntersect(const XYZ &p1, const XYZ &p2, const XYZ &p3, const XYZ &p4, double dRadii, double* pdU1 = NULL, double* pdU2 = NULL)
	{
		double mua, mub;
		XYZ pa, pb;

		if (!ShortestDistLineLine(p1, p2, p3, p4, mua, mub, pa, pb))
			return false;

		if (pdU1)
			*pdU1 = mua;
		if (pdU2)
			*pdU2 = mub;

		if (!SphereSphereIntersect(pa, pb, dRadii))
			return false;
		int iCase = 0;

		if (mua < 0)
		{
			if (pdU1)
				*pdU1 = 0;
			iCase |= 1<<0;
		}
		if (mua > 1)
		{
			if (pdU1)
				*pdU1 = 1;
			iCase |= 1<<1;
		}
		if (mub < 0)
		{
			if (pdU2)
				*pdU2 = 0;
			iCase |= 1<<2;
		}
		if (mub > 1)
		{
			if (pdU2)
				*pdU2 = 1;
			iCase |= 1<<3;
		}

	/*	CASE:
		=====

		0 : mua & mub are within bounds

		1 : mua < 0
		2 : mua > 1
		4 : mub < 0
		8 : mub > 1

		5 : mua < 0 & mub < 0
		6 : mua > 1 & mub < 0
		9 : mua < 0 & mub > 1
		10: mua > 1 & mub > 1

		3, 15 : Impossible (mua or mub can't be bigger than 1 AND smaller than 0)
	*/
		double dOverlap1, dOverlap2;

		switch (iCase)
		{
		case 0:		// mua & mub are within bounds
			return true;	// We already checked if the distance between points was less than radius
	//		return SphereSphereIntersect(pa, pb, dRadii);
		case 1:		// mua < 0
			return SphereCylinderIntersect(p1, p3, p4, dRadii, pdU2);
		case 2:		// mua > 1
			return SphereCylinderIntersect(p2, p3, p4, dRadii, pdU2);
		case 4:		// mub < 0
			return SphereCylinderIntersect(p3, p1, p2, dRadii, pdU1);
		case 8:		// mub > 1
			return SphereCylinderIntersect(p4, p1, p2, dRadii, pdU1);
		case 5:		// mua < 0 & mub < 0
			dOverlap1 = GetLength(p2 - p1) * -mua;
			dOverlap2 = GetLength(p4 - p3) * -mub;
			if (dOverlap1 > dOverlap2)
				return SphereCylinderIntersect(p1, p3, p4, dRadii, pdU2);
			else
				return SphereCylinderIntersect(p3, p1, p2, dRadii, pdU1);
		case 6:		// mua > 1 & mub < 0
			dOverlap1 = GetLength(p2 - p1) * (mua-1);
			dOverlap2 = GetLength(p4 - p3) * -mub;
			if (dOverlap1 > dOverlap2)
				return SphereCylinderIntersect(p2, p3, p4, dRadii, pdU2);
			else
				return SphereCylinderIntersect(p3, p1, p2, dRadii, pdU1);
		case 9:		// mua < 0 & mub > 1
			dOverlap1 = GetLength(p2 - p1) * -mua;
			dOverlap2 = GetLength(p4 - p3) * (mub-1);
			if (dOverlap1 > dOverlap2)
				return SphereCylinderIntersect(p1, p3, p4, dRadii, pdU2);
			else
				return SphereCylinderIntersect(p4, p1, p2, dRadii, pdU1);
		case 10:	// mua > 1 & mub > 1
			dOverlap1 = GetLength(p2 - p1) * (mua-1);
			dOverlap2 = GetLength(p4 - p3) * (mub-1);
			if (dOverlap1 > dOverlap2)
				return SphereCylinderIntersect(p2, p3, p4, dRadii, pdU2);
			else
				return SphereCylinderIntersect(p4, p1, p2, dRadii, pdU1);
		}
		assert(false);		// Should never reach this point
		return false;
	}

	/// Find if two AABBs intersect with given tolerance
	inline bool BoundingBoxIntersect(const XYZ &BBox1Min, const XYZ &BBox1Max, const XYZ &BBox2Min, const XYZ &BBox2Max, double dTolerance = 0)
	{
		if (BBox1Min.x > BBox2Max.x + dTolerance)
			return false;
		if (BBox1Max.x < BBox2Min.x - dTolerance)
			return false;
		if (BBox1Min.y > BBox2Max.y + dTolerance)
			return false;
		if (BBox1Max.y < BBox2Min.y - dTolerance)
			return false;
		if (BBox1Min.z > BBox2Max.z + dTolerance)
			return false;
		if (BBox1Max.z < BBox2Min.z - dTolerance)
			return false;
		return true;
	}

	/// Find if a point is inside an Axis Aligned Bounding Box with given tolerance
	inline bool PointInsideBox(const XYZ &Point, const XYZ &BoxMin, const XYZ &BoxMax, double dTolerance = 0)
	{
		if (Point.x > BoxMax.x + dTolerance)
			return false;
		if (Point.x < BoxMin.x - dTolerance)
			return false;
		if (Point.y > BoxMax.y + dTolerance)
			return false;
		if (Point.y < BoxMin.y - dTolerance)
			return false;
		if (Point.z > BoxMax.z + dTolerance)
			return false;
		if (Point.z < BoxMin.z - dTolerance)
			return false;
		return true;
	}

	/// Get a local coordinate system for given 2 points, where the z axis corresponds to the line between points 1 and 2.
	/// The X and Y axis will be perpendicalar to this one, with Y pointing upwards
	inline void GetLocalCoordinateSystem(XYZ &LocalX, XYZ &LocalY, XYZ &LocalZ, const XYZ &Point1, const XYZ &Point2)
	{
		XYZ GlobalZ(0,0,1);
		XYZ GlobalY(0,1,0);
		LocalX = Point2-Point1;
		Normalise(LocalX);
		LocalY = CrossProduct(GlobalZ, LocalX);
		// If LocalX is exactly in line with GlobalZ then we have a problem
		// just choose a random axis and take cross product of LocalX with it
		// to get LocalY. It doesn't matter what the axis LocalY/Z are as long
		// as they are perpendicular to LocalX and each other
		if (LocalY == 0)
			LocalY = CrossProduct(GlobalY, LocalX);
		Normalise(LocalY);
		LocalZ = CrossProduct(LocalX, LocalY);
	}

	/// P1 and P2 are two points on the line, P3 is a point on the plane, N is the plane normal, Intersection is the returned intersection
	inline bool GetIntersectionLinePlane(const XYZ &p1, const XYZ &p2, const XYZ &p3, const XYZ &N, XYZ &Intersection, double *pdU = NULL)
	{
		double dU;
		double dNumer, dDenom;
		dNumer = DotProduct(N, p3-p1);
		dDenom = DotProduct(N, p2-p1);
		if (!dDenom)
			return false;
		dU = dNumer/dDenom;
		Intersection = p1 + (p2-p1)*dU;
		if (pdU)
		{
			*pdU = dU;
		}
		return true;
	}

	/// Create a random color from a set of pre-defined colors
	inline void GetRandomColor(double &r, double &g, double &b)
	{
		const int iMaxIndex = 12;
		double ColorArray[iMaxIndex][3] =
		{
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 },
			{ 1, 1, 0 },
			{ 0, 1, 1 },
			{ 1, 0, 1 },
			{ 1, 0.5, 0 },
			{ 1, 0, 0.5 },
			{ 0.5, 1, 0 },
			{ 0, 1, 0.5 },
			{ 0.5, 0, 1 },
			{ 0, 0.5, 1 },
		};

		int iIndex;
		do
		{
			iIndex = int(RandomNumber(0, iMaxIndex));
		} while (iIndex < 0 || iIndex >= iMaxIndex);

		r = ColorArray[iIndex][0];
		g = ColorArray[iIndex][1];
		b = ColorArray[iIndex][2];
	}

	/**
	Given a fraction between 0 and 1, create an rgb color where 0 is blue, 0.5 is green and 1 is red
	anything in between is a mixture of the three colors.
	*/
	inline void GetFractionColor(double dFraction, double &r, double &g, double &b)
	{
		if (dFraction<0)
		{
			dFraction = 0;
		}
		else if (dFraction>1)
		{
			dFraction = 1;
		}

		r = g = b = 0.0f;
		if (dFraction > 0.75f)
			r = 1.0f;
		else if (dFraction > 0.5f)
			r = (dFraction-0.5f)*4;

		if (dFraction < 0.25f)
			b = 1.0f;
		else if (dFraction < 0.5f)
			b = (0.5f-dFraction)*4;

		if (dFraction > 0.75f)
			g = (1.0f-dFraction)*4;
		else if (dFraction > 0.25f)
			g = 1;
		else
			g = dFraction*4;
	}

	/// Given three points lying on the edge of a circle, find the center point. false is returned if there is an
	/// error calculating them (e.g. all 3 points lie on a straight line).
	inline bool GetCircleCenter(const XYZ &A, const XYZ &B, const XYZ &C, XYZ &Center)
	{
		XYZ E, F, G, H, N;
		N = CrossProduct(A-B, C-B);
		if (GetLength(N) < 0.000001)
			return false;
		Normalise(N);
		E = (A+B)/2;
		G = (B+C)/2;
		F = E + CrossProduct(A-B, N);
		H = G + CrossProduct(C-B, N);
		double dU1, dU2;
		XYZ I1, I2;
		if (!ShortestDistLineLine(E, F, G, H, dU1, dU2, I1, I2))
			return false;
	//	assert(GetLength(I1, I2)<0.000001);
		if (GetLength(I1, I2)>0.000001)
			return false;
		Center = (I1+I2)/2;
		return true;
	}

	/// Given two points, return a new point who's coordinates are the greater of the two
	inline XYZ Max(const XYZ &P1, const XYZ &P2)
	{
		XYZ Return;
		Return.x = P1.x > P2.x ? P1.x : P2.x;
		Return.y = P1.y > P2.y ? P1.y : P2.y;
		Return.z = P1.z > P2.z ? P1.z : P2.z;
		return Return;
	}

	/// Given two points, return a new point who's coordinates are the smaller of the two
	inline XYZ Min(const XYZ &P1, const XYZ &P2)
	{
		XYZ Return;
		Return.x = P1.x < P2.x ? P1.x : P2.x;
		Return.y = P1.y < P2.y ? P1.y : P2.y;
		Return.z = P1.z < P2.z ? P1.z : P2.z;
		return Return;
	}

	/// Given two points, return a new point who's coordinates are the greater of the two
	inline XY Max(const XY &P1, const XY &P2)
	{
		XY Return;
		Return.x = P1.x > P2.x ? P1.x : P2.x;
		Return.y = P1.y > P2.y ? P1.y : P2.y;
		return Return;
	}

	/// Given two points, return a new point who's coordinates are the smaller of the two
	inline XY Min(const XY &P1, const XY &P2)
	{
		XY Return;
		Return.x = P1.x < P2.x ? P1.x : P2.x;
		Return.y = P1.y < P2.y ? P1.y : P2.y;
		return Return;
	}

	inline int Round(double dValue) 
	{
		return int(dValue + (dValue > 0 ? 0.5 : -0.5));
	}

	template<typename T>
	inline T CalculateBezierPoint(T p1, T p2, T p3, T p4, double mu)
	{		// from http://astronomy.swin.edu.au/~pbourke/curves/bezier/ Now http://local.wasp.uwa.edu.au/~pbourke/geometry/bezier/index2.html
		double mum1 = 1 - mu;

		return (mum1*mum1*mum1)*p1 + (3*mu*mum1*mum1)*p2 + (3*mu*mu*mum1)*p3 + (mu*mu*mu)*p4;
	}

	template<typename T>
	inline T CalculateBezierTangent(T p1, T p2, T p3, T p4, double mu)
	{		// This is the partial derivative of the above function with respect to mu
		double mum1 = 1 - mu;

		return (-3*mum1*mum1)*p1 + (3*((mum1*mum1)-2*mu*mum1))*p2 + (3*(2*mu*mum1-mu*mu))*p3 + (3*mu*mu)*p4;
	}

	inline double RandomNormalDistribution(double dStandrdDeviation = 1, double dAverage = 0)
	{		// from http://home.online.no/~pjacklam/notes/invnorm/
		const double A1 = -3.969683028665376e+01;
		const double A2 = 2.209460984245205e+02;
		const double A3 = -2.759285104469687e+02;
		const double A4 =  1.383577518672690e+02;
		const double A5 = -3.066479806614716e+01;
		const double A6 =  2.506628277459239e+00;

		const double B1 = -5.447609879822406e+01;
		const double B2 =  1.615858368580409e+02;
		const double B3 = -1.556989798598866e+02;
		const double B4 =  6.680131188771972e+01;
		const double B5 = -1.328068155288572e+01;

		const double C1 = -7.784894002430293e-03;
		const double C2 = -3.223964580411365e-01;
		const double C3 = -2.400758277161838e+00;
		const double C4 = -2.549732539343734e+00;
		const double C5 = 4.374664141464968e+00;
		const double C6 = 2.938163982698783e+00;

		const double D1 = 7.784695709041462e-03;
		const double D2 = 3.224671290700398e-01;
		const double D3 = 2.445134137142996e+00;
		const double D4 = 3.754408661907416e+00;

		const double P_LOW = 0.02425;
		const double P_HIGH = 0.97575;	// P_high = 1 - p_low

		double q, r, x;

		double p;
		do
		{
			p = RandomNumber(0, 1);	// Get a random number between 0 and 1
		} while (p <= 0 || p >= 1);	// Repeat making sure p is not 0 or 1

		if (p > 0 && p < P_LOW){
			q = sqrt(-2*log(p));
			x = (((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1);
		}
		else if (p >= P_LOW && p <= P_HIGH)
		{
			q = p - 0.5;
			r = q*q;
			x = (((((A1*r+A2)*r+A3)*r+A4)*r+A5)*r+A6)*q /(((((B1*r+B2)*r+B3)*r+B4)*r+B5)*r+1);
		}
		else if (p > P_HIGH && p < 1){
			q = sqrt(-2*log(1-p));
			x = -(((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1);
		}
		return x*dStandrdDeviation+dAverage;
	}

	inline double GetArea(XYZ Points[], int iNumPoints)
	{
		double dArea = 0;
		int i;
		XYZ Point1, Point2;
		for (i=0; i<iNumPoints; ++i)
		{
			Point1 = Points[i];
			Point2 = Points[(i+1)%iNumPoints];
			dArea += (Point1.x-Point2.x)*(Point1.y+Point2.y);
		}
		return 0.5*dArea;
	}

	/// P1, P2, P3 are the three triangle corners, P is the points to be tested
	inline bool PointInsideTriangle(const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P)
	{
		const double dTolerance = 1e-6;
		XYZ N = CrossProduct(P2 - P1, P3 - P1);
		XYZ vert0p = P1 - P; XYZ vert1p = P2 - P;
		if (DotProduct(CrossProduct(vert0p, vert1p), N) < -dTolerance) return false;
		XYZ vert2p = P3 - P;
		if (DotProduct(CrossProduct(vert1p, vert2p), N) < -dTolerance) return false;
		if (DotProduct(CrossProduct(vert2p, vert0p), N) < -dTolerance) return false;
		return true;
	}

	/// P1, P2, P3 are the three triangle corners, P is the points to be tested, N is the normal to the triangle
	inline bool PointInsideTriangle(const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P, const XYZ &N)
	{
		const double dTolerance = 1e-6;
		XYZ vert0p = P1 - P; XYZ vert1p = P2 - P;
		if (DotProduct(CrossProduct(vert0p, vert1p), N) < -dTolerance) return false;
		XYZ vert2p = P3 - P;
		if (DotProduct(CrossProduct(vert1p, vert2p), N) < -dTolerance) return false;
		if (DotProduct(CrossProduct(vert2p, vert0p), N) < -dTolerance) return false;
		return true;
	}

	/// P1, P2, P3 are the three triangle corners, P is the points to be tested, N is the normal to the triangle
	inline double PointInsideTriangleAccuracy(const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P, const XYZ &N)
	{
		XYZ vert0p = P1 - P; XYZ vert1p = P2 - P;
		XYZ vert2p = P3 - P;
		double dMin = DotProduct(CrossProduct(vert0p, vert1p), N);
		dMin = std::min(dMin, DotProduct(CrossProduct(vert1p, vert2p), N));
		dMin = std::min(dMin, DotProduct(CrossProduct(vert2p, vert0p), N));
		return dMin;
	}

	/// P1, P2, P3 are the three triangle corners, P is the points to be tested, ignoring the z component
	inline bool PointInsideTriangle2D(const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P, const XYZ *pNormal = NULL)
	{
		double dN;
		if (!pNormal)
			dN = (P2.x-P1.x)*(P3.y-P1.y) - (P3.x-P1.x)*(P2.y-P1.y);
		else
			dN = pNormal->z;
		XYZ V1 = P1 - P;
		XYZ V2 = P2 - P;
		if ((V1.x*V2.y-V2.x*V1.y) * dN < 0.0)
			return false;
		XYZ V3 = P3 - P;
		if ((V2.x*V3.y-V3.x*V2.y) * dN < 0.0)
			return false;
		if ((V3.x*V1.y-V1.x*V3.y) * dN < 0.0)
			return false;
		return true;
	}

	/// P1, P2, P3 are the three triangle corners, P is the points to be tested
	inline double ShortestDistPointTriangle(const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P, XYZ *pTrianglePoint = NULL, XYZ *pNormal = NULL)
	{
		// First find the closest distance between the point and the plane

		XYZ Normal;

		if (pNormal)
		{
			Normal = *pNormal;
		}
		else
		{
			Normal = CrossProduct(P2-P1, P3-P1);
			Normalise(Normal);
		}

		XYZ ClosestPlanePoint;

		double dDist = DotProduct(Normal, P1 - P);
		ClosestPlanePoint = P+Normal*dDist;

		// Check that the point on the plane lies inside the triangle

		if (PointInsideTriangle(P1, P2, P3, ClosestPlanePoint))
		{
			if (pTrianglePoint)
				*pTrianglePoint = ClosestPlanePoint;
			return fabs(dDist);
		}

		// If not find the closest distance between each of the edges of the lines
		// also making sure that the closest point lies within the triangle boundaries

		XYZ X1, X2, X3;
		double u1, u2, u3;

		X1 = ShortestDistPointLine(P, P1, P2, u1);
		X2 = ShortestDistPointLine(P, P2, P3, u2);
		X3 = ShortestDistPointLine(P, P3, P1, u3);

		double dShortestDist = -1;

		if (u1 >= 0 && u1 <= 1)
		{
			dDist = GetLength(X1, P);
			if (dDist < dShortestDist || dShortestDist < 0)
			{
				dShortestDist = dDist;
				if (pTrianglePoint)
					*pTrianglePoint = X1;
			}
		}

		if (u2 >= 0 && u2 <= 1)
		{
			dDist = GetLength(X2, P);
			if (dDist < dShortestDist || dShortestDist < 0)
			{
				dShortestDist = dDist;
				if (pTrianglePoint)
					*pTrianglePoint = X2;
			}
		}

		if (u3 >= 0 && u3 <= 1)
		{
			dDist = GetLength(X3, P);
			if (dDist < dShortestDist || dShortestDist < 0)
			{
				dShortestDist = dDist;
				if (pTrianglePoint)
					*pTrianglePoint = X3;
			}
		}

		// Also check for the closest corner, and take the shortest of all

		dDist = GetLength(P1, P);
		if (dDist < dShortestDist || dShortestDist < 0)
		{
			dShortestDist = dDist;
			if (pTrianglePoint)
				*pTrianglePoint = P1;
		}

		dDist = GetLength(P2, P);
		if (dDist < dShortestDist || dShortestDist < 0)
		{
			dShortestDist = dDist;
			if (pTrianglePoint)
				*pTrianglePoint = P2;
		}

		dDist = GetLength(P3, P);
		if (dDist < dShortestDist || dShortestDist < 0)
		{
			dShortestDist = dDist;
			if (pTrianglePoint)
				*pTrianglePoint = P3;
		}

		return dShortestDist;
	}

	/// Get the barycentric coordinates of a point lying on a triangle
	inline XYZ GetBarycentricCoordinates(const XY &P1, const XY &P2, const XY &P3, const XY &P)
	{
		XYZ Coordinates;
		double denom = (-P1.x * P3.y - P2.x * P1.y + P2.x * P3.y + P1.y * P3.x + P2.y * P1.x - P2.y * P3.x);

		if (fabs(denom) >= 1e-6)
		{
			Coordinates.x = (P2.x * P3.y - P2.y * P3.x - P.x * P3.y + P3.x * P.y - P2.x * P.y + P2.y * P.x) / denom;
			Coordinates.y = -(-P1.x * P.y + P1.x * P3.y + P1.y * P.x - P.x * P3.y + P3.x * P.y - P1.y * P3.x) / denom;
	//		Coordinates.z = (-P1.x * P.y + P2.y * P1.x + P2.x * P.y - P2.x * P1.y - P2.y * P.x + P1.y * P.x) / denom;
		}
		Coordinates.z = 1 - Coordinates.x - Coordinates.y;

		return Coordinates;
	}

	/// Get the point in a vector of 2D points which is closest to the specified position
	/// Returns the index of the point
	inline int GetClosestPointIndex( const std::vector<XY> &Points, const XY Position)
	{
		std::vector<XY>::const_iterator itPoint;
		double dClosestDistSqrd = -1;
		double dDistSqrd;
		int iClosestIndex = -1, i;
		for (itPoint = Points.begin(), i=0; itPoint != Points.end(); ++itPoint, ++i)
		{
			dDistSqrd = GetLengthSquared(Position, *itPoint);
			if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
			{
				dClosestDistSqrd = dDistSqrd;
				iClosestIndex = i;
			}
		}
		return iClosestIndex;
	}

	/// Get the index of the point in a vector of 2D points which is within a given tolerance of a specified position
	/// Returns index, or -1 if no point found within tolerance
	inline int GetClosestPointWithinTol(const std::vector<XY> &Points, const XY Position, double dTol )
	{
		std::vector<XY>::const_iterator itPoint;
		double dClosestDistSqrd = -1;
		double dDistSqrd;
		int iClosestIndex = -1, i;
		for (itPoint = Points.begin(), i=0; itPoint != Points.end(); ++itPoint, ++i)
		{
			dDistSqrd = GetLengthSquared(Position, *itPoint);
			if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
			{
				dClosestDistSqrd = dDistSqrd;
				if ( dClosestDistSqrd <= dTol )
					iClosestIndex = i;
			}
		}
		return iClosestIndex;
	}

	/// Returns the closest point to the lines joining the vector of 2D Points from the specified point, Position
	/// Assumes that the points are ordered
	inline XY GetClosestPoint( const std::vector<XY> &Points, const XY Position)
	{
		int index = GetClosestPointIndex( Points, Position );
		int iSize = (int)Points.size();
		int iNextInd = (index+1)%iSize;
		int iPrevInd;
		XYZ Pos(Position.x, Position.y, 0);
		XYZ p1(Points[index].x, Points[index].y, 0);

		XYZ p2(Points[iNextInd].x, Points[iNextInd].y, 0);
	
		if ( index )
			iPrevInd = index-1;
		else
			iPrevInd = Points[0] == Points[iSize-1] ? iSize-2 : iSize-1;  // Check if closed loop
		XYZ p3(Points[iPrevInd].x, Points[iPrevInd].y, 0);

		double dDist1 = GetLength(Pos, p2);
		double dDist2 = GetLength(Pos, p3);
		XYZ comp = dDist1 < dDist2 ? p2 : p3;
		double dU = 0.0;
		
		XYZ pClosest = ShortestDistPointLine( Pos, p1, comp, dU );
		return XY(pClosest.x, pClosest.y);
	}

	/// Get the point in a vector of 3D points which is closest to the specified position
	/// Returns the index of the point
	inline int GetClosestPointIndex( const std::vector<XYZ> &Points, const XYZ Position)
	{
		std::vector<XYZ>::const_iterator itPoint;
		double dClosestDistSqrd = -1;
		double dDistSqrd;
		int iClosestIndex = -1, i;
		for (itPoint = Points.begin(), i=0; itPoint != Points.end(); ++itPoint, ++i)
		{
			dDistSqrd = GetLengthSquared(Position, *itPoint);
			if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
			{
				dClosestDistSqrd = dDistSqrd;
				iClosestIndex = i;
			}
		}
		return iClosestIndex;
	}

	/// Get the index of the point in a vector of 3D points which is within a given tolerance of a specified position
	/// Returns index, or -1 if no point found within tolerance
	inline int GetClosestPointWithinTol(const std::vector<XYZ> &Points, const XYZ Position, double dTol )
	{
		std::vector<XYZ>::const_iterator itPoint;
		double dClosestDistSqrd = -1;
		double dDistSqrd;
		int iClosestIndex = -1, i;
		for (itPoint = Points.begin(), i=0; itPoint != Points.end(); ++itPoint, ++i)
		{
			dDistSqrd = GetLengthSquared(Position, *itPoint);
			if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
			{
				dClosestDistSqrd = dDistSqrd;
				if ( dClosestDistSqrd <= dTol )
					iClosestIndex = i;
			}
		}
		return iClosestIndex;
	}

	/// Returns the closest point to the lines joining the vector of 3D Points from the specified point, Position
	/// Assumes that the points are ordered
	inline XYZ GetClosestPoint( const std::vector<XYZ> &Points, const XYZ Position)
	{
		int index = GetClosestPointIndex( Points, Position );
		int iSize = (int)Points.size();
		int iNextInd = (index+1)%iSize;
		int iPrevInd;
		
		XYZ p1(Points[index].x, Points[index].y, Points[index].z);

		XYZ p2(Points[iNextInd].x, Points[iNextInd].y, Points[iNextInd].z);
	
		if ( index )
			iPrevInd = index-1;
		else
			iPrevInd = Points[0] == Points[iSize-1] ? iSize-2 : iSize-1;  // Check if closed loop
		XYZ p3(Points[iPrevInd].x, Points[iPrevInd].y, Points[iPrevInd].z);

		double dDist1 = GetLength(Position, p2);
		double dDist2 = GetLength(Position, p3);
		XYZ comp = dDist1 < dDist2 ? p2 : p3;
		double dU = 0.0;
		
		return ShortestDistPointLine( Position, p1, comp, dU );
	}
};	// namespace TexGen












