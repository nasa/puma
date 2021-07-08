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


#include "Octree.hpp"

#include <vector>
#include <iostream>


using namespace hxa7241_graphics;




/**
 * * step 1:<br/>
 * check your item class provides access to its 3D properties.<br/><br/>
 *
 * * step 2:<br/>
 * implement a derivative of OctreeAgent<YourItem>, to calculate if an item
 * overlaps a cell, or subcells.<br/><br/>
 *
 * * step 3:<br/>
 * implement derivatives of OctreeVisitor<YourItem>, to perform an operations
 * on an octree.<br/><br/>
 *
 * * step 4:<br/>
 * write code to make items, make octrees, make visitors, add/remove items
 * to octrees, execute operations on octrees.
 */


/**
 * file contents:<br/><br/>

 * classes<br/>
 *   * Block class: an example item<br/>
 *   * OctreeAgentBlock class: an agent for the example item<br/>
 *   * OctreeVisitorExample class: an example visitor using the item<br/>
 * functions<br/>
 *   * main function: code to make some items, make an octree, and use the
 *     visitor.<br/>
 */




/// Block //////////////////////////////////////////////////////////////////////

/**
 * a minimal axis-aligned 3D block: position and dimensions.<br/><br/>
 *
 * client-written item class.<br/><br/>
 *
 * this will have some primitive access to its 3D properties.
 */
class Block
{
/// standard object services ---------------------------------------------------
public:
	         Block();
	         Block( const Vector3f& position,
	                const Vector3f& dimension );

	        ~Block();
	         Block( const Block& );
	Block& operator=( const Block& );


/// queries --------------------------------------------------------------------
	        const Vector3f& getPosition()                                 const;
	        const Vector3f& getDimensions()                               const;


/// fields ---------------------------------------------------------------------
private:
	Vector3f position_m;
	Vector3f dimensions_m;
};




/// standard object services ---------------------------------------------------
Block::Block()
 :	position_m  ()
 ,	dimensions_m()
{
}


Block::Block
(
	const Vector3f& position,
	const Vector3f& dimensions
)
 :	position_m  ( position )
 ,	dimensions_m( dimensions )
{
}


Block::~Block()
{
}


Block::Block
(
	const Block& other
)
 :	position_m  ( other.position_m )
 ,	dimensions_m( other.dimensions_m )
{
}


Block& Block::operator=
(
	const Block& other
)
{
	if( &other != this )
	{
		position_m   = other.position_m;
		dimensions_m = other.dimensions_m;
	}

	return *this;
}


/// queries --------------------------------------------------------------------
const Vector3f& Block::getPosition() const
{
	return position_m;
}


const Vector3f& Block::getDimensions() const
{
	return dimensions_m;
}








/// OctreeAgentBlock ///////////////////////////////////////////////////////////

/**
 * an example Octree agent, for Block item class.<br/><br/>
 *
 * client must write a concrete derivative of OctreeAgent<> like this; in order
 * to provide the Octree a way to interact with the client's item class.
 */
class OctreeAgentBlock
	: public OctreeAgent<Block>
{
/// standard object services ---------------------------------------------------
public:
	         OctreeAgentBlock() {};

	virtual ~OctreeAgentBlock() {};
private:
	         OctreeAgentBlock( const OctreeAgentBlock& );
	OctreeAgentBlock& operator=( const OctreeAgentBlock& );


/// queries --------------------------------------------------------------------
/// octree agent overrides
protected:
	virtual bool  isOverlappingCell ( const Block&    item,
	                                  const Vector3f& lowerCorner,
	                                  const Vector3f& upperCorner )       const;
	virtual dword getSubcellOverlaps( const Block&    item,
	                                  const Vector3f& lower,
	                                  const Vector3f& middle,
	                                  const Vector3f& upper )             const;
};




/// queries --------------------------------------------------------------------
bool OctreeAgentBlock::isOverlappingCell
(
	const Block&    item,
	const Vector3f& lowerCorner,
	const Vector3f& upperCorner
) const
{
	const Vector3f& itemLower( item.getPosition() );
	const Vector3f  itemUpper = item.getPosition() + item.getDimensions();

	/// check the two ranges overlap in every dimension
	return Vector3f::ONE() == (itemLower < upperCorner) &&
	       Vector3f::ONE() == (itemUpper > lowerCorner);
}


dword OctreeAgentBlock::getSubcellOverlaps
(
	const Block&    item,
	const Vector3f& cellsLowerPos,
	const Vector3f& cellsMiddlePos,
	const Vector3f& cellsUpperPos
) const
{
	/// efficiency could (probably) be improved by doing minimal necessary checks
	/// against the dividing bounds, instead of repeatedly delegating to
	/// isOverlappingCell().

	dword flags = 0;

	const Vector3f* lowMidPoints[]  = { &cellsLowerPos, &cellsMiddlePos };
	const Vector3f* midHighPoints[] = { &cellsMiddlePos, &cellsUpperPos };

	for( dword i = 8;  i-- > 0; )
	{
		Vector3f lowerCorner( lowMidPoints[ i       & 1]->getX(),
		                      lowMidPoints[(i >> 1) & 1]->getY(),
		                      lowMidPoints[(i >> 2) & 1]->getZ() );
		Vector3f upperCorner( midHighPoints[ i       & 1]->getX(),
		                      midHighPoints[(i >> 1) & 1]->getY(),
		                      midHighPoints[(i >> 2) & 1]->getZ() );
		flags |= dword(isOverlappingCell( item, lowerCorner, upperCorner )) << i;
	}

	return flags;
}








/// OctreeVisitorExample ///////////////////////////////////////////////////////

/**
 * an example Octree visitor for writing out leaf content.<br/><br/>
 *
 * client must write a concrete derivative of OctreeVisitor<> like this; to
 * define an operation on the client's items through octree.
 */
class OctreeVisitorExample
	: public OctreeVisitor<Block>
{
/// standard object services ---------------------------------------------------
public:
	         OctreeVisitorExample();

	virtual ~OctreeVisitorExample();
private:
	         OctreeVisitorExample( const OctreeVisitorExample& );
	OctreeVisitorExample& operator=( const OctreeVisitorExample& );


/// commands -------------------------------------------------------------------
/// octree visitor overrides
protected:
	virtual void  visitRoot  ( const OctreeCell* pRootCell,
	                           const OctreeData& octreeData );
	virtual void  visitBranch( const OctreeCell* subCells[8],
	                           const OctreeData& octreeData );
	virtual void  visitLeaf  ( const Array<const Block*>& items,
	                           const OctreeData& octreeData );

	/// any other commands ...


/// queries --------------------------------------------------------------------
	/// any queries ...


/// fields ---------------------------------------------------------------------
private:
	/// any fields ...
};




/// standard object services ---------------------------------------------------
OctreeVisitorExample::OctreeVisitorExample()
{
}


OctreeVisitorExample::~OctreeVisitorExample()
{
}


/// commands -------------------------------------------------------------------
void OctreeVisitorExample::visitRoot
(
	const OctreeCell* pRootCell,
	const OctreeData& octreeData
)
{
	if( pRootCell != 0 )
	{
		pRootCell->visit( octreeData, *this );
	}
}


void OctreeVisitorExample::visitBranch
(
	const OctreeCell* subCells[8],
	const OctreeData& octreeData
)
{
	/// step through subcells (can be in any order)
	/// subcell numbering:
	///    y z       6 7
	///    |/   2 3  4 5
	///     -x  0 1
	///
	/// (in binary:)
	///    y z           110 111
	///    |/   010 011  100 101
	///     -x  000 001
	///
	for( dword i = 0;  i < 8;  ++i )
	{
		const OctreeCell* pSubCell = subCells[i];
		/// avoid null subcells
		if( pSubCell != 0 )
		{
			/// make subcell OctreeData: using octreeData argument and subcell
			/// index
			const OctreeData subCellData( octreeData, i );
			/// continue visit traversal (can choose not to)
			pSubCell->visit( subCellData, *this );
		}
	}
}


void OctreeVisitorExample::visitLeaf
(
	const Array<const Block*>& items,
	const OctreeData&          octreeData
)
{
	/// make some short aliases
	const Vector3f& lower( octreeData.getBound().getLowerCorner() );
	const Vector3f& upper( octreeData.getBound().getUpperCorner() );

	/// write leaf bound
	std::cout << "leaf: level(" << octreeData.getLevel() << ") ";
	std::cout << "lower(" << lower.getX() << ", " <<
	                         lower.getY() << ", " <<
	                         lower.getZ() << ") ";
	std::cout << "upper(" << upper.getX() << ", " <<
	                         upper.getY() << ", " <<
	                         upper.getZ() << ")\n";

	/// write items
	for( dword i = 0, end = items.getLength();  i < end;  ++i )
	{
		/// make some short aliases
		const Vector3f& lower( items[i]->getPosition() );
		const Vector3f  upper( items[i]->getPosition() + items[i]->getDimensions() );

		/// write item
		std::cout << "   item: ";
		std::cout << "lower(" << lower.getX() <<
		                 ", " << lower.getY() <<
		                 ", " << lower.getZ() << ") ";
		std::cout << "upper(" << upper.getX() <<
		                 ", " << upper.getY() <<
		                 ", " << upper.getZ() << ")\n";
	}

	std::cout << "\n";
}








/// main ///////////////////////////////////////////////////////////////////////

int main
(
	int    ,//argc,
	char*[] //argv[]
)
{
	/// make some items
	std::vector<Block> blocks;
	for( int i = 0;  i < 8;  ++i )
	{
		/// one in the middle of each octant
		blocks.push_back( Block( Vector3f( float(i & 1)        + 0.25f,
		                                   float((i >> 1) & 1) + 0.25f,
		                                   float((i >> 2) & 1) + 0.25f ),
		                         Vector3f(0.5f, 0.5f, 0.5f) ) );
	}

	/// make an octree:
	/// position (0,0,0), size 2, max items per leaf 4, max depth 4
	Octree<Block> octree( Vector3f::ZERO(), 2.0f, 4, 4 );


	/// make an agent (probably a singleton)
	OctreeAgentBlock agent;

	/// add items to octree, using agent
	for( int i = 0, end = blocks.size();  i < end;  ++i )
	{
		octree.insertItem( blocks[i], agent );
	}


	/// make a visitor (as many concrete variants, and instances of those as
	/// wanted)
	OctreeVisitorExample visitor;

	/// execute visitor
	octree.visit( visitor );


	return 0;
}
