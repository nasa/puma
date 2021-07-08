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


#ifndef Array_h
#define Array_h




#include "hxa7241_general.hpp"
namespace hxa7241_general
{


/**
 * a simpler, compacter alternative to std::vector.<br/><br/>
 *
 * length is explicit - there is no hidden reserve.<br/><br/>
 *
 * operator[] wraps around out of bound indexs.
 */
template<class TYPE>
class Array
{
/// standard object services ---------------------------------------------------
public:
	               Array();
	explicit       Array( dword length );                             /// throws

	virtual       ~Array();
	               Array( const Array& );                             /// throws
	        Array& operator=( const Array& );                         /// throws


/// commands -------------------------------------------------------------------
	virtual void   setLength( dword length );                         /// throws

	virtual void   swap( Array& );                                    /// throws
	virtual void   append( const TYPE& );                             /// throws
	virtual void   remove( int index );                               /// throws


/// queries --------------------------------------------------------------------
	virtual dword  getLength()                                            const;
	virtual bool   isEmpty()                                              const;
	static  dword  getMaxLength();

	virtual TYPE*  getMemory()                                            const;
	/**
	 * wraps out of bounds.
	 */
	virtual TYPE&  operator[]( int index )                                const;

	virtual void   zeroMemory()                                           const;


/// implementation -------------------------------------------------------------
protected:
	virtual void   assign( const Array<TYPE>& );

	virtual void   acquireMemory( dword length,
	                              bool  isCopied );

	static  void   copyObjects( TYPE*       lValStart,
	                            const TYPE* rValStart,
	                            dword       length );


/// fields ---------------------------------------------------------------------
protected:
	TYPE* pMemory_m;
	dword length_m;
};








/// standard object services ---------------------------------------------------
template<class TYPE>
Array<TYPE>::Array()
 :	pMemory_m( 0 )
 ,	length_m( 0 )
{
}


template<class TYPE>
Array<TYPE>::Array
(
	const dword length
)
 :	pMemory_m( 0 )
 ,	length_m( 0 )
{
	Array<TYPE>::setLength( length );
}


template<class TYPE>
Array<TYPE>::~Array()
{
	delete[] pMemory_m;
}


template<class TYPE>
Array<TYPE>::Array
(
	const Array<TYPE>& other
)
 :	pMemory_m( 0 )
 ,	length_m( 0 )
{
	Array<TYPE>::assign( other );
}


template<class TYPE>
Array<TYPE>& Array<TYPE>::operator=
(
	const Array<TYPE>& other
)
{
	assign( other );

	return *this;
}




/// commands -------------------------------------------------------------------
template<class TYPE>
void Array<TYPE>::setLength
(
	const dword length
)
{
	acquireMemory( length, false );
}


template<class TYPE>
void Array<TYPE>::swap
(
	Array<TYPE>& other
)
{
	TYPE*const tmpM  = pMemory_m;
	pMemory_m        = other.pMemory_m;
	other.pMemory_m  = tmpM;

	const dword tmpL = length_m;
	length_m         = other.length_m;
	other.length_m   = tmpL;
}


template<class TYPE>
void Array<TYPE>::append
(
	const TYPE& element
)
{
	/// expand storage, duplicating elements
	acquireMemory( length_m + 1, true );

	/// write new element into last position
	*(pMemory_m + length_m - 1) = element;


//	/// make larger storage
//	Array<TYPE> newArray( length_m + 1 );
//
//	/// duplicate elements, and append new element
//	copyObjects( newArray.pMemory_m, pMemory_m, length_m );
//	*(newArray.pMemory_m + length_m) = element;
//
//	/// swap new storage with this
//	swap( newArray );
}


template<class TYPE>
void Array<TYPE>::remove
(
	const int index
)
{
	/// check index is within range
	if( (index >= 0) & (index < length_m)  )
	{
		/// make smaller storage
		Array<TYPE> newArray( length_m - 1 );

		/// copy elements, skipping element at index
		{
			      TYPE* pDestination = newArray.pMemory_m;
			      TYPE* pEnd         = pDestination + newArray.length_m;
			const TYPE* pSource      = pMemory_m;
			const TYPE* pIndex       = pSource + index;
			while( pDestination < pEnd )
			{
				pSource += dword(pSource == pIndex);
				*(pDestination++) = *(pSource++);
			}
		}

		/// swap new storage with this
		swap( newArray );
	}
}




/// queries --------------------------------------------------------------------
template<class TYPE>
inline
dword Array<TYPE>::getLength() const
{
	return length_m;
}


template<class TYPE>
inline
bool Array<TYPE>::isEmpty() const
{
	return length_m == 0;
}


template<class TYPE>
inline
dword Array<TYPE>::getMaxLength()
{
	return DWORD_MAX;
}


template<class TYPE>
inline
TYPE* Array<TYPE>::getMemory() const
{
	return pMemory_m;
}


template<class TYPE>
//inline
TYPE& Array<TYPE>::operator[]
(
	const int index
) const
{
	return pMemory_m[ (index >= 0 ? index : -index) % length_m ];
}


template<class TYPE>
void Array<TYPE>::zeroMemory() const
{
	for( dword i = length_m;  i-- > 0; )
	{
		pMemory_m[ i ] = TYPE();
	}
}




/// implementation -------------------------------------------------------------
template<class TYPE>
void Array<TYPE>::assign
(
	const Array<TYPE>& other
)
{
	if( &other != this )
	{
		acquireMemory( other.getLength(), false );

		copyObjects( getMemory(), other.getMemory(), other.getLength() );
	}
}


template<class TYPE>
void Array<TYPE>::acquireMemory
(
	dword      newLength,
	const bool isCopied
)
{
	/// clamp to 0 min
	newLength = (newLength >= 0) ? newLength : 0;

	/// only allocate if different length
	if( newLength != length_m )
	{
		/// allocate new storage
		TYPE* pNewMemory = new TYPE[ newLength ];
		if( pNewMemory == 0 )
		{
			throw "Array<>::acquireMemory()\n\ncouldnt get memory\n";
		}

		/// copy elements to new storage
		if( isCopied )
		{
			copyObjects( pNewMemory, pMemory_m,
			             (length_m <= newLength ? length_m : newLength) );
		}

		/// delete old storage and set the members
		delete[] pMemory_m;
		pMemory_m = pNewMemory;
		length_m  = newLength;
	}
}


template<class TYPE>
void Array<TYPE>::copyObjects
(
	TYPE*const       pDestination,
	const TYPE*const pSource,
	const dword      length
)
{
	if( length >= 0 )
	{
		      TYPE* pDestinationCursor = pDestination + length;
		const TYPE* pSourceCursor      = pSource      + length;

		while( pDestinationCursor > pDestination )
		{
			*(--pDestinationCursor) = *(--pSourceCursor);
		}
	}
}


}//namespace




#endif//Array_h
