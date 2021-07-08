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


#include <iostream>

#include "OctreeStreamOut.hpp"   /// own header is included last


using namespace hxa7241_graphics;




/// other ----------------------------------------------------------------------
static std::ostream& operator<<( std::ostream&, const Vector3f& );

std::ostream& operator<<
(
	std::ostream&   out,
	const Vector3f& v
)
{
	return out << "(" << v.getX() << ", "
	                  << v.getY() << ", "
	                  << v.getZ() << ")";
}




/// standard object services ---------------------------------------------------
OctreeStreamOutImplementation::OctreeStreamOutImplementation
(
	std::ostream& outStream,
	const bool    isLongFormat
)
 :	pOutStream_m  ( &outStream )
 ,	isLongFormat_m( isLongFormat )
 ,	pCurrentCell_m( 0 )
 ,	indent_m      ( 0 )
{
}


OctreeStreamOutImplementation::~OctreeStreamOutImplementation()
{
}


OctreeStreamOutImplementation::OctreeStreamOutImplementation
(
	const OctreeStreamOutImplementation& other
)
 :	pOutStream_m  ( other.pOutStream_m )
 ,	isLongFormat_m( other.isLongFormat_m )
 ,	pCurrentCell_m( 0 )
 ,	indent_m      ( 0 )
{
}


OctreeStreamOutImplementation& OctreeStreamOutImplementation::operator=
(
	const OctreeStreamOutImplementation& other
)
{
	if( &other != this )
	{
		pOutStream_m   = other.pOutStream_m;
		isLongFormat_m = other.isLongFormat_m;

		pCurrentCell_m = 0;
		indent_m       = 0;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void OctreeStreamOutImplementation::visitRoot
(
	const OctreeCell* pRootCell,
	const OctreeData& octreeData,
	OctreeVisitorV&   visitor
)
{
	dword byteSize  = 0;
	dword leafCount = 0;
	dword itemCount = 0;
	dword maxDepth  = 0;

	if( pRootCell != 0 )
	{
		pRootCell->getInfo( byteSize, leafCount, itemCount, maxDepth );
	}

	byteSize += sizeof(OctreeRoot);

	const OctreeDimensions& dimensions = octreeData.getDimensions();
	*pOutStream_m << "\nOctree " <<
	             "\nposition: "        << dimensions.getPosition()            <<
	             "  size: "            << dimensions.getSize()                <<
	             "  maxItemsPerCell: " << dimensions.getMaxItemCountPerCell() <<
	             "  maxLevelCount: "   << dimensions.getMaxLevelCount()       <<
	             "\n(bytes: "          << byteSize                            <<
	             ", leafs: "           << leafCount                           <<
	             ", items: "           << itemCount                           <<
	             ", depth: "           << maxDepth                            <<
	             ")\n{ ";

	if( pRootCell != 0 )
	{
		pCurrentCell_m = pRootCell;
		indent_m       = 1;

		/// continue visit traversal
		pRootCell->visit( octreeData, visitor );
	}
	else
	{
		*pOutStream_m << "empty ";
	}

	*pOutStream_m << "}\n";
}


void OctreeStreamOutImplementation::visitBranch
(
	const OctreeCell* subCells[8],
	const OctreeData& octreeData,
	OctreeVisitorV&   visitor
)
{
	writeDataAndInfo( *pCurrentCell_m, octreeData, *pOutStream_m );

	/// step through subcells
	for( int i = 0;  i < 8;  ++i )
	{
		*pOutStream_m << "\n";
		for( dword j = indent_m;  j-- > 0; )
		{
			*pOutStream_m << "\t";
		}
		*pOutStream_m << "[" << i << " ";

		const OctreeCell*const pSubCell = subCells[i];
		if( pSubCell != 0 )
		{
			pCurrentCell_m = pSubCell;
			indent_m      += 1;

			/// continue visit traversal
			const OctreeData subCellData( octreeData, i );
			pSubCell->visit( subCellData, visitor );

			indent_m -= 1;
		}
		else
		{
			*pOutStream_m << "empty ";
		}

		*pOutStream_m << "] ";
	}

	*pOutStream_m << "\n";
	for( dword i = indent_m - 1;  i-- > 0; )
	{
		*pOutStream_m << "\t";
	}
}


void OctreeStreamOutImplementation::visitLeafBefore
(
	const OctreeData& octreeData
)
{
	writeDataAndInfo( *pCurrentCell_m, octreeData, *pOutStream_m );

	indentNewline( 0 );

	if( isLongFormat_m )
	{
		*pOutStream_m << "<";
	}
}


void OctreeStreamOutImplementation::visitLeafAfter()
{
	if( isLongFormat_m )
	{
		indentNewline( 0 );
		*pOutStream_m << ">";
		indentNewline( -1 );
	}
}


bool OctreeStreamOutImplementation::isLongFormat() const
{
	return isLongFormat_m;
}


std::ostream& OctreeStreamOutImplementation::getOutStream() const
{
	return *pOutStream_m;
}


std::ostream& OctreeStreamOutImplementation::indentNewline
(
	const dword extraIndent
) const
{
	*pOutStream_m << "\n";
	for( dword i = indent_m + extraIndent;  i-- > 0; )
	{
		*pOutStream_m << "\t";
	}

	return *pOutStream_m;
}




/// implementation -------------------------------------------------------------

void OctreeStreamOutImplementation::writeDataAndInfo
(
	const OctreeCell& octreeCell,
	const OctreeData& octreeData,
	std::ostream&     outStream
)
{
	/// do octreeData
	outStream << "level(" <<
	              octreeData.getLevel()                  << ") lo"  <<
	              octreeData.getBound().getLowerCorner() << " hi"   <<
	              octreeData.getBound().getUpperCorner() << " ctr"  <<
	              octreeData.getBound().getCenter()      << " sph(" <<
	              octreeData.getBound().getRadius()      << ")  ";

	/// do info
	dword byteSize  = 0;
	dword leafCount = 0;
	dword itemCount = 0;
	dword depthLargest  = 0;
	octreeCell.getInfo( byteSize, leafCount, itemCount, depthLargest );

	outStream << "(bytes: "  << byteSize     <<
	             ", leafs: " << leafCount    <<
				 ", items: " << itemCount    <<
				 ", depth: " << depthLargest << ")  ";
}
