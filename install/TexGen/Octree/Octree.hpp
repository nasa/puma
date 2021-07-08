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


#ifndef Octree_h
#define Octree_h


#include "OctreeAuxiliary.hpp"
#include "OctreeImplementation.hpp"




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{
	using hxa7241_graphics::Vector3f;


/**
 * agent abstract base, for client use with Octree.<br/><br/>
 *
 * client of Octree must define a concrete derivative of
 * OctreeAgent<ItemType>.<br/><br/>
 *
 * this is similar to a proxy: its an intermediary for an octree to query
 * its typeless subject items, when inserting or removing.<br/><br/>
 *
 * the overlap methods are to determine an items relation to a cell or cells,
 * for insertion or removal. the parameters supply the bounds of the cell.
 * <br/><br/>
 *
 * return value of getSubcellOverlaps is 8 bits, each bit is a bool
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
 *
 * @implementation
 * the ___V methods simply apply a type cast to void*s and forward to their
 * abstract counterparts.<br/><br/>
 *
 * an octree requires its contained items to provide positional info. but
 * requiring the item classes to implement an OctreeItem interface would
 * impose a direct interface change on every prospective item type, and enlarge
 * their instances with a vptr.<br/><br/>
 *
 * instead, this agent transfers the octree related interface/implementation
 * away from the item type into a separate class. the octree can now hold void
 * pointers to items and call the agent to query them indirectly.<br/><br/>
 */
template<class TYPE>
class OctreeAgent
	: public OctreeAgentV
{
/// standard object services ---------------------------------------------------
protected:
	         OctreeAgent() {}
public:
	virtual ~OctreeAgent() {}
private:
	         OctreeAgent( const OctreeAgent& );
	OctreeAgent& operator=( const OctreeAgent& );


/// void-to-type forwarders
public:
/// queries --------------------------------------------------------------------
	virtual bool  isOverlappingCellV ( const void*     pItem,
	                                   const Vector3f& lowerCorner,
	                                   const Vector3f& upperCorner )      const;
	virtual dword getSubcellOverlapsV( const void*     pItem,
	                                   const Vector3f& lower,
	                                   const Vector3f& middle,
	                                   const Vector3f& upper )            const;


/// abstract interface
protected:
/// queries --------------------------------------------------------------------
	/**
	 * called by octree to get relation of item to cell.
	 */
	virtual bool  isOverlappingCell ( const TYPE&     item,
	                                  const Vector3f& lowerCorner,
	                                  const Vector3f& upperCorner )    const =0;
	/**
	 * called by octree to get relation of item to subcell octants.<br/><br/>
	 * allows more efficent calculation, since boundaries are shared. but client
	 * could implement by delegating to isOverlappingCell.
	 * @return
	 * 8 bits, each a bool corresponding to a subcell, the high bit for subcell
	 * 7, the low bit for subcell 0.<br/><br/>
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
	virtual dword getSubcellOverlaps( const TYPE&     item,
	                                  const Vector3f& lowerCorner,
	                                  const Vector3f& middlePoint,
	                                  const Vector3f& upperCorner )    const =0;
};




/// void-to-type forwarders
template<class TYPE>
inline
bool OctreeAgent<TYPE>::isOverlappingCellV
(
	const void*     pItem,
	const Vector3f& lowerCorner,
	const Vector3f& upperCorner
) const
{
	bool is = false;

	if( pItem != 0 )
	{
		is = isOverlappingCell( *reinterpret_cast<const TYPE*>( pItem ),
		                        lowerCorner, upperCorner );
	}

	return is;
}


template<class TYPE>
inline
dword OctreeAgent<TYPE>::getSubcellOverlapsV
(
	const void*     pItem,
	const Vector3f& lower,
	const Vector3f& middle,
	const Vector3f& upper
) const
{
	dword ov = ALL_OUTSIDE;

	if( pItem != 0 )
	{
		ov = getSubcellOverlaps( *reinterpret_cast<const TYPE*>( pItem ),
		                         lower, middle, upper );
	}

	return ov;
}




/**
 * visitor abstract base, for client use with Octree.<br/><br/>
 *
 * client of Octree must define a concrete derivative of
 * OctreeVisitor<ItemType>.<br/><br/>
 *
 * this is a reversal of the Visitor pattern: it allows an operation to be
 * performed with the octree, except the octree is merely read from and it is
 * the visitor that is modified.<br/><br/>
 *
 * the visit methods are called by the tree nodes during the visit operation.
 * the parameters supply the cell and boundary info. the implementation can
 * call visit on the supplied cell.<br/><br/>
 *
 * the implementation of visitBranch needs to make the OctreeData to be given
 * in each call of visit.
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
 * @implementation
 * the ___V methods simply apply a type cast to void*s and forward to their
 * abstract counterparts.<br/><br/>
 */
template<class TYPE>
class OctreeVisitor
	: public OctreeVisitorV
{
/// standard object services ---------------------------------------------------
protected:
	         OctreeVisitor() {}
public:
	virtual ~OctreeVisitor() {}
private:
	         OctreeVisitor( const OctreeVisitor& );
	OctreeVisitor& operator=( const OctreeVisitor& );


/// void-to-type forwarders
public:
/// commands -------------------------------------------------------------------
	virtual void  visitRootV  ( const OctreeCell* pRootCell,
	                            const OctreeData& octreeData );
	virtual void  visitBranchV( const OctreeCell* subCells[8],
	                            const OctreeData& octreeData );
	virtual void  visitLeafV  ( const Array<const void*>& items,
	                            const OctreeData&         octreeData );


/// abstract interface
protected:
/// commands -------------------------------------------------------------------
	/**
	 * called by octree when visit traversal is at the root.<br/><br/>
	 * to continue deeper, implementation calls visit on pRootCell, supplying
	 * octreeData and *this as arguments.<br/><br/>
	 * @see OctreeData
	 */
	virtual void  visitRoot  ( const OctreeCell* pRootCell,
	                           const OctreeData& octreeData )                =0;
	/**
	 * called by octree when visit traversal is at a branch.<br/><br/>
	 * to continue deeper, implementation calls visit on any/all subcells,
	 * supplying a new octreeData and *this as arguments. the new octreeData is
	 * made with its constructor that will take the original octreeData and the
	 * subcell index. check subcell values before use, since they can be null.
	 * <br/><br/>
	 * @see OctreeData
	 */
	virtual void  visitBranch( const OctreeCell* subCells[8],
	                           const OctreeData& octreeData )                =0;
	/**
	 * called by octree when visit traversal is at a leaf.<br/><br/>
	 * @see OctreeData
	 */
	virtual void  visitLeaf  ( const Array<const TYPE*>& items,
	                           const OctreeData&         octreeData )        =0;
};




/// void-to-type forwarders
template<class TYPE>
inline
void OctreeVisitor<TYPE>::visitRootV
(
	const OctreeCell* pRootCell,
	const OctreeData& octreeData
)
{
	visitRoot( pRootCell, octreeData );
}


template<class TYPE>
inline
void OctreeVisitor<TYPE>::visitBranchV
(
	const OctreeCell* subCells[8],
	const OctreeData& octreeData
)
{
	visitBranch( subCells, octreeData );
}


template<class TYPE>
inline
void OctreeVisitor<TYPE>::visitLeafV
(
	const Array<const void*>& items,
	const OctreeData&         octreeData
)
{
	visitLeaf( reinterpret_cast<const Array<const TYPE*>&>( items ),
	           octreeData );
}








/**
 * octree based spatial index.<br/><br/>
 *
 * client must define concrete derivatives of OctreeAgent<ItemType> and
 * OctreeVisitor<ItemType>.<br/><br/>
 *
 * maxItemCountPerCell is ignored where maxLevelCount is reached.<br/><br/>
 *
 * the octree is cubical and axis aligned, partitions are axis aligned,
 * partitions divide in half, each level partitions the previous level in all
 * three axiss.<br/><br/>
 *
 * storage is contracted or expanded as needed by item insertion and removal.
 * <br/><br/>
 *
 * (occupies, very approximately, 20 bytes per point item. maybe...)<br/><br/>
 *
 * octree is only an index: it points to client items, it does not manage
 * storage of items themselves.<br/><br/>
 *
 * @see OctreeAgent
 * @see OctreeVisitor
 *
 * @implementation
 * the octree structure follows the Composite pattern.<br/><br/>
 *
 * this template wrapper ensures the items indexed by the octree and the agents
 * and visitors used when accessing them are of matching types. all algorithmic
 * work is delegated to OctreeRoot and OctreeCell derivatives in
 * OctreeImplementation, which work with abstract base interfaces and void
 * pointers.<br/><br/>
 *
 * for the insertion and removal commands, the agent provides an interface for
 * the octree to query the typeless item, and for the visit query, the visitor
 * provides callbacks to read tree nodes for carrying out the visit operation.
 */
template<class TYPE>
class Octree
{
/// standard object services ---------------------------------------------------
public:
	/**
	 * constructs a particular format of octree.<br/><br/>
	 * @parameters
	 * * sizeOfCube is desired length along a side<br/>
	 * * maxItemCountPerCell is desired max item pointers per leaf<br/>
	 * * maxLevelCount is desired max depth of tree<br/>
	 */
	         Octree( const Vector3f& positionOfLowerCorner,
	                 float           sizeOfCube,
	                 dword           maxItemCountPerCell,
	                 dword           maxLevelCount );

	virtual ~Octree();
	         Octree( const Octree& );
	Octree& operator=( const Octree& );


/// commands -------------------------------------------------------------------
	/**
	 * add pointer(s) to the item to the octree.<br/><br/>
	 * (if an item has non-zero volume, it may have pointers in multiple
	 * cells.)<br/><br/>
	 * @return is the item inserted -- false if item not inside root bound
	 * @exceptions
	 * can throw storage allocation exceptions. in such cases the octree remains
	 * structurally ok, but the item will not be fully added, -- call this
	 * method again or the removeItem method.
	 * @see removeItem, OctreeAgent
	 */
	virtual bool  insertItem( const TYPE&              item,
	                          const OctreeAgent<TYPE>& agent );
	/**
	 * removes pointer(s) to the item from the octree.<br/><br/>
	 * (if an item has non-zero volume, it may have pointers in multiple
	 * cells.)<br/><br/>
	 * @return is the item removed -- false if item wasn't present
	 * @exceptions
	 * can throw storage allocation exceptions. in such cases the octree remains
	 * structurally ok, but the item will not be fully removed, -- call this
	 * method again or the insertItem method.
	 * @see insertItem, OctreeAgent
	 */
	virtual bool  removeItem( const TYPE&              item,
	                          const OctreeAgent<TYPE>& agent );


/// queries --------------------------------------------------------------------
	/**
	 * execute a visit query operation.
	 * @see OctreeVisitor
	 */
	virtual void  visit( OctreeVisitor<TYPE>& visitor )                   const;

	/**
	 * reports if the octree is empty.
	 */
	virtual bool  isEmpty()                                               const;
	/**
	 * provides stats on the octree.<br/><br/>
	 * @parameters
	 * * byteSize is size in bytes<br/>
	 * * leafCount is number of leafs<br/>
	 * * itemRefCount is total number of item pointers in all leafs<br/>
	 * * maxDepth is deepest depth of tree<br/>
	 */
	virtual void  getInfo( dword& byteSize,
	                       dword& leafCount,
	                       dword& itemRefCount,
	                       dword& maxDepth )                              const;

	/**
	 * gives the position supplied at construction.
	 */
	virtual const Vector3f& getPosition()                                 const;
	/**
	 * gives the size supplied at construction.
	 */
	virtual float           getSize()                                     const;
	/**
	 * gives the leaf pointer limit supplied at construction.
	 */
	virtual dword           getMaxItemCountPerCell()                      const;
	/**
	 * gives the depth limit supplied at construction.
	 */
	virtual dword           getMaxLevelCount()                            const;


/// fields ---------------------------------------------------------------------
private:
	OctreeRoot root_m;
};




/// templates ///

/// standard object services ---------------------------------------------------
template<class TYPE>
inline
Octree<TYPE>::Octree
(
	const Vector3f& position,
	const float     sizeOfCube,
	const dword     maxItemCountPerCell,
	const dword     maxLevelCount
)
 :	root_m( position, sizeOfCube, maxItemCountPerCell, maxLevelCount )
{
}


template<class TYPE>
inline
Octree<TYPE>::~Octree()
{
}


template<class TYPE>
inline
Octree<TYPE>::Octree
(
	const Octree& other
)
 :	root_m( other.root_m )
{
}


template<class TYPE>
inline
Octree<TYPE>& Octree<TYPE>::operator=
(
	const Octree& other
)
{
	root_m = other.root_m;

	return *this;
}




/// commands -------------------------------------------------------------------
template<class TYPE>
inline
bool Octree<TYPE>::insertItem
(
	const TYPE&              item,
	const OctreeAgent<TYPE>& agent
)
{
	return root_m.insertItem( &item, agent );
}


template<class TYPE>
inline
bool Octree<TYPE>::removeItem
(
	const TYPE&              item,
	const OctreeAgent<TYPE>& agent
)
{
	return root_m.removeItem( &item, agent );
}




/// queries --------------------------------------------------------------------
template<class TYPE>
inline
void Octree<TYPE>::visit
(
	OctreeVisitor<TYPE>& visitor
) const
{
	root_m.visit( visitor );
}


template<class TYPE>
inline
bool Octree<TYPE>::isEmpty() const
{
	return root_m.isEmpty();
}


template<class TYPE>
inline
void Octree<TYPE>::getInfo
(
	dword& byteSize,
	dword& leafCount,
	dword& itemRefCount,
	dword& maxDepth
) const
{
	root_m.getInfo( sizeof(*this), byteSize, leafCount,
	                itemRefCount, maxDepth );
}


template<class TYPE>
inline
const Vector3f& Octree<TYPE>::getPosition() const
{
	return root_m.getPosition();
}


template<class TYPE>
inline
float Octree<TYPE>::getSize() const
{
	return root_m.getSize();
}


template<class TYPE>
inline
dword Octree<TYPE>::getMaxItemCountPerCell() const
{
	return root_m.getMaxItemCountPerCell();
}


template<class TYPE>
inline
dword Octree<TYPE>::getMaxLevelCount() const
{
	return root_m.getMaxLevelCount();
}


}//namespace




#endif//Octree_h
