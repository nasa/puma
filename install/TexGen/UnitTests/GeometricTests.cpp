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

#include "GeometricTests.h"
#include "../Core/MatrixUtils.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CGeometricTests);

void CGeometricTests::setUp()
{
}

void CGeometricTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CGeometricTests::TestLenticularSection()
{
	double dWidth = 2;
	double dHeight = 1;
	double dDistortion = 0.1;
	CSectionLenticular Section(dWidth, dHeight, dDistortion);
	XY Point;
	Point = Section.GetPoint(0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5*dWidth, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(dDistortion, Point.y, 1e-9);
	Point = Section.GetPoint(0.25);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5*dHeight, Point.y, 1e-9);
	Point = Section.GetPoint(0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5*dWidth, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(dDistortion, Point.y, 1e-9);
	Point = Section.GetPoint(0.75);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5*dHeight, Point.y, 1e-9);
}

void CGeometricTests::TestHybridQuarterSection()
{
	CSectionHybrid Section(CSectionEllipse(2,2), CSectionEllipse(1,2), CSectionEllipse(1,1), CSectionEllipse(2,1));

	XY Point;
	Point = Section.GetPoint(0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.25);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.75);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, Point.y, 1e-9);
}

void CGeometricTests::TestHybridHalfSection()
{
	CSectionHybrid Section(CSectionEllipse(2,2), CSectionEllipse(2,1));

	XY Point;
	Point = Section.GetPoint(0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.25);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.y, 1e-9);
	Point = Section.GetPoint(0.75);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, Point.x, 1e-9);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, Point.y, 1e-9);
}

void CGeometricTests::TestPointInsideYarn()
{
	// Build a single yarn of diameter 1, going from (0,0,0) to (1,0,0)
	// The domain is 1 x 1 x 1 with center (1,1,1)
	// Build a grid of points inside the domain and check that the analytical solution
	// agrees with the numerical one within a certain tolerance
	CTextile Textile = m_TextileFactory.GetSingleYarn(100, 40);
	CYarn &Yarn = *Textile.GetYarns().begin();
	const CDomain &Domain = *Textile.GetDomain();
	vector<XYZ> Translations = Domain.GetTranslations(Yarn);
	int i, j, k;
	int iGridSize = 10;
	XYZ Point;
	bool bResult;
	pair<XYZ, XYZ> DomainSize = Domain.GetMesh().GetAABB();
	double dDist;
	const double dTolerance = 0.001;
	double dSurfaceDist;
	for (i=0; i<iGridSize; ++i)
	{
		for (j=0; j<iGridSize; ++j)
		{
			for (k=0; k<iGridSize; ++k)
			{
				Point.x = (i+0.5)/iGridSize;
				Point.y = (j+0.5)/iGridSize;
				Point.z = (k+0.5)/iGridSize;

				Point *= DomainSize.second - DomainSize.first;
				Point += DomainSize.first;
				
				bResult = Yarn.PointInsideYarn(Point, Translations, NULL, NULL, NULL, &dSurfaceDist);
				dDist = GetDistanceFromEdge(Point);
				if (bResult)
				{
					CPPUNIT_ASSERT_DOUBLES_EQUAL(dDist, dSurfaceDist, 0.01);
					CPPUNIT_ASSERT(dDist < dTolerance);
				}
				else
				{
					CPPUNIT_ASSERT(dDist > -dTolerance);
				}
			}
		}
	}
}

double CGeometricTests::GetDistanceFromEdge(XYZ Point)
{
	while (Point.y>0.5)
		Point.y -= 1;
	while (Point.z>0.5)
		Point.z -= 1;
	while (Point.y<-0.5)
		Point.y += 1;
	while (Point.z<-0.5)
		Point.z += 1;
	Point.x = 0;
	return GetLength(Point)-0.5;
}

void CGeometricTests::TestQuaternionRotation()
{
	XYZ Vec(1, 0, 0);
	WXYZ Rot(PI/2, 0, 0);
	XYZ Expected(0, 1, 0);
	Vec = Rot * Vec;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.x, Vec.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.y, Vec.y, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected.z, Vec.z, 1e-6);
}

void CGeometricTests::TestConvertRotation()
{
	// Create a random vector and a random rotation
	XYZ Vec(RandomNumber(-1, 1), RandomNumber(-1, 1), RandomNumber(-1, 1));
	WXYZ RotQuat(RandomNumber(0, 2*PI), RandomNumber(0, 2*PI), RandomNumber(0, 2*PI));
	// Convert the rotation quaternion to a rotation matrix
	CMatrix RotMat = ConvertRotation(RotQuat);
	// Compare the results of rotating by quaternion and matrix
	XYZ VecQuat = RotQuat * Vec;
	XYZ VecMat = RotMat * Vec;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.x, VecMat.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.y, VecMat.y, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(VecQuat.z, VecMat.z, 1e-6);
}

void CGeometricTests::TestGetClosestPointFunctions()
{
	vector<XY> Points;
	int index = -1; 
	XY Expected(1,-1);
	double dTol = 1e-6;
	
	Points.push_back(XY(0,0));
	Points.push_back(Expected);
	Points.push_back(XY(2,0));
	Points.push_back(XY(1,1));

	XY TestPoint(1.2,-0.7);

	index = GetClosestPointIndex( Points, TestPoint);
	CPPUNIT_ASSERT_EQUAL(index, 1);

	// Point at index 2
	index = GetClosestPointWithinTol(Points, Expected, dTol );
	CPPUNIT_ASSERT_EQUAL(index,1);
	
	// Point not within tolerance of point in list, returns -1
	index = GetClosestPointWithinTol(Points, XY(1.01,-1.01), dTol );
	CPPUNIT_ASSERT_EQUAL(index,-1);

	XY pClosest = GetClosestPoint(Points, XY(0.75,-0.25) );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, pClosest.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, pClosest.y, 1e-6);
}

void CGeometricTests::Test3DGetClosestPointFunctions()
{
	vector<XYZ> Points;
	int index = -1; 
	XYZ Expected(1,-1, -1);
	double dTol = 1e-6;
	
	Points.push_back(XYZ(0,0,0));
	Points.push_back(Expected);
	Points.push_back(XYZ(2,0,0));
	Points.push_back(XYZ(1,1,1));

	XYZ TestPoint(1.2,-0.7,-0.8);

	index = GetClosestPointIndex( Points, TestPoint);
	CPPUNIT_ASSERT_EQUAL(index, 1);

	// Point at index 1
	index = GetClosestPointWithinTol(Points, Expected, dTol );
	CPPUNIT_ASSERT_EQUAL(index,1);
	
	// Point not within tolerance of point in list, returns -1
	index = GetClosestPointWithinTol(Points, XYZ(1.01,-1.01,-1.01), dTol );
	CPPUNIT_ASSERT_EQUAL(index,-1);

	XYZ pClosest = GetClosestPoint(Points, XYZ(0.75,-0.25, -0.5) );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, pClosest.x, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, pClosest.y, 1e-6);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, pClosest.z, 1e-6);
}

void CGeometricTests::TestFindClosestSurfacePoint()
{
	// Build a single yarn of diameter 1, going from (0,0,0) to (1,0,0)
	// The domain is 1 x 1 x 1 with center (1,1,1)
	// Build a grid of points inside the domain and check that the analytical solution
	// agrees with the numerical one within a certain tolerance
	CTextile Textile = m_TextileFactory.GetSingleYarn(2, 40);
	CYarn &Yarn = *Textile.GetYarns().begin();
	const CDomain &Domain = *Textile.GetDomain();
	vector<XYZ> Translations = Domain.GetTranslations(Yarn);
	int i, j, k;
	//double i,j,k;
	int iGridSize = 10;
	XYZ Point, TestPoint;
	bool bResult;
	pair<XYZ, XYZ> DomainSize = Domain.GetMesh().GetAABB();
	ifstream TestPoints("..\\..\\UnitTests\\ClosestPointsTest.txt");
	
	for (i=0; i<iGridSize; ++i)
	{
		for (j=0; j<iGridSize; ++j)
		{
			for (k=0; k<iGridSize; ++k )
			{
				Point.x = (i+0.5)/iGridSize;
				Point.y = (j+0.5)/iGridSize;
				Point.z = (k+0.5)/iGridSize;

				Point *= DomainSize.second - DomainSize.first;
				Point += DomainSize.first;
				XYZ ClosestPoint;
				double minDist = 0.0;
				
				bResult = Yarn.FindClosestSurfacePoint(Point, Translations, ClosestPoint, 40 );

				TestPoints >> TestPoint.x >> TestPoint.y >> TestPoint.z;
				CPPUNIT_ASSERT_DOUBLES_EQUAL(ClosestPoint.x, TestPoint.x, 1e-5);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(ClosestPoint.y, TestPoint.y, 1e-5);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(ClosestPoint.z, TestPoint.z, 1e-5);
			}
		}
	}
}

void CGeometricTests::TestRotateYarn()
{
	CTextile Textile = m_TextileFactory.GetSingleYarn(3, 20);
	CYarn* Yarn = Textile.GetYarn(0);
	vector<XYZ> Expected;
	Expected.push_back( XYZ(0,0,0) );
	Expected.push_back( XYZ(0,0.5,0) );
	Expected.push_back( XYZ(0,1,0) );

	Yarn->Rotate(WXYZ(XYZ(0,0,1), PI/2) );
	for ( int i=0; i < 3; ++i )
	{
		XYZ Point = Yarn->GetNode(i)->GetPosition();
		CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected[i].x, Point.x, 1e-5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected[i].y, Point.y, 1e-5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(Expected[i].z, Point.z, 1e-5);
	}
}







