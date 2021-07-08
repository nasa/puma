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


#ifndef Vector3f_h
#define Vector3f_h


#include "hxa7241_general.hpp"




#include "hxa7241_graphics.hpp"
namespace hxa7241_graphics
{


/**
 * yes, its the 3d vector class! pretty much as youd expect.<br/><br/>
 *
 * if you write some 3d graphics software, then you MUST write your own vector
 * class - its the law. so heres mine.
 */
class Vector3f
{
/// standard object services ---------------------------------------------------
public:
	                  Vector3f();
	                  Vector3f( float x, float y, float z );
	explicit          Vector3f( const float xyz[3] );

	                 ~Vector3f();
	                  Vector3f( const Vector3f& );
	        Vector3f& operator=( const Vector3f& );


/// access ---------------------------------------------------------------------
	        Vector3f& setXYZ( float x, float y, float z );
	        Vector3f& setXYZ( const float xyz[3] );

	        void      getXYZ( float& x, float& y, float& z )              const;
	        void      getXYZ( float xyz[3] )                              const;

	        float     getX()                                              const;
	        float     getY()                                              const;
	        float     getZ()                                              const;


/// arithmetic -----------------------------------------------------------------
	        float     total()                                             const;
	        float     average()                                           const;
	        float     smallest()                                          const;
	        float     largest()                                           const;

	        float     length()                                            const;
	        float     dot( const Vector3f& )                              const;
	        float     distance( const Vector3f& )                         const;

	        Vector3f  operator-()                                         const;
	        Vector3f& negate();

	        Vector3f  abs()                                               const;
	        Vector3f& absEq();

	        Vector3f  unitize()                                           const;
	        Vector3f& unitizeEq();

	        Vector3f  cross( const Vector3f& )                            const;
	        Vector3f& crossEq( const Vector3f& );

	        Vector3f  operator+ ( const Vector3f& )                       const;
	        Vector3f& operator+=( const Vector3f& );

	        Vector3f  operator- ( const Vector3f& )                       const;
	        Vector3f& operator-=( const Vector3f& );

	        Vector3f  operator* ( const Vector3f& )                       const;
	        Vector3f& operator*=( const Vector3f& );

	        Vector3f  operator/ ( const Vector3f& )                       const;
	        Vector3f& operator/=( const Vector3f& );

	        Vector3f  operator* ( float )                                 const;
	        Vector3f& operator*=( float );

	        Vector3f  operator/ ( float )                                 const;
	        Vector3f& operator/=( float );

	        Vector3f  operator^ ( float )                                 const;
	        Vector3f& operator^=( float );

	        Vector3f  operator^ ( const Vector3f& )                       const;
	        Vector3f& operator^=( const Vector3f& );

	friend  Vector3f  operator*( float, const Vector3f& );
	friend  Vector3f  operator/( float, const Vector3f& );


/// logical --------------------------------------------------------------------
	        bool      operator==( const Vector3f& )                       const;
	        bool      operator!=( const Vector3f& )                       const;
	        bool      isZero()                                            const;

	        Vector3f  eq( const Vector3f& )                               const;
	        Vector3f  compare( const Vector3f& )                          const;

	        Vector3f  operator>( const Vector3f& )                        const;
	        Vector3f  operator>=( const Vector3f& )                       const;

	        Vector3f  operator<( const Vector3f& )                        const;
	        Vector3f  operator<=( const Vector3f& )                       const;


/// clamps ---------------------------------------------------------------------
	        Vector3f& clampToMinOf( const Vector3f& );
	        Vector3f& clampToMaxOf( const Vector3f& );
	        Vector3f& clampBetween( const Vector3f& min, const Vector3f& max );
	        /**
	         * 0 to almost 1, ie: [0,1).
	         */
	        Vector3f& clamp01();
	        /**
	         * 0 to almost 1, ie: [0,1).
	         */
	        void      clamp01( Vector3f& result )                         const;


/// constants ------------------------------------------------------------------
	static const Vector3f& ZERO();
	static const Vector3f& HALF();
	static const Vector3f& ONE();
	static const Vector3f& EPSILON();
	static const Vector3f& ONE_MINUS_EPSILON();
	static const Vector3f& ALMOST_ONE();
	static const Vector3f& MIN();
	static const Vector3f& MAX();
	static const Vector3f& X();
	static const Vector3f& Y();
	static const Vector3f& Z();


/// fields ---------------------------------------------------------------------
public:
	float x_m;
	float y_m;
	float z_m;
};








/// friends
Vector3f operator*( float, const Vector3f& );
Vector3f operator/( float, const Vector3f& );




/// INLINES ///


/// standard object services ---------------------------------------------------
inline
Vector3f::~Vector3f()
{
}




/// access ---------------------------------------------------------------------
inline
float Vector3f::getX() const
{
	return x_m;
}


inline
float Vector3f::getY() const
{
	return y_m;
}


inline
float Vector3f::getZ() const
{
	return z_m;
}


}//namespace




#endif//Vector3f_h
