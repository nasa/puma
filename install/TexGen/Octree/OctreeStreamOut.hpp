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


#ifndef OctreeStreamOut_h
#define OctreeStreamOut_h


#include "Octree.hpp"
#include <iosfwd>




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{


/**
 * implementation for OctreeStreamOut template.<br/><br/>
 *
 * @see OctreeStreamOut
 */
class OctreeStreamOutImplementation
{
/// standard object services ---------------------------------------------------
public:
	         OctreeStreamOutImplementation( std::ostream& outStream,
	                                        bool          isLongFormat );

	        ~OctreeStreamOutImplementation();
	         OctreeStreamOutImplementation( const OctreeStreamOutImplementation& );
	OctreeStreamOutImplementation& operator=( const OctreeStreamOutImplementation& );


/// commands -------------------------------------------------------------------
	        void  visitRoot  ( const OctreeCell* pRootCell,
	                           const OctreeData& octreeData,
	                           OctreeVisitorV&   visitor );
	        void  visitBranch( const OctreeCell* subCells[8],
	                           const OctreeData& octreeData,
	                           OctreeVisitorV&   visitor );
	        void  visitLeafBefore( const OctreeData& octreeData );
	        void  visitLeafAfter ();

	        bool          isLongFormat()                                  const;
	        std::ostream& getOutStream()                                  const;
	        std::ostream& indentNewline( dword extraIndent )              const;


/// implementation -------------------------------------------------------------
protected:
	static  void writeDataAndInfo( const OctreeCell& octreeCell,
	                               const OctreeData& octreeData,
	                               std::ostream&     outStream );


/// fields ---------------------------------------------------------------------
private:
	std::ostream* pOutStream_m;
	bool          isLongFormat_m;

	/// visit state (only used during a visit)
	const OctreeCell* pCurrentCell_m;
	dword             indent_m;
};





/**
 * an octree visitor for writing a text representation of an octree and its
 * contents.<br/><br/>
 *
 * @see OctreeStreamOutImplementation
 */
template<class TYPE>
class OctreeStreamOut
	: public OctreeVisitor<TYPE>
{
/// standard object services ---------------------------------------------------
public:
	         OctreeStreamOut( std::ostream& outStream,
	                          bool          isLongFormat );

	virtual ~OctreeStreamOut();
	         OctreeStreamOut( const OctreeStreamOut& );
	OctreeStreamOut& operator=( const OctreeStreamOut& );


/// octree visitor overrides
/// commands -------------------------------------------------------------------
protected:
	virtual void  visitRoot  ( const OctreeCell* pRootCell,
	                           const OctreeData& octreeData );
	virtual void  visitBranch( const OctreeCell* subCells[8],
	                           const OctreeData& octreeData );
	virtual void  visitLeaf  ( const Array<const TYPE*>& items,
	                           const OctreeData&         octreeData );


/// fields ---------------------------------------------------------------------
private:
	OctreeStreamOutImplementation implementation_m;
};








/// TEMPLATES ///

/// standard object services ---------------------------------------------------
template<class TYPE>
inline
OctreeStreamOut<TYPE>::OctreeStreamOut
(
	std::ostream& outStream,
	const bool    isLongFormat
)
 :	OctreeVisitor<TYPE>()
 ,	implementation_m( outStream, isLongFormat )
{
}


template<class TYPE>
inline
OctreeStreamOut<TYPE>::~OctreeStreamOut()
{
}


template<class TYPE>
inline
OctreeStreamOut<TYPE>::OctreeStreamOut
(
	const OctreeStreamOut& other
)
 :	OctreeVisitor<TYPE>()
 ,	implementation_m( other.implementation_m )
{
}


template<class TYPE>
inline
OctreeStreamOut<TYPE>& OctreeStreamOut<TYPE>::operator=
(
	const OctreeStreamOut& other
)
{
	if( &other != this )
	{
		implementation_m = other.implementation_m;
	}

	return *this;
}




/// commands -------------------------------------------------------------------
template<class TYPE>
inline
void OctreeStreamOut<TYPE>::visitRoot
(
	const OctreeCell* pRootCell,
	const OctreeData& octreeData
)
{
	implementation_m.visitRoot( pRootCell, octreeData, *this );
}


template<class TYPE>
inline
void OctreeStreamOut<TYPE>::visitBranch
(
	const OctreeCell* subCells[8],
	const OctreeData& octreeData
)
{
	implementation_m.visitBranch( subCells, octreeData, *this );
}


template<class TYPE>
void OctreeStreamOut<TYPE>::visitLeaf
(
	const Array<const TYPE*>& items,
	const OctreeData&         octreeData
)
{
	implementation_m.visitLeafBefore( octreeData );

	const TYPE** ppItem = items.getMemory();
	const TYPE** ppEnd  = ppItem + items.getLength();

	for( ;  ppItem < ppEnd;  ++ppItem )
	{
		if( implementation_m.isLongFormat() )
		{
			implementation_m.indentNewline( +1 );
			implementation_m.getOutStream() << "id:" << *ppItem << ": " << **ppItem;
		}
		else
		{
			implementation_m.getOutStream() << "<"  << "id:" << *ppItem <<
			                                   ": " << **ppItem << "> ";
		}
	}

	implementation_m.visitLeafAfter();
}


}//namespace




#endif//OctreeStreamOut_h
