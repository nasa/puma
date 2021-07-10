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


#include "Vector3f.hpp"

#include "OctreeImplementation.hpp"   /// own header is included last


using namespace hxa7241_graphics;




/// OctreeRoot /////////////////////////////////////////////////////////////////


/// standard object services ---------------------------------------------------
OctreeRoot::OctreeRoot
(
	const Vector3f& position,
	const float     sizeOfCube,
	const dword     maxItemsPerCell,
	const dword     maxLevelCount
)
 :	dimensions_m( position, sizeOfCube, maxItemsPerCell, maxLevelCount )
 ,	pRootCell_m ( 0 )
{
}


OctreeRoot::~OctreeRoot()
{
	delete pRootCell_m;
}


OctreeRoot::OctreeRoot
(
	const OctreeRoot& other
)
 :	dimensions_m( other.dimensions_m )
 ,	pRootCell_m ( OctreeCell::cloneNonZero( other.pRootCell_m ) )
{
}


OctreeRoot& OctreeRoot::operator=
(
	const OctreeRoot& other
)
{
	if( &other != this )
	{
		delete pRootCell_m;
		pRootCell_m = 0;
		pRootCell_m = OctreeCell::cloneNonZero( other.pRootCell_m );

		dimensions_m = other.dimensions_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
bool OctreeRoot::insertItem
(
	const void* const   pItem,
	const OctreeAgentV& agent
)
{
	bool isInserted = false;

	/// make data
	const OctreeData data( dimensions_m );

	/// check if item overlaps root cell
	if( agent.isOverlappingCellV( pItem, data.getBound().getLowerCorner(),
	                              data.getBound().getUpperCorner() ) )
	{
		OctreeLeaf::insertItemMaybeCreate( data, pRootCell_m, pItem, agent );

		isInserted = true;
	}

	return isInserted;
}


bool OctreeRoot::removeItem
(
	const void* const   pItem,
	const OctreeAgentV& //agent
)
{
	bool isRemoved = false;

	if( pRootCell_m != 0 )
	{
		dword unusedBranchItemCount = 0;
		isRemoved = pRootCell_m->removeItem( pRootCell_m, pItem,
			dimensions_m.getMaxItemCountPerCell(), unusedBranchItemCount );
	}

	return isRemoved;
}




/// queries --------------------------------------------------------------------
void OctreeRoot::visit
(
	OctreeVisitorV& visitor
) const
{
	/// make data
	const OctreeData data( dimensions_m );

	visitor.visitRootV( pRootCell_m, data );
}


bool OctreeRoot::isEmpty() const
{
	return pRootCell_m == 0;
}


void OctreeRoot::getInfo
(
	const dword rootWrapperByteSize,
	dword&      byteSize,
	dword&      leafCount,
	dword&      itemCount,
	dword&      maxDepth
) const
{
	byteSize  = 0;
	leafCount = 0;
	itemCount = 0;
	maxDepth  = 0;

	if( pRootCell_m != 0 )
	{
		pRootCell_m->getInfo( byteSize, leafCount, itemCount, maxDepth );
	}

	byteSize += rootWrapperByteSize;
}


const Vector3f& OctreeRoot::getPosition() const
{
	return dimensions_m.getPosition();
}


float OctreeRoot::getSize() const
{
	return dimensions_m.getSize();
}


dword OctreeRoot::getMaxItemCountPerCell() const
{
	return dimensions_m.getMaxItemCountPerCell();
}


dword OctreeRoot::getMaxLevelCount() const
{
	return dimensions_m.getMaxLevelCount();
}








/// OctreeCell /////////////////////////////////////////////////////////////////


/// statics --------------------------------------------------------------------
OctreeCell* OctreeCell::cloneNonZero
(
	const OctreeCell* pOriginal
)
{
	return (pOriginal != 0) ? pOriginal->clone() : 0;
}








/// OctreeBranch ///////////////////////////////////////////////////////////////


/// standard object services ---------------------------------------------------
OctreeBranch::OctreeBranch()
{
	OctreeBranch::zeroSubCells();
}


OctreeBranch::OctreeBranch
(
	const OctreeData&         thisData,
	const Array<const void*>& items,
	const void* const         pItem,
	const OctreeAgentV&       agent
)
{
	OctreeBranch::zeroSubCells();

	try
	{
		OctreeCell* pNotUsed = 0;

		/// insert items
		for( int j = items.getLength();  j-- > 0; )
		{
			OctreeBranch::insertItem( thisData, pNotUsed, items[j], agent );
		}

		OctreeBranch::insertItem( thisData, pNotUsed, pItem, agent );
	}
	catch( ... )
	{
		/// delete any allocated cells
		this->~OctreeBranch();

		throw;
	}
}


OctreeBranch::~OctreeBranch()
{
	for( int i = 8;  i-- > 0; )
	{
		delete subCells_m[i];
	}
}


OctreeBranch::OctreeBranch
(
	const OctreeBranch& other
)
 :	OctreeCell()
{
	OctreeBranch::zeroSubCells();

	try
	{
		for( int i = 8;  i-- > 0; )
		{
			subCells_m[i] = OctreeCell::cloneNonZero( other.subCells_m[i] );
		}
	}
	catch( ... )
	{
		/// delete any allocated cells
		this->~OctreeBranch();

		throw;
	}
}


OctreeBranch& OctreeBranch::operator=
(
	const OctreeBranch& other
)
{
	if( &other != this )
	{
		for( int i = 8;  i-- > 0; )
		{
			delete subCells_m[i];
			subCells_m[i] = 0;
			subCells_m[i] = OctreeCell::cloneNonZero( other.subCells_m[i] );
		}
	}

	return *this;
}




/// commands -------------------------------------------------------------------
void OctreeBranch::insertItem
(
	const OctreeData&   thisData,
	OctreeCell*&        ,//pThis,
	const void* const   pItem,
	const OctreeAgentV& agent
)
{
	/// get subcell-item overlaps flags
	const OctreeBound& bound    = thisData.getBound();
	const dword        overlaps = agent.getSubcellOverlapsV( pItem,
		bound.getLowerCorner(), bound.getCenter(), bound.getUpperCorner() );

	/// loop through sub cells
	for( int i = 8;  i-- > 0; )
	{
		/// check if sub cell is overlapped by item
		if( (overlaps >> i) & 1 )
		{
			/// make sub cell data
			const OctreeData subCellData( thisData, i );

			/// add item to sub cell
			OctreeLeaf::insertItemMaybeCreate( subCellData, subCells_m[i],
			                                   pItem, agent );
		}
	}
}


bool OctreeBranch::removeItem
(
	OctreeCell*&      pThis,
	const void* const pItem,
	const dword       maxItemsPerCell,
	dword&            itemCount
)
{
	bool  isRemoved       = false;
	dword branchItemCount = 0;

	/// loop through sub cells
	for( int i = 8;  i-- > 0; )
	{
		/// remove item from non-null sub cell
		OctreeCell*& pSubCell = subCells_m[i];
		if( pSubCell != 0 )
		{
			isRemoved |= pSubCell->removeItem( pSubCell, pItem, maxItemsPerCell,
			                                   branchItemCount );
		}
	}

	itemCount += branchItemCount;

	/// decide whether to collapse this branch
	if( branchItemCount > 0 )
	{
		/// collapse to leaf
		if( branchItemCount <= maxItemsPerCell )
		{
			/// all subcells *will* be leafs!
			/// because:
			/// a) if a branch has below it less item refs than the threshold,
			///    it collapses to a leaf (this function!)
			/// b) the total of item refs below this branch in the tree is less
			///    than the threshold
			/// c) therefore the total of item refs in any branch below this
			///    cell will be less than the threshold
			/// d) branchs below this cell will be collapsed before this branch
			///    (because the recursive 'removeItem' call is before the
			///    collapsing code)
			/// so: if this branch will collapse to a leaf, then all its sub
			/// branchs (direct and indirect) will collapse to leafs, and that
			/// will happen before this branch.
			OctreeCell*const pLeaf = new OctreeLeaf(
				reinterpret_cast<OctreeLeaf**>( subCells_m ) );

			delete pThis;
			pThis = pLeaf;
		}
	}
	else
	{
		/// delete
		delete pThis;
		pThis = 0;
	}

	return isRemoved;
}




/// queries --------------------------------------------------------------------
void OctreeBranch::visit
(
	const OctreeData& thisData,
	OctreeVisitorV&   visitor
) const
{
	visitor.visitBranchV( const_cast<const OctreeCell**>(subCells_m),
	                      thisData );
}


OctreeCell* OctreeBranch::clone() const
{
	return new OctreeBranch( *this );
}


void OctreeBranch::getInfo
(
	dword& byteSize,
	dword& leafCount,
	dword& itemCount,
	dword& maxDepth
) const
{
	byteSize += sizeof(*this);

	const dword thisDepth = maxDepth + 1;

	for( int i = 8;  i-- > 0; )
	{
		const OctreeCell*const pSubCell = subCells_m[i];
		if( pSubCell != 0 )
		{
			dword depth = thisDepth;
			pSubCell->getInfo( byteSize, leafCount, itemCount, depth );

			if( maxDepth < depth )
			{
				maxDepth = depth;
			}
		}
	}
}




/// implementation -------------------------------------------------------------
void OctreeBranch::zeroSubCells()
{
	for( int i = 8;  i-- > 0; )
	{
		subCells_m[i] = 0;
	}
}








/// OctreeLeaf /////////////////////////////////////////////////////////////////


/// standard object services ---------------------------------------------------
OctreeLeaf::OctreeLeaf()
 :	items_m()
{
}


OctreeLeaf::OctreeLeaf
(
	const void* pItem
)
 :	items_m()
{
	items_m.append( pItem );
}


OctreeLeaf::OctreeLeaf
(
	const OctreeLeaf*const leafs[8]
)
 :	items_m()
{
	/// sum all items lengths
	dword totalLength = 0;
	for( int i = 8;  i-- > 0; )
	{
		const OctreeLeaf*const pLeaf = leafs[i];
		if( 0 != pLeaf )
		{
			totalLength += pLeaf->items_m.getLength();
		}
	}

	/// prepare items array to hold all other items
	items_m.setLength( totalLength );

	/// copy items arrays
	const void** pElement = items_m.getMemory();
	for( int i = 0;  i < 8;  ++i )
	{
		const OctreeLeaf*const pLeaf = leafs[i];
		if( 0 != pLeaf )
		{
			const void** pOtherElement = pLeaf->items_m.getMemory();
			const void** pOtherEnd = pOtherElement + pLeaf->items_m.getLength();
			for( ;  pOtherElement < pOtherEnd;  ++pOtherElement, ++pElement )
			{
				*pElement = *pOtherElement;
			}
		}
	}
}


OctreeLeaf::~OctreeLeaf()
{
}


OctreeLeaf::OctreeLeaf
(
	const OctreeLeaf& other
)
 :	OctreeCell()
 ,	items_m( other.items_m )
{
}


OctreeLeaf& OctreeLeaf::operator=
(
	const OctreeLeaf& other
)
{
	items_m = other.items_m;

	return *this;
}




/// commands -------------------------------------------------------------------
void OctreeLeaf::insertItem
(
	const OctreeData&   thisData,
	OctreeCell*&        pThis,
	const void* const   pItem,
	const OctreeAgentV& agent
)
{
	/// check if item already present
	bool isAlreadyPresent = false;
	for( int i = items_m.getLength();  (i-- > 0) & !isAlreadyPresent; )
	{
		isAlreadyPresent |= (items_m[i] == pItem);
	}

	/// only insert if item not already present
	if( !isAlreadyPresent )
	{
		/// check if leaf should be subdivided
		if( !thisData.isSubdivide( items_m.getLength() + 1 ) )
		{
			/// append item to collection
			items_m.append( pItem );
		}
		else
		{
			/// subdivide by making branch and adding items to it
			OctreeCell*const pBranch = new OctreeBranch( thisData, items_m,
			                                             pItem, agent );

			/// replace this with branch
			delete pThis;
			pThis = pBranch;
		}
	}
}


bool OctreeLeaf::removeItem
(
	OctreeCell*&      pThis,
	const void* const pItem,
	const dword       ,//maxItemsPerCell,
	dword&            itemCount
)
{
	bool isRemoved = false;

	/// loop through items
	for( int i = 0;  i < items_m.getLength(); )
	{
		/// check if item is present
		if( items_m[i] == pItem )
		{
			/// remove item
			items_m.remove( i );
			isRemoved = true;
		}
		else
		{
			++i;
		}
	}

	itemCount += items_m.getLength();

	/// check if leaf is now empty
	if( items_m.isEmpty() )
	{
		/// remove this leaf
		delete pThis;
		pThis = 0;
	}

	return isRemoved;
}




/// queries --------------------------------------------------------------------
void OctreeLeaf::visit
(
	const OctreeData& thisData,
	OctreeVisitorV&   visitor
) const
{
	visitor.visitLeafV( items_m, thisData );
}


OctreeCell* OctreeLeaf::clone() const
{
	return new OctreeLeaf( *this );
}


void OctreeLeaf::getInfo
(
	dword& byteSize,
	dword& leafCount,
	dword& itemCount,
	dword& maxDepth
) const
{
	byteSize  += sizeof(*this) + (items_m.getLength() * sizeof(void*));
	++leafCount;
	itemCount += items_m.getLength();
	++maxDepth;
}




/// statics --------------------------------------------------------------------
void OctreeLeaf::insertItemMaybeCreate
(
	const OctreeData&   cellData,
	OctreeCell*&        pCell,
	const void* const   pItem,
	const OctreeAgentV& agent
)
{
	/// check cell exists
	if( 0 == pCell )
	{
		/// make leaf, adding item
		OctreeCell*const pLeaf = new OctreeLeaf( pItem );

		/// replace cell with leaf
		delete pCell;
		pCell = pLeaf;
	}
	else
	{
		/// forward to existing cell
		pCell->insertItem( cellData, pCell, pItem, agent );
	}
}
