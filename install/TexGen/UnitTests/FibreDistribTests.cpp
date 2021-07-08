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

#include "FibreDistribTests.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CFibreDistribTests);

void CFibreDistribTests::setUp()
{
}

void CFibreDistribTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CFibreDistribTests::TestInside()
{
	CSectionEllipse Ellipse(2, 2);
	CPPUNIT_ASSERT(PointInsideSection(Ellipse.GetPoints(100), XY(0,0)));
}

void CFibreDistribTests::TestOutside()
{
	CSectionEllipse Ellipse(2, 2);
	CPPUNIT_ASSERT(!PointInsideSection(Ellipse.GetPoints(100), XY(1,1)));
}

void CFibreDistribTests::TestConst()
{
	CFibreDistributionConst FibreDist;
	CSectionEllipse Ellipse(4, 1);
	double dAverageVF = GetAverageVF(FibreDist, Ellipse);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0/PI, dAverageVF, 1e-3);
}

void CFibreDistribTests::TestQuad()
{
	CFibreDistribution1DQuad FibreDist(0.5);
	CSectionEllipse Ellipse(4, 1);
	double dAverageVF = GetAverageVF(FibreDist, Ellipse);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0/PI, dAverageVF, 1e-3);
}

double CFibreDistribTests::GetAverageVF(CFibreDistribution &FibreDistrib, CSection &Section)
{
	int i, j;
	int iSizeX = 100, iSizeY = 100;
	const vector<XY> &Points = Section.GetPoints(100);
	XY Min, Max;
	GetBoundingBox(Points, Min, Max);
	XY Point;
	double dVolFrac = 0;
	int iCount = 0;
	for (i=0; i<iSizeX; ++i)
	{
		Point.x = Min.x + (Max.x-Min.x) * i / double(iSizeX-1);
		for (j=0; j<iSizeY; ++j)
		{
			Point.y = Min.y + (Max.y-Min.y) * j / double(iSizeY-1);
			if (PointInsideSection(Points, Point))
			{
				dVolFrac += FibreDistrib.GetVolumeFraction(Points, 1.0, Point);
				++iCount;
			}
		}
	}
	if (iCount > 0)
		return dVolFrac/iCount;
	else
		return -1;
}

bool CFibreDistribTests::PointInsideSection(const vector<XY> &Points, XY Point)
{
	int i;
	for (i=0; i<(int)Points.size(); ++i)
	{
		XY A, B;
		A = Points[(i+1)%Points.size()] - Points[i];
		B = Point - Points[i];
		if ((A.x*B.y - A.y*B.x) < 0)
			return false;
	}
	return true;
}

void CFibreDistribTests::GetBoundingBox(const vector<XY> &Points, XY &Min, XY &Max)
{
	Min = Max = XY();
	int i;
	for (i=0; i<(int)Points.size(); ++i)
	{
		Min = ::Min(Points[i], Min);
		Max = ::Max(Points[i], Max);
	}
}













