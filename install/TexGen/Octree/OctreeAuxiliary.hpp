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


#ifndef OctreeAuxiliary_h
#define OctreeAuxiliary_h


#include "Vector3f.hpp"
#include "Array.hpp"




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{
	using hxa7241_graphics::Vector3f;
	using hxa7241_general::Array;

	class OctreeRoot;
	class OctreeCell;


/**
 * global octree data -- one instance for whole octree.<br/><br/>
 *
 * constant.
 */
class OctreeDimensions
{
/// standard object services ---------------------------------------------------
public:
	         OctreeDimensions( const Vector3f& positionOfLowerCorner,
	                           float           size,
	                           dword           maxItemCountPerCell,
	                           dword           maxLevelCount );

	        ~OctreeDimensions();
	         OctreeDimensions( const OctreeDimensions& );
	OctreeDimensions& operator=( const OctreeDimensions& );


/// queries --------------------------------------------------------------------
	        const Vector3f& getPosition()                                 const;
	        float           getSize()                                     const;
	        dword           getMaxItemCountPerCell()                      const;
	        dword           getMaxLevelCount()                            const;

	        bool            isSubdivide( dword itemCount,
	                                     dword level )                    const;


/// fields ---------------------------------------------------------------------
private:
	Vector3f positionOfLowerCorner_m;
	float    size_m;
	dword    maxItemsPerCell_m;
	dword    maxLevel_m;

	static const dword MAX_LEVEL;
};




/**
 * geometric data for the bound of an octree cell.<br/><br/>
 *
 * constant.<br/><br/>
 *
 * radius is that of the circumsphere.<br/><br/>
 *
 * subcell numbering:
 * <pre>
 *    y z       6 7
 *    |/   2 3  4 5
 *     -x  0 1
 * </pre>
 * in binary:
 * <pre>
 *    y z           110 111
 *    |/   010 011  100 101
 *     -x  000 001
 * </pre>
 */
class OctreeBound
{
/// standard object services ---------------------------------------------------
public:
	         OctreeBound();
	         OctreeBound( const Vector3f& positionOfLowerCorner,
	                      float           size );
	         OctreeBound( const OctreeBound& parentCellBound,
	                      dword              subCellIndex );

	        ~OctreeBound();
	         OctreeBound( const OctreeBound& );
	OctreeBound& operator=( const OctreeBound& );


/// queries --------------------------------------------------------------------
	        const Vector3f& getLowerCorner()                              const;
	        const Vector3f& getUpperCorner()                              const;
	        const Vector3f& getCenter()                                   const;
	        float           getRadius()                                   const;


/// fields ---------------------------------------------------------------------
private:
	Vector3f positionOfLowerCorner_m;
	Vector3f positionOfUpperCorner_m;
	Vector3f center_m;
	float    circumSphereRadius_m;
};




/**
 * global and local octree cell data.<br/><br/>
 *
 * constant.<br/><br/>
 *
 * to be made during each level of tree descent, so storage is avoided, except
 * to hold one at the root.<br/><br/>
 *
 * subcell numbering:
 * <pre>
 *    y z       6 7
 *    |/   2 3  4 5
 *     -x  0 1
 * </pre>
 * in binary:
 * <pre>
 *    y z           110 111
 *    |/   010 011  100 101
 *     -x  000 001
 * </pre>
 */
class OctreeData
{
/// standard object services ---------------------------------------------------
public:
	explicit OctreeData( const OctreeDimensions& dimensions );
	         OctreeData( const OctreeData& parentCellData,
	                     dword             subCellIndex );
	         OctreeData( const OctreeData&,
	                     const OctreeDimensions& );

	        ~OctreeData();
	         OctreeData( const OctreeData& );
	OctreeData& operator=( const OctreeData& );


/// queries --------------------------------------------------------------------
	        const OctreeBound&      getBound()                            const;
	        dword                   getLevel()                            const;
	        const OctreeDimensions& getDimensions()                       const;

	        bool  isSubdivide( dword itemCount )                          const;


/// fields ---------------------------------------------------------------------
private:
	/// local to cell
	OctreeBound bound_m;
	dword       level_m;

	/// global for octree
	const OctreeDimensions* pDimensions_m;
};




/**
 * agent abstract base, for Octree implementation use.<br/><br/>
 *
 * return value of getSubcellOverlapsV is 8 bits, each bit is a bool
 * corresponding to a subcell, the high bit for subcell 7, the low bit for
 * subcell 0.<br/><br/>
 *
 * subcell numbering:
 * <pre>
 *    y z       6 7
 *    |/   2 3  4 5
 *     -x  0 1
 * </pre>
 * in binary:
 * <pre>
 *    y z           110 111
 *    |/   010 011  100 101
 *     -x  000 001
 * </pre>
 */
class OctreeAgentV
{
/// standard object services ---------------------------------------------------
protected:
	         OctreeAgentV() {}
public:
	virtual ~OctreeAgentV() {}
private:
	         OctreeAgentV( const OctreeAgentV& );
	OctreeAgentV& operator=( const OctreeAgentV& );


/// queries --------------------------------------------------------------------
public:
	virtual bool  isOverlappingCellV ( const void*     pItem,
	                                   const Vector3f& lowerCorner,
	                                   const Vector3f& upperCorner )   const =0;
	virtual dword getSubcellOverlapsV( const void*     pItem,
	                                   const Vector3f& lower,
	                                   const Vector3f& middle,
	                                   const Vector3f& upper )         const =0;


/// constants ------------------------------------------------------------------
	static const dword ALL_INSIDE  = 0x0000FFFF;
	static const dword ALL_OUTSIDE = 0x00000000;
};




/**
 * visitor abstract base, for Octree implementation use.<br/><br/>
 *
 * subcell numbering:
 * <pre>
 *    y z       6 7
 *    |/   2 3  4 5
 *     -x  0 1
 * </pre>
 * in binary:
 * <pre>
 *    y z           110 111
 *    |/   010 011  100 101
 *     -x  000 001
 * </pre>
 *
 * @see OctreeCell
 * @see OctreeBranch
 * @see OctreeLeaf
 */
class OctreeVisitorV
{
/// standard object services ---------------------------------------------------
protected:
	         OctreeVisitorV() {}
public:
	virtual ~OctreeVisitorV() {}
private:
	         OctreeVisitorV( const OctreeVisitorV& );
	OctreeVisitorV& operator=( const OctreeVisitorV& );


/// commands -------------------------------------------------------------------
public:
	virtual void  visitRootV  ( const OctreeCell* pRootCell,
	                            const OctreeData& octreeData )               =0;
	virtual void  visitBranchV( const OctreeCell* subCells[8],
	                            const OctreeData& octreeData )               =0;
	virtual void  visitLeafV  ( const Array<const void*>& items,
	                            const OctreeData&         octreeData )       =0;
};








/// inlines ///

/// OctreeDimensions -----------------------------------------------------------
inline
const Vector3f& OctreeDimensions::getPosition() const
{
	return positionOfLowerCorner_m;
}


inline
float OctreeDimensions::getSize() const
{
	return size_m;
}


inline
dword OctreeDimensions::getMaxItemCountPerCell() const
{
	return maxItemsPerCell_m;
}


inline
dword OctreeDimensions::getMaxLevelCount() const
{
	return maxLevel_m + 1;
}




/// OctreeBound ----------------------------------------------------------------
inline
const Vector3f& OctreeBound::getLowerCorner() const
{
	return positionOfLowerCorner_m;
}


inline
const Vector3f& OctreeBound::getUpperCorner() const
{
	return positionOfUpperCorner_m;
}


inline
const Vector3f& OctreeBound::getCenter() const
{
	return center_m;
}


inline
float OctreeBound::getRadius() const
{
	return circumSphereRadius_m;
}




/// OctreeData -----------------------------------------------------------------
inline
const OctreeBound& OctreeData::getBound() const
{
	return bound_m;
}


inline
dword OctreeData::getLevel() const
{
	return level_m;
}


inline
const OctreeDimensions& OctreeData::getDimensions() const
{
	return *pDimensions_m;
}


inline
bool OctreeData::isSubdivide
(
	const dword itemCount
) const
{
	return pDimensions_m->isSubdivide( itemCount, level_m );
}


}//namespace




#endif//OctreeAuxiliary_h
