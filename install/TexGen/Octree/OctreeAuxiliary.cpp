/*--------------------------------------------------------------------

   Octree Component, version 2.0
   Copyright (c) 2004-2005,  Harrison Ainsworth / HXA7241.

   http://www.hxa7241.org/

--------------------------------------------------------------------*/

/*--------------------------------------------------------------------

Copyright (c) 2004-2005, Harrison Ainsworth / HXA7241.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice(s) and this permission notice appear in all copies of
the Software and that both the above copyright notice(s) and this
permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY
SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder
shall not be used in advertising or otherwise to promote the sale, use
or other dealings in this Software without prior written authorization
of the copyright holder.

--------------------------------------------------------------------*/


#include "OctreeAuxiliary.hpp"   /// own header is included last


using namespace hxa7241_graphics;




/// OctreeDimensions ///////////////////////////////////////////////////////////


/// to fit within fp single precision
const dword OctreeDimensions::MAX_LEVEL = 23;


/// standard object services ---------------------------------------------------
OctreeDimensions::OctreeDimensions
(
	const Vector3f& positionOfLowerCorner,
	const float     size,
	const dword     maxItemsPerCell,
	const dword     maxLevelCount
)
 :	positionOfLowerCorner_m( positionOfLowerCorner )
 ,	size_m                 ( size            >= 0.0f ? size          : -size )
 ,	maxItemsPerCell_m      ( maxItemsPerCell >  0    ? maxItemsPerCell   : 1 )
 ,	maxLevel_m             ( maxLevelCount   >  0    ? maxLevelCount - 1 : 0 )
{
	if( maxLevel_m > MAX_LEVEL )
	{
		maxLevel_m = MAX_LEVEL;
	}
}


OctreeDimensions::~OctreeDimensions()
{
}


OctreeDimensions::OctreeDimensions
(
	const OctreeDimensions& other
)
// :	positionOfLowerCorner_m( other.positionOfLowerCorner_m )
// ,	size_m                 ( other.size_m )
// ,	maxItemsPerCell_m      ( other.maxItemsPerCell_m )
// ,	maxLevel_m             ( other.maxLevel_m )
{
	OctreeDimensions::operator=( other );
}


OctreeDimensions& OctreeDimensions::operator=
(
	const OctreeDimensions& other
)
{
	if( &other != this )
	{
		positionOfLowerCorner_m = other.positionOfLowerCorner_m;
		size_m                  = other.size_m;
		maxItemsPerCell_m       = other.maxItemsPerCell_m;
		maxLevel_m              = other.maxLevel_m;
	}

	return *this;
}




/// queries --------------------------------------------------------------------
bool OctreeDimensions::isSubdivide
(
	const dword itemCount,
	const dword level
) const
{
	return (itemCount > maxItemsPerCell_m) & (level < maxLevel_m);
}








/// OctreeBound ////////////////////////////////////////////////////////////////


/// standard object services ---------------------------------------------------
OctreeBound::OctreeBound()
 :	positionOfLowerCorner_m( Vector3f::ZERO() )
 ,	positionOfUpperCorner_m( Vector3f::ONE() )
 ,	center_m               ( Vector3f::HALF() )
 ,	circumSphereRadius_m   ( Vector3f::HALF().length() )
{
}


OctreeBound::OctreeBound
(
	const Vector3f& positionOfLowerCorner,
	const float     size
)
 :	positionOfLowerCorner_m( positionOfLowerCorner )
 ,	positionOfUpperCorner_m( positionOfLowerCorner +
                             Vector3f(size, size, size) )
 ,	center_m               ( (positionOfLowerCorner_m + positionOfUpperCorner_m)
                             *= 0.5f )
 ,	circumSphereRadius_m   ( (Vector3f::HALF() * size).length() )
{
}


OctreeBound::OctreeBound
(
	const OctreeBound& parentCellBound,
	const dword        subCellIndex
)
{
	{
		const Vector3f* lowMidHigh[] =
		{
			&(parentCellBound.positionOfLowerCorner_m),
			&(parentCellBound.center_m),
			&(parentCellBound.positionOfUpperCorner_m)
		};

		positionOfLowerCorner_m.setXYZ(
			lowMidHigh[ subCellIndex       & 1]->getX(),
		    lowMidHigh[(subCellIndex >> 1) & 1]->getY(),
		    lowMidHigh[(subCellIndex >> 2) & 1]->getZ() );
		positionOfUpperCorner_m.setXYZ(
			(lowMidHigh+1)[ subCellIndex       & 1]->getX(),
		    (lowMidHigh+1)[(subCellIndex >> 1) & 1]->getY(),
		    (lowMidHigh+1)[(subCellIndex >> 2) & 1]->getZ() );
	}

	((center_m = positionOfLowerCorner_m) += positionOfUpperCorner_m) *= 0.5f;
	circumSphereRadius_m = parentCellBound.circumSphereRadius_m * 0.5f;
}


OctreeBound::~OctreeBound()
{
}


OctreeBound::OctreeBound
(
	const OctreeBound& other
)
 :	positionOfLowerCorner_m( other.positionOfLowerCorner_m ),
	positionOfUpperCorner_m( other.positionOfUpperCorner_m ),
	center_m               ( other.center_m ),
	circumSphereRadius_m   ( other.circumSphereRadius_m )
{
}


OctreeBound& OctreeBound::operator=
(
	const OctreeBound& other
)
{
	if( &other != this )
	{
		positionOfLowerCorner_m = other.positionOfLowerCorner_m;
		positionOfUpperCorner_m = other.positionOfUpperCorner_m;
		center_m                = other.center_m;
		circumSphereRadius_m    = other.circumSphereRadius_m;
	}

	return *this;
}








/// OctreeData /////////////////////////////////////////////////////////////////


/// standard object services ---------------------------------------------------
OctreeData::OctreeData
(
	const OctreeDimensions& dimensions
)
 :	bound_m      ( dimensions.getPosition(), dimensions.getSize() )
 ,	level_m      ( 0 )
 ,	pDimensions_m( &dimensions )
{
}


OctreeData::OctreeData
(
	const OctreeData& parentCellData,
	const dword       subCellIndex
)
 :	bound_m      ( parentCellData.bound_m, subCellIndex )
 ,	level_m      ( parentCellData.level_m + 1 )
 ,	pDimensions_m( parentCellData.pDimensions_m )
{
}


OctreeData::OctreeData
(
	const OctreeData&       other,
	const OctreeDimensions& dimensions
)
 :	bound_m      ( other.bound_m )
 ,	level_m      ( other.level_m )
 ,	pDimensions_m( &dimensions )
{
}


OctreeData::~OctreeData()
{
}


OctreeData::OctreeData
(
	const OctreeData& other
)
 :	bound_m      ( other.bound_m )
 ,	level_m      ( other.level_m )
 ,	pDimensions_m( other.pDimensions_m )
{
}


OctreeData& OctreeData::operator=
(
	const OctreeData& other
)
{
	if( &other != this )
	{
		bound_m       = other.bound_m;
		level_m       = other.level_m;
		pDimensions_m = other.pDimensions_m;
	}

	return *this;
}
