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
#include "OctreeStreamOut.hpp"
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <memory>
#include <iostream>
#include <sstream>
#include <time.h>

#include "OctreeTest.hpp"   /// own header is included last


using namespace hxa7241_graphics;




std::ostream& operator<<( std::ostream&, const Vector3f& );

std::ostream& operator<<
(
	std::ostream&   out,
	const Vector3f& v3f
)
{
	out << "("  << v3f.getX() <<
	       ", " << v3f.getY() <<
	       ", " << v3f.getZ() << ")";

	return out;
}




/// OctreeItemTest /////////////////////////////////////////////////////////////

class OctreeItemTest
{
/// standard object services ---------------------------------------------------
public:
	         OctreeItemTest( const Vector3f& position  = Vector3f::ZERO(),
	                         const Vector3f& dimension = Vector3f::ZERO(),
	                         dword           payload   = 0 );

	virtual ~OctreeItemTest();
	         OctreeItemTest( const OctreeItemTest& );
	OctreeItemTest& operator=( const OctreeItemTest& );


/// queries --------------------------------------------------------------------
	virtual const Vector3f& getPosition()                                 const;
	virtual const Vector3f& getDimensions()                               const;

	virtual dword getPayload()                                            const;


/// fields ---------------------------------------------------------------------
private:
	Vector3f position_m;
	Vector3f dimensions_m;

	dword    payload_m;
};




/// standard object services ---------------------------------------------------
OctreeItemTest::OctreeItemTest
(
	const Vector3f& position,
	const Vector3f& dimensions,
	const dword     payload
)
 :	position_m  ( position )
 ,	dimensions_m( dimensions )
 ,	payload_m   ( payload )
{
}


OctreeItemTest::~OctreeItemTest()
{
}


OctreeItemTest::OctreeItemTest
(
	const OctreeItemTest& other
)
 :	position_m  ( other.position_m )
 ,	dimensions_m( other.dimensions_m )
 ,	payload_m   ( other.payload_m )
{
}


OctreeItemTest& OctreeItemTest::operator=
(
	const OctreeItemTest& other
)
{
	if( &other != this )
	{
		position_m   = other.position_m;
		dimensions_m = other.dimensions_m;
		payload_m    = other.payload_m;
	}

	return *this;
}


/// queries --------------------------------------------------------------------
const Vector3f& OctreeItemTest::getPosition() const
{
	return position_m;
}


const Vector3f& OctreeItemTest::getDimensions() const
{
	return dimensions_m;
}


dword OctreeItemTest::getPayload() const
{
	return payload_m;
}


std::ostream& operator<<( std::ostream&, const OctreeItemTest& );

std::ostream& operator<<
(
	std::ostream&         out,
	const OctreeItemTest& item
)
{
	const Vector3f& position( item.getPosition() );
	const Vector3f& dimensions( item.getDimensions() );

	out << "{" << position << " " << dimensions << "}";

	return out;
}




/// OctreeAgentTest ////////////////////////////////////////////////////////////

class OctreeAgentTest
	: public OctreeAgent<OctreeItemTest>
{
/// standard object services ---------------------------------------------------
public:
	         OctreeAgentTest() {};

	virtual ~OctreeAgentTest() {};
private:
	         OctreeAgentTest( const OctreeAgentTest& );
	OctreeAgentTest& operator=( const OctreeAgentTest& );


/// queries --------------------------------------------------------------------
/// octree agent overrides
protected:
	virtual bool  isOverlappingCell ( const OctreeItemTest& item,
	                                  const Vector3f& lowerCorner,
	                                  const Vector3f& upperCorner )       const;
	virtual dword getSubcellOverlaps( const OctreeItemTest& item,
	                                  const Vector3f& lower,
	                                  const Vector3f& middle,
	                                  const Vector3f& upper )             const;


/// implementation -------------------------------------------------------------
public:
	static  bool isOverlapping( const Vector3f& itemLower,
	                            const Vector3f& itemUpper,
	                            const Vector3f& cellLower,
	                            const Vector3f& cellUpper );
};




/// queries --------------------------------------------------------------------
/// octree agent overrides
bool OctreeAgentTest::isOverlappingCell
(
	const OctreeItemTest& item,
	const Vector3f&       lowerCorner,
	const Vector3f&       upperCorner
) const
{
	return isOverlapping( item.getPosition(),
	                      item.getPosition() + item.getDimensions(),
		                  lowerCorner,
		                  upperCorner );
}


dword OctreeAgentTest::getSubcellOverlaps
(
	const OctreeItemTest& item,
	const Vector3f&       lower,
	const Vector3f&       middle,
	const Vector3f&       upper
) const
{
	dword flags = 0;

	const Vector3f* lowMidPoints[]  = { &lower, &middle };
	const Vector3f* midHighPoints[] = { &middle, &upper };

	for( dword i = 8;  i-- > 0; )
	{
		const Vector3f lowerCorner( lowMidPoints[ i       & 1]->getX(),
		                            lowMidPoints[(i >> 1) & 1]->getY(),
		                            lowMidPoints[(i >> 2) & 1]->getZ() );
		const Vector3f upperCorner( midHighPoints[ i       & 1]->getX(),
		                            midHighPoints[(i >> 1) & 1]->getY(),
		                            midHighPoints[(i >> 2) & 1]->getZ() );
		flags |= (dword(isOverlappingCell( item, lowerCorner, upperCorner ))
		          << i);
	}

	return flags;
}


bool OctreeAgentTest::isOverlapping
(
	const Vector3f& itemLower,
	const Vector3f& itemUpper,
	const Vector3f& cellLower,
	const Vector3f& cellUpper
)
{
	/// check the two ranges overlap in every dimension
	return Vector3f::ONE() == (itemLower < cellUpper) &&
	       Vector3f::ONE() == (itemUpper > cellLower);
}




/// OctreeVisitorTest //////////////////////////////////////////////////////////

class OctreeVisitorTest
	: public OctreeVisitor<OctreeItemTest>
{
/// standard object services ---------------------------------------------------
public:
	         OctreeVisitorTest( const Octree<OctreeItemTest>& );

	virtual ~OctreeVisitorTest();
private:
	         OctreeVisitorTest( const OctreeVisitorTest& );
	OctreeVisitorTest& operator=( const OctreeVisitorTest& );


/// commands -------------------------------------------------------------------
protected:
	virtual void  visitRoot  ( const OctreeCell* pRootCell,
	                           const OctreeData& octreeData );
	virtual void  visitBranch( const OctreeCell* subCells[8],
	                           const OctreeData& octreeData );
	virtual void  visitLeaf  ( const Array<const OctreeItemTest*>& items,
	                           const OctreeData& octreeData );


/// queries --------------------------------------------------------------------
public:
	typedef std::pair<OctreeData, Array<const OctreeItemTest*> > LeafData;

	const std::vector<const void*>&
	    getIds()                                                          const;
	const std::vector<LeafData>&
	    getLeafs()                                                        const;


/// fields ---------------------------------------------------------------------
private:
	std::vector<const void*> ids_m;
	std::vector<LeafData>    leafs_m;
};




/// standard object services ---------------------------------------------------
OctreeVisitorTest::OctreeVisitorTest
(
	const Octree<OctreeItemTest>& octree
)
 :	ids_m  ()
 ,	leafs_m()
{
	ids_m.push_back( static_cast<const void*>(&octree) );
}


OctreeVisitorTest::~OctreeVisitorTest()
{
}


OctreeVisitorTest::OctreeVisitorTest
(
	const OctreeVisitorTest& other
)
 :	OctreeVisitor<OctreeItemTest>()
 ,	ids_m  ( other.ids_m )
 ,	leafs_m( other.leafs_m )
{
}


OctreeVisitorTest& OctreeVisitorTest::operator=
(
	const OctreeVisitorTest& other
)
{
	if( &other != this )
	{
		ids_m   = other.ids_m;
		leafs_m = other.leafs_m;
	}

	return *this;
}


/// commands -------------------------------------------------------------------
void OctreeVisitorTest::visitRoot
(
	const OctreeCell* pRootCell,
	const OctreeData& octreeData
)
{
	if( pRootCell != 0 )
	{
		ids_m.push_back( static_cast<const void*>(pRootCell) );

		pRootCell->visit( octreeData, *this );
	}
}


void OctreeVisitorTest::visitBranch
(
	const OctreeCell* subCells[8],
	const OctreeData& octreeData
)
{
	ids_m.push_back( subCells );

	/// step through subcells
	for( dword i = 8;  i-- > 0; )
	{
		const OctreeCell* pSubCell = subCells[i];
		if( pSubCell != 0 )
		{
			ids_m.push_back( static_cast<const void*>(pSubCell) );

			/// continue visit traversal
			const OctreeData subCellData( octreeData, i );
			pSubCell->visit( subCellData, *this );
		}
	}
}


void OctreeVisitorTest::visitLeaf
(
	const Array<const OctreeItemTest*>& items,
	const OctreeData& octreeData
)
{
	ids_m.push_back( static_cast<const void*>(&items) );

	leafs_m.push_back( LeafData( octreeData, items ) );
}


/// queries --------------------------------------------------------------------
const std::vector<const void*>&
OctreeVisitorTest::getIds() const
{
	return ids_m;
}


const std::vector<OctreeVisitorTest::LeafData>&
OctreeVisitorTest::getLeafs() const
{
	return leafs_m;
}








/// declarations ///////////////////////////////////////////////////////////////

static bool testConstruction
(
	std::ostream* out       = 0,
	const bool    isVerbose = false,
	const dword   seed      = 0
);
static bool testCommands
(
	std::ostream* out       = 0,
	const bool    isVerbose = false,
	const dword   seed      = 0
);
static bool testCommands1
(
	std::ostream* out       = 0,
	const bool    isVerbose = false,
	const dword   seed      = 0
);
static bool testCommands2
(
	std::ostream* out       = 0,
	const bool    isVerbose = false,
	const dword   seed      = 0
);
static bool testCommands3
(
	std::ostream* out       = 0,
	const bool    isVerbose = false,
	const dword   seed      = 0
);


class RandomFast
{
public:
/// standard object services ---------------------------------------------------
	explicit RandomFast( const dword seed =0 )
	 :	random_m( seed )
	{
	}

	~RandomFast()
	{
	}

	RandomFast( const RandomFast& other )
	 :	random_m( other.random_m )
	{
	}

	RandomFast& operator=( const RandomFast& other )
	{
		random_m = other.random_m;

		return *this;
	}


/// commands -------------------------------------------------------------------
	RandomFast& setSeed( const dword seed )
	{
		random_m = seed;

		return *this;
	}

	RandomFast& next()
	{
		random_m = dword(1664525) * random_m + dword(1013904223);

		return *this;
	}


/// queries --------------------------------------------------------------------
	dword getDword()                                                       const
	{
		return random_m;
	}

	udword getUdword()                                                     const
	{
		return udword(random_m);
	}

	float getFloat()                                                       const
	{
		dword itemp = dword(0x3F800000) | (dword(0x007FFFFF) & random_m);
		return *(reinterpret_cast<float*>(&itemp)) - 1.0f;
	}

	float getFloat( const float scale,
	                const float displace = 0.0f )                          const
	{
		return getFloat() * scale + displace;
	}


/// fields ---------------------------------------------------------------------
private:
	/// current value of sequence and seed of the following part of the sequence
	dword random_m;
};


static void makeRandomFilledOctree
(
	RandomFast&                             rand,
	const dword                             howManyItems,
	std::auto_ptr<Octree<OctreeItemTest> >& pOctree,
	std::vector<OctreeItemTest>&            items
);
static void makeRandomOctree
(
	RandomFast&                             rand,
	std::auto_ptr<Octree<OctreeItemTest> >& pOctree
);
static void makeRandomItems
(
	RandomFast&                  rand,
	dword                        howMany,
	const Vector3f&              position,
	float                        size,
	std::vector<OctreeItemTest>& items
);








/// test functions /////////////////////////////////////////////////////////////

void hxa7241_graphics::preTest()
{
	/// make items
	OctreeItemTest item1( Vector3f( 1, 1, 1 ) );
	OctreeItemTest item2( Vector3f( 3, 3, 3 ) );
	OctreeItemTest item3( Vector3f( 4, 4, 4 ) );

	/// make octree
	Vector3f position( 0 );
	float    size      = 5.0f;
	dword    maxItems  = 1;
	dword    maxLevels = 4;

	Octree<OctreeItemTest> o1( position, size, maxItems, maxLevels );

	/// insert items into octree
	OctreeAgentTest a;
	o1.insertItem( item1, a );
	o1.insertItem( item2, a );
	o1.insertItem( item3, a );

	/// stream out octree
	OctreeStreamOut<OctreeItemTest> vso( std::cout, true );
	o1.visit( vso );
}




bool hxa7241_graphics::test_Octree
(
	std::ostream* pOut,
	const bool    isVerbose,
	dword         seed
)
{
	if( 0 == seed )
	{
		seed = ::time(0);
	}

	if( pOut != 0 )
	{
		pOut->flags( std::ios_base::boolalpha );// | std::ios_base::uppercase );
	}

	return testConstruction( pOut, isVerbose, seed ) &&
	       testCommands( pOut, isVerbose, seed );
}




bool testConstruction
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   seed
)
{
	/// Copying:
	///
	/// Generate some random octrees. Copy each and check it is identical to
	/// its original in value, and check all octree object ids are different.

	bool isOk = true;

	RandomFast rand( seed );
	if( pOut != 0 )
	{
		*pOut << "\nseed= " << rand.getDword() << "\n";
	}

	/// loop
	for( dword i = 0;  i < 10;  ++i )
	{
		std::auto_ptr<Octree<OctreeItemTest> > po1;
		std::vector<OctreeItemTest>            items;
		makeRandomFilledOctree( rand, 30, po1, items );

		if( (*pOut != 0) && isVerbose )
		{
			OctreeStreamOut<OctreeItemTest> so( *pOut, true );
			po1->visit( so );
			*pOut << "\n";
		}

		/// make copy octree
		Octree<OctreeItemTest> o2( *po1 );

		{
			/// stream out each to string
			std::ostringstream ss1;
			OctreeStreamOut<OctreeItemTest> so1( ss1, false );
			po1->visit( so1 );
			std::ostringstream ss2;
			OctreeStreamOut<OctreeItemTest> so2( ss2, false );
			o2.visit( so2 );

			/// compare strings
			isOk &= (ss1.str() == ss2.str());
		}

		{
			/// make arrays of octree object ids
			OctreeVisitorTest v1( *po1 );
			po1->visit( v1 );
			OctreeVisitorTest v2( o2 );
			o2.visit( v2 );

			isOk &= (v1.getIds().size() == v2.getIds().size());

			/// check set intersection of ids is empty
			std::vector<const void*> ids1( v1.getIds() );
			std::vector<const void*> ids2( v2.getIds() );
			std::sort( ids1.begin(), ids1.end() );
			std::sort( ids2.begin(), ids2.end() );
			std::vector<const void*> intersect;
			std::set_intersection( ids1.begin(), ids1.end(),
								   ids2.begin(), ids2.end(),
								   intersect.begin() );
			isOk &= intersect.empty();
		}

		if( pOut != 0 )
		{
			*pOut << i << " " << isOk << "\n";
		}
	}

	if( pOut != 0 )
	{
		*pOut << "\n--- testConstruction: " << isOk << "\n";
	}

	return isOk;
}


bool testCommands
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   seed
)
{
	return testCommands1( pOut, isVerbose, seed ) &&
	       testCommands2( pOut, isVerbose, seed ) &&
	       testCommands3( pOut, isVerbose, seed );
}


bool testCommands1
(
	std::ostream* pOut,
	const bool    isVerbose,
	const dword   //seed
)
{
	/// simple structural test:
	///
	/// 1) make octree with four item per leaf limit
	/// 2) add an item in each octant
	/// 3) check each item is in the correct octant

	bool isOk = true;

	if( pOut != 0 )
	{
		*pOut << "\n\n";
	}


	/// make octree
	Octree<OctreeItemTest> o1( Vector3f::ZERO(), 2, 4, 4 );

	/// make items
	std::vector<OctreeItemTest> i1(8);
	OctreeAgentTest a;
	for( dword i = 0;  i < 8;  ++i )
	{
		/// add item in each octant
		const Vector3f position( float(i & 1)        + 0.25f,
		                         float((i >> 1) & 1) + 0.25f,
		                         float((i >> 2) & 1) + 0.25f );
		i1[i] = OctreeItemTest( position, Vector3f::HALF(), i );
		o1.insertItem( i1[i], a );

		if( (pOut != 0) & isVerbose )
		{
			*pOut << "item " << i << " :  " << i1[i] << "\n";
		}
	}
	if( (pOut != 0) & isVerbose )
	{
		*pOut << "\n";
	}

	if( (pOut != 0) & isVerbose )
	{
		OctreeStreamOut<OctreeItemTest> so( *pOut, true );
		o1.visit( so );
		*pOut << "\n";
	}

	/// make info from octree
	OctreeVisitorTest v1( o1 );
	o1.visit( v1 );

	/// step through leafs
	const std::vector<OctreeVisitorTest::LeafData>& leafs( v1.getLeafs() );
	for( udword j = leafs.size();  j-- > 0; )
	{
		const OctreeVisitorTest::LeafData& leaf( leafs[j] );

		const Vector3f& cellLower( leaf.first.getBound().getLowerCorner() );
		const Vector3f& cellUpper( leaf.first.getBound().getUpperCorner() );

		/// step through items
		const Array<const OctreeItemTest*>& items( leaf.second );
		for( int i = 0;  i < items.getLength();  ++i )
		{
			const OctreeItemTest* pItem = items[i];

			const Vector3f itemLower( pItem->getPosition() );
			const Vector3f itemUpper( itemLower + pItem->getDimensions() );

			/// check item is overlapping its leaf cell
			const bool is =
				OctreeAgentTest::isOverlapping( itemLower, itemUpper,
			                                    cellLower, cellUpper );
			isOk &= is;
			if( (pOut != 0) & isVerbose )
			{
				*pOut << "overlap " << is << " :  " <<
				         itemLower << "  " << itemUpper << "  " <<
				         cellLower << "  " << cellUpper << "\n";
			}

			/// check item is in correct leaf cell
			const dword octantIndex = pItem->getPayload();
			const Vector3f octantLower( float(octantIndex & 1),
			                            float((octantIndex >> 1) & 1),
			                            float((octantIndex >> 2) & 1) );
			const Vector3f octantUpper( octantLower + Vector3f::ONE() );
			isOk &= (cellLower == octantLower);
			isOk &= (cellUpper == octantUpper);

			if( pOut != 0 )
			{
				if( isVerbose )
				{
					*pOut << "\toctant match " << (cellLower == octantLower) <<
					         " :  " << cellLower << "  " << octantLower << "\n";
					*pOut << "\toctant match " << (cellUpper == octantUpper) <<
					         " :  " << cellUpper << "  " << octantUpper << "\n";
				}
				else
				{
					*pOut << isOk << "\n";
				}
			}
		}
	}

	if( pOut != 0 )
	{
		*pOut << "\n--- testCommands1: " << isOk << "\n";
	}

	return isOk;
}


bool testCommands2
(
	std::ostream* pOut,
	const bool    ,//isVerbose,
	const dword   seed
)
{
	/// basic insert/remove test:
	///
	/// insert a lot, visit to collect all items, check they are all there
	/// remove them all, get counts to check they all came out

	bool isOk = true;


	if( pOut != 0 )
	{
		*pOut << "\n\n";
	}

	RandomFast rand( seed );
	if( pOut != 0 )
	{
		*pOut << "\nseed= " << rand.getDword() << "\n\n";
	}

	for( dword j = 0;  j < 10;  ++j )
	{
		/// make octree with items
		std::auto_ptr<Octree<OctreeItemTest> > po1;
		std::vector<OctreeItemTest>            i1;
		makeRandomFilledOctree( rand, 100, po1, i1 );

//      /// only for 10 or so items (otherwise > 14MB of output)
//		if( (pOut != 0) & isVerbose )
//		{
//			OctreeStreamOut<OctreeItemTest> so( *pOut, false );
//			po1->visit( so );
//			*pOut << "\n";
//			for( udword i = 0;  i < i1.size();  ++i )
//			{
//				*pOut << i1[i] << "\n";
//			}
//		}

		/// collect leafs items
		OctreeVisitorTest v1( *po1 );
		po1->visit( v1 );

		dword byteSize, leafCount, itemRefCount, maxDepth;
		po1->getInfo( byteSize, leafCount, itemRefCount, maxDepth );

		/// check collection length equals info leaf count
		isOk &= (v1.getLeafs().size() == udword(leafCount));

		/// compare items list to leafs collection
		{
			const std::vector<OctreeVisitorTest::LeafData>& leafs( v1.getLeafs() );

			std::set<const void*> i2s;
			for( udword k = 0;  k < leafs.size();  ++k )
			{
				const Array<const OctreeItemTest*>& items( leafs[k].second );
				for( dword m = 0;  m < items.getLength();  ++m )
				{
					i2s.insert( items[m] );
				}
			}
			std::set<const void*> i1s;
			for( udword k = 0;  k < i1.size();  ++k )
			{
				i1s.insert( &(i1[k]) );
			}

			isOk &= (i1s == i2s);
		}

		/// remove items
		OctreeAgentTest a;
		for( udword i = 0;  i < i1.size();  ++i )
		{
			po1->removeItem( i1[i], a );

//      	/// only for 10 or so items (otherwise > 14MB of output)
//			if( (pOut != 0) & isVerbose )
//			{
//				OctreeStreamOut<OctreeItemTest> so( *pOut, false );
//				po1->visit( so );
//			}
		}

		/// check octree info data is all 'zero'
		//dword byteSize, leafCount, itemRefCount, maxDepth;
		po1->getInfo( byteSize, leafCount, itemRefCount, maxDepth );
		isOk &= (leafCount == 0);
		isOk &= (itemRefCount == 0);
		isOk &= (maxDepth == 0);
		isOk &= po1->isEmpty();

		if( pOut != 0 )
		{
			*pOut << j << " " << isOk << "\n";
		}
	}

	if( pOut != 0 )
	{
		*pOut << "\n--- testCommands2: " << isOk << "\n";
	}


	return isOk;
}


bool testCommands3
(
	std::ostream* ,//pOut,
	const bool    ,//isVerbose,
	const dword   //seed
)
{
	/// Structural conformance postconditions:
	///
	/// Randomly insert and remove many random points, one at a time.
	/// After each command, check structure of octree:
	/// * after insertion, the octree is:
	///    * same, or
	///    * one leaf holding one more item, or
	///    * one leaf has changed to a branch containing 1-8 leafs and 0-7
	///      nils
	///    * one leaf has changed to 0-n levels of branchs of one subcell,
	///      and either 2-8 leafs, or one leaf at max level
	/// * after removal, the octree is:
	///    * same, or
	///    * one leaf holding one less item, or
	///    * one leaf has changed to nil, or
	///    * eight leafs have disappeared, and their direct parent branch has
	///      changed to a leaf, or
	///    * one leaf has disappeared, one of its parent branchs has changed
	///      to nil, and any branchs between have disappeared

	bool isOk = true;
/*
	if( pOut != 0 )
	{
		*pOut << "\n\n";
	}

	RandomFast rand( seed );
	if( pOut != 0 )
	{
		*pOut << "\nseed= " << rand.getDword() << "\n\n";
	}


	if( pOut != 0 )
	{
		*pOut << "\n--- testCommands3: " << isOk << "\n";
	}
*/
	return isOk;
}




///-----------------------------------------------------------------------------

void makeRandomFilledOctree
(
	RandomFast&                             rand,
	const dword                             howManyItems,
	std::auto_ptr<Octree<OctreeItemTest> >& pOctree,
	std::vector<OctreeItemTest>&            items
)
{
	makeRandomOctree( rand, pOctree );
	makeRandomItems( rand, howManyItems,
	                 pOctree->getPosition(), pOctree->getSize(),
	                 items );

	OctreeAgentTest a;
	for( int i = 0, end = items.size();  i < end;  ++i )
	{
		pOctree->insertItem( items[i], a );
	}
}


void makeRandomOctree
(
	RandomFast&                             rand,
	std::auto_ptr<Octree<OctreeItemTest> >& pOctree
)
{
	/// position: random between -/+100
	/// (making sure x,y,z are defined in order)
	const float x = rand.next().getFloat(200.0f, -100.0f);
	const float y = rand.next().getFloat(200.0f, -100.0f);
	const float z = rand.next().getFloat(200.0f, -100.0f);
	const Vector3f position( x, y, z );

	/// size: random between 0.5 and 50
	const float size = rand.next().getFloat(49.5f, 0.5f);

	/// max items: random between 1 and 8
	const dword maxItems = (rand.next().getUdword() >> 29) + 1;

	/// max levels: random between bounds
	const dword maxLevels = (rand.next().getUdword() >> 30) + 3;

	/// construct
	pOctree.reset(
		new Octree<OctreeItemTest>( position, size, maxItems, maxLevels ) );
}


void makeRandomItems
(
	RandomFast&                  rand,
	const dword                  howMany,
	const Vector3f&              position,
	const float                  size,
	std::vector<OctreeItemTest>& items
)
{
	items.resize( howMany );

	for( dword i = 0;  i < howMany;  ++i )
	{
		/// make random item
		const float x = rand.next().getFloat();
		const float y = rand.next().getFloat();
		const float z = rand.next().getFloat();
		Vector3f itemPosition( x, y, z );
		itemPosition *= (size * 0.99f);
		itemPosition += (Vector3f::ONE() * (size * 0.005f));
		itemPosition += position;

		/// add item
		if( rand.next().getDword() >= 0 )
		{
			items[i] = OctreeItemTest( itemPosition, Vector3f::ZERO() );
		}
		else
		{
			const float x = rand.next().getFloat();
			const float y = rand.next().getFloat();
			const float z = rand.next().getFloat();
			Vector3f dimensions( x, y, z );
			dimensions *= 0.175f;
			dimensions += Vector3f(0.025f, 0.025f, 0.025f);
			dimensions *= (size / 3.0f);
			items[i] = OctreeItemTest( itemPosition, dimensions );
		}
	}
}








/// main ///////////////////////////////////////////////////////////////////////

int main
(
	int   argc,
	char* argv[]
)
{
	bool  isVerbose = false;
	dword seed      = 0;

	if( argc >= 2 )
	{
		const std::string a1( argv[1] );
		isVerbose = (a1 == std::string("true"));
	}
	if( argc >= 3 )
	{
		std::istringstream iss( argv[2] );
		iss >> seed;
	}

	const bool isOk = hxa7241_graphics::test_Octree( &std::cout, isVerbose, seed );

	std::cout << "\n\n" << (isOk ? "---" : "***") << " test_Octree: " << isOk << "\n";


	return int(!isOk);
}
