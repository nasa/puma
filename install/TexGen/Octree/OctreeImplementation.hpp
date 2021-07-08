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


#ifndef OctreeImplementation_h
#define OctreeImplementation_h


#include "OctreeAuxiliary.hpp"
#include "Array.hpp"




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{
	using hxa7241_graphics::Vector3f;
	using hxa7241_general::Array;
	class OctreeCell;


/**
 * implementation class for the Octree template.
 *
 * @invariants
 * pRootCell_m can be null, or point to an OctreeCell instance.<br/><br/>
 *
 * at construction, pRootCell_m is set to a legal value.<br/>
 * at destruction, pRootCell_m is deleted.<br/>
 * whenever pRootCell_m is modified, it must be deleted then set to a legal
 * value.<br/>
 * a legal value is: either 0, or the value from invocation of 'new'.
 */
class OctreeRoot
{
/// standard object services ---------------------------------------------------
public:
	         OctreeRoot( const Vector3f& position,
	                     float           sizeOfCube,
	                     dword           maxItemsPerCell,
	                     dword           maxLevelCount );

	        ~OctreeRoot();
	         OctreeRoot( const OctreeRoot& );
	OctreeRoot& operator=( const OctreeRoot& );


/// commands -------------------------------------------------------------------
	        bool  insertItem( const void*         pItem,
	                          const OctreeAgentV& agent );
	        bool  removeItem( const void*         pItem,
	                          const OctreeAgentV& agent );


/// queries --------------------------------------------------------------------
	        void  visit( OctreeVisitorV& visitor )                        const;

	        bool  isEmpty()                                               const;
	        void  getInfo( dword  rootWrapperByteSize,
	                       dword& byteSize,
	                       dword& leafCount,
	                       dword& itemCount,
	                       dword& maxDepth )                              const;

	        const Vector3f& getPosition()                                 const;
	        float           getSize()                                     const;
	        dword           getMaxItemCountPerCell()                      const;
	        dword           getMaxLevelCount()                            const;


/// fields ---------------------------------------------------------------------
private:
	OctreeDimensions dimensions_m;
	OctreeCell*      pRootCell_m;
};




/**
 * abstract base for Composite types, for implementing Octree nodes.
 *
 * @implementation
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
class OctreeCell
{
/// standard object services ---------------------------------------------------
protected:
	         OctreeCell() {}
public:
	virtual ~OctreeCell() {}
private:
	         OctreeCell( const OctreeCell& );
	OctreeCell& operator=( const OctreeCell& );


/// commands -------------------------------------------------------------------
public:
	virtual void  insertItem( const OctreeData&   thisData,
	                          OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const OctreeAgentV& agent )                    =0;
	virtual bool  removeItem( OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const dword         maxItemsPerCell,
	                          dword&              itemCount )                =0;


/// queries --------------------------------------------------------------------
	virtual void  visit( const OctreeData& thisData,
	                     OctreeVisitorV&   visitor )                   const =0;

	virtual OctreeCell* clone()                                        const =0;

	virtual void  getInfo( dword& byteSize,
	                       dword& leafCount,
	                       dword& itemCount,
	                       dword& maxDepth )                           const =0;


/// statics --------------------------------------------------------------------
	static  OctreeCell* cloneNonZero( const OctreeCell* );
};




/**
 * inner node implementation of an octree cell.<br/><br/>
 *
 * stores pointers to eight child cells.
 *
 * @invariants
 * subCells_m elements can be null, or point to an OctreeCell instance.
 */
class OctreeBranch
	: public OctreeCell
{
/// standard object services ---------------------------------------------------
public:
	         OctreeBranch();
	         OctreeBranch( const OctreeData&         thisData,
	                       const Array<const void*>& items,
	                       const void* const         pItem,
	                       const OctreeAgentV&       agent );

	virtual ~OctreeBranch();
	         OctreeBranch( const OctreeBranch& );
	OctreeBranch& operator=( const OctreeBranch& );


/// commands -------------------------------------------------------------------
	virtual void  insertItem( const OctreeData&   thisData,
	                          OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const OctreeAgentV& agent );
	virtual bool  removeItem( OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const dword         maxItemsPerCell,
	                          dword&              itemCount );


/// queries --------------------------------------------------------------------
	virtual void  visit( const OctreeData& thisData,
	                     OctreeVisitorV&   visitor )                      const;

	virtual OctreeCell* clone()                                           const;

	virtual void  getInfo( dword& byteSize,
	                       dword& leafCount,
	                       dword& itemCount,
	                       dword& maxDepth )                              const;


/// implementation -------------------------------------------------------------
protected:
	virtual void  zeroSubCells();


/// fields ---------------------------------------------------------------------
private:
	OctreeCell* subCells_m[8];
};




/**
 * outer node implementation of an octree cell.<br/><br/>
 *
 * stores pointers to items.
 */
class OctreeLeaf
	: public OctreeCell
{
/// standard object services ---------------------------------------------------
public:
	         OctreeLeaf();
	         OctreeLeaf( const OctreeLeaf*const leafs[8] );
private:
	explicit OctreeLeaf( const void* pItem );

public:
	virtual ~OctreeLeaf();
	         OctreeLeaf( const OctreeLeaf& );
	OctreeLeaf& operator=( const OctreeLeaf& );


/// commands -------------------------------------------------------------------
	virtual void  insertItem( const OctreeData&   thisData,
	                          OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const OctreeAgentV& agent );
	virtual bool  removeItem( OctreeCell*&        pThis,
	                          const void*         pItem,
	                          const dword         maxItemsPerCell,
	                          dword&              itemCount );


/// queries --------------------------------------------------------------------
	virtual void  visit( const OctreeData& thisData,
	                     OctreeVisitorV&   visitor )                      const;

	virtual OctreeCell* clone()                                           const;

	virtual void  getInfo( dword& byteSize,
	                       dword& leafCount,
	                       dword& itemCount,
	                       dword& maxDepth )                              const;


/// statics --------------------------------------------------------------------
	static  void  insertItemMaybeCreate( const OctreeData&   cellData,
	                                     OctreeCell*&        pCell,
	                                     const void*         pItem,
	                                     const OctreeAgentV& agent );


/// fields ---------------------------------------------------------------------
private:
	Array<const void*> items_m;
};


}//namespace




#endif//OctreeImplementation_h
