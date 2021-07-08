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


#include <math.h>

#include "Vector3f.hpp"


using namespace hxa7241_graphics;




/// standard object services ---------------------------------------------------
Vector3f::Vector3f()
 :	x_m(),
	y_m(),
	z_m()
{
}


Vector3f::Vector3f
(
	const float x,
	const float y,
	const float z
)
 :	x_m( x )
 ,	y_m( y )
 ,	z_m( z )
{
}


Vector3f::Vector3f
(
	const float xyz[3]
)
 :	x_m( xyz[0] )
 ,	y_m( xyz[1] )
 ,	z_m( xyz[2] )
{
}


Vector3f::Vector3f
(
	const Vector3f& v
)
 :	x_m( v.x_m )
 ,	y_m( v.y_m )
 ,	z_m( v.z_m )
{
}


Vector3f& Vector3f::operator=
(
	const Vector3f& v
)
{
	x_m = v.x_m;
	y_m = v.y_m;
	z_m = v.z_m;

	return *this;
}




/// access ---------------------------------------------------------------------
Vector3f& Vector3f::setXYZ
(
	const float x,
	const float y,
	const float z
)
{
	x_m = x;
	y_m = y;
	z_m = z;

	return *this;
}


Vector3f& Vector3f::setXYZ
(
	const float xyz[3]
)
{
	x_m = xyz[0];
	y_m = xyz[1];
	z_m = xyz[2];

	return *this;
}


void Vector3f::getXYZ
(
	float& x,
	float& y,
	float& z
) const
{
	x = x_m;
	y = y_m;
	z = z_m;
}


void Vector3f::getXYZ
(
	float xyz[3]
) const
{
	xyz[0] = x_m;
	xyz[1] = y_m;
	xyz[2] = z_m;
}




/// arithmetic -----------------------------------------------------------------
float Vector3f::total() const
{
	return x_m + y_m + z_m;
}


float Vector3f::average() const
{
	static const float ONE_OVER_3 = 1.0f / 3.0f;

	return (x_m + y_m + z_m) * ONE_OVER_3;
}


float Vector3f::smallest() const
{
	float smallest;
	smallest = x_m      <= y_m ? x_m      : y_m;
	smallest = smallest <= z_m ? smallest : z_m;

	return smallest;
}


float Vector3f::largest() const
{
	float largest;
	largest = x_m     >= y_m ? x_m     : y_m;
	largest = largest >= z_m ? largest : z_m;

	return largest;
}


float Vector3f::length() const
{
	return float(::sqrt( (x_m * x_m) +
	                     (y_m * y_m) +
	                     (z_m * z_m) ));
}


float Vector3f::dot
(
	const Vector3f& v
) const
{
	return (x_m * v.x_m) +
	       (y_m * v.y_m) +
	       (z_m * v.z_m);
}


float Vector3f::distance
(
	const Vector3f& v
) const
{
	const float xDif = x_m - v.x_m;
	const float yDif = y_m - v.y_m;
	const float zDif = z_m - v.z_m;

	return float(::sqrt( (xDif * xDif) +
	                     (yDif * yDif) +
	                     (zDif * zDif) ));

//	Vector3f dif( *this - v );   // Vector3f dif( *this );   dif -= v;
//
//	return float(::sqrt( dif.Dot( dif ) ));
}


Vector3f Vector3f::operator-() const
{
	return Vector3f( -x_m, -y_m, -z_m );
}


Vector3f& Vector3f::negate()
{
	x_m = -x_m;
	y_m = -y_m;
	z_m = -z_m;

	return *this;
}


Vector3f Vector3f::abs() const
{
	return Vector3f( *this ).absEq();
}


Vector3f& Vector3f::absEq()
{
	if( x_m < 0.0f )
	{
		x_m = -x_m;
	}
	if( y_m < 0.0f )
	{
		y_m = -y_m;
	}
	if( z_m < 0.0f )
	{
		z_m = -z_m;
	}

	return *this;
}


Vector3f Vector3f::unitize() const
{
	const float length = float(::sqrt( (x_m * x_m) +
	                                   (y_m * y_m) +
	                                   (z_m * z_m) ));
	const float oneOverLength = length != 0.0f ? 1.0f / length : 0.0f;

	return Vector3f( x_m * oneOverLength,
					 y_m * oneOverLength,
					 z_m * oneOverLength );
}


Vector3f& Vector3f::unitizeEq()
{
	const float length = float(::sqrt( (x_m * x_m) +
	                                   (y_m * y_m) +
	                                   (z_m * z_m) ));
	const float oneOverLength = length != 0.0f ? 1.0f / length : 0.0f;

	x_m *= oneOverLength;
	y_m *= oneOverLength;
	z_m *= oneOverLength;

	return *this;
}


Vector3f Vector3f::cross
(
	const Vector3f& v
) const
{
	return Vector3f( (y_m * v.z_m) - (z_m * v.y_m),
	                 (z_m * v.x_m) - (x_m * v.z_m),
	                 (x_m * v.y_m) - (y_m * v.x_m) );
}


Vector3f& Vector3f::crossEq
(
	const Vector3f& v
)
{
	const float x = (y_m * v.z_m) - (z_m * v.y_m);
	const float y = (z_m * v.x_m) - (x_m * v.z_m);
	const float z = (x_m * v.y_m) - (y_m * v.x_m);

	x_m = x;
	y_m = y;
	z_m = z;

	return *this;
}




/// plus minus -----------------------------------------------------------------
Vector3f Vector3f::operator+
(
	const Vector3f& v
) const
{
	return Vector3f( x_m + v.x_m,
	                 y_m + v.y_m,
	                 z_m + v.z_m );
}


Vector3f& Vector3f::operator+=
(
	const Vector3f& v
)
{
	x_m += v.x_m;
	y_m += v.y_m;
	z_m += v.z_m;

	return *this;
}


Vector3f Vector3f::operator-
(
	const Vector3f& v
) const
{
	return Vector3f( x_m - v.x_m,
	                 y_m - v.y_m,
	                 z_m - v.z_m );
}


Vector3f& Vector3f::operator-=
(
	const Vector3f& v
)
{
	x_m -= v.x_m;
	y_m -= v.y_m;
	z_m -= v.z_m;

	return *this;
}




/// mult div -------------------------------------------------------------------
Vector3f Vector3f::operator*
(
	const Vector3f& v
) const
{
	return Vector3f( x_m * v.x_m,
	                 y_m * v.y_m,
	                 z_m * v.z_m );
}


Vector3f& Vector3f::operator*=
(
	const Vector3f& v
)
{
	x_m *= v.x_m;
	y_m *= v.y_m;
	z_m *= v.z_m;

	return *this;
}


Vector3f Vector3f::operator/
(
	const Vector3f& v
) const
{
	return Vector3f( x_m / v.x_m,
	                 y_m / v.y_m,
	                 z_m / v.z_m );
}


Vector3f& Vector3f::operator/=
(
	const Vector3f& v
)
{
	x_m /= v.x_m;
	y_m /= v.y_m;
	z_m /= v.z_m;

	return *this;
}




Vector3f Vector3f::operator*
(
	const float f
) const
{
	return Vector3f( x_m * f,
	                 y_m * f,
	                 z_m * f );
}


Vector3f& Vector3f::operator*=
(
	const float f
)
{
	x_m *= f;
	y_m *= f;
	z_m *= f;

	return *this;
}


Vector3f Vector3f::operator/
(
	const float f
) const
{
	const float oneOverF = 1.0f / f;

	return Vector3f( x_m * oneOverF,
	                 y_m * oneOverF,
	                 z_m * oneOverF );
}


Vector3f& Vector3f::operator/=
(
	const float f
)
{
	const float oneOverF = 1.0f / f;

	x_m *= oneOverF;
	y_m *= oneOverF;
	z_m *= oneOverF;

	return *this;
}




/// pow ------------------------------------------------------------------------
Vector3f Vector3f::operator^
(
	const float f
) const
{
	return  Vector3f( *this ) ^= f;
}


Vector3f& Vector3f::operator^=
(
	const float f
)
{
	x_m = float(::pow( x_m, f ));
	y_m = float(::pow( y_m, f ));
	z_m = float(::pow( z_m, f ));

	return *this;
}


Vector3f Vector3f::operator^
(
	const Vector3f& v
) const
{
	return  Vector3f( *this ) ^= v;
}


Vector3f& Vector3f::operator^=
(
	const Vector3f& v
)
{
	x_m = float(::pow( x_m, v.x_m ));
	y_m = float(::pow( y_m, v.y_m ));
	z_m = float(::pow( z_m, v.z_m ));

	return *this;
}




/// friends --------------------------------------------------------------------
Vector3f hxa7241_graphics::operator*
(
	const float     f,
	const Vector3f& v
)
{
	return Vector3f( f * v.x_m,
	                 f * v.y_m,
	                 f * v.z_m );
}




Vector3f hxa7241_graphics::operator/
(
	const float     f,
	const Vector3f& v
)
{
	return Vector3f( f / v.x_m,
	                 f / v.y_m,
	                 f / v.z_m );
}




/// logical --------------------------------------------------------------------
bool Vector3f::operator==
(
	const Vector3f& v
) const
{
	return (x_m == v.x_m) &
	       (y_m == v.y_m) &
	       (z_m == v.z_m);
}


bool Vector3f::operator!=
(
	const Vector3f& v
) const
{
	return (x_m != v.x_m) |
	       (y_m != v.y_m) |
	       (z_m != v.z_m);
}


bool Vector3f::isZero() const
{
	return bool( (x_m == 0.0f) & (y_m == 0.0f) & (z_m == 0.0f) );
}


Vector3f Vector3f::eq
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m == v.x_m),
	                 float(y_m == v.y_m),
	                 float(z_m == v.z_m) );
}


Vector3f Vector3f::compare
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m > v.x_m ? +1 : (x_m == v.x_m ? 0 : -1)),
	                 float(y_m > v.y_m ? +1 : (y_m == v.y_m ? 0 : -1)),
	                 float(z_m > v.z_m ? +1 : (z_m == v.z_m ? 0 : -1)) );
}


Vector3f Vector3f::operator>
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m > v.x_m),
	                 float(y_m > v.y_m),
	                 float(z_m > v.z_m) );
}


Vector3f Vector3f::operator>=
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m >= v.x_m),
	                 float(y_m >= v.y_m),
	                 float(z_m >= v.z_m) );
}


Vector3f Vector3f::operator<
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m < v.x_m),
	                 float(y_m < v.y_m),
	                 float(z_m < v.z_m) );
}


Vector3f Vector3f::operator<=
(
	const Vector3f& v
) const
{
	return Vector3f( float(x_m <= v.x_m),
	                 float(y_m <= v.y_m),
	                 float(z_m <= v.z_m) );
}




/// clamps ---------------------------------------------------------------------
Vector3f& Vector3f::clampToMinOf
(
	const Vector3f& min
)
{
	if( x_m < min.x_m )
	{
		x_m = min.x_m;
	}
	if( y_m < min.y_m )
	{
		y_m = min.y_m;
	}
	if( z_m < min.z_m )
	{
		z_m = min.z_m;
	}

	return *this;
}


Vector3f& Vector3f::clampToMaxOf
(
	const Vector3f& max
)
{
	if( x_m > max.x_m )
	{
		x_m = max.x_m;
	}
	if( y_m > max.y_m )
	{
		y_m = max.y_m;
	}
	if( z_m > max.z_m )
	{
		z_m = max.z_m;
	}

	return *this;
}


Vector3f& Vector3f::clampBetween
(
	const Vector3f& min,
	const Vector3f& max
)
{
	if( x_m > max.x_m )
		x_m = max.x_m;
	else
	if( x_m < min.x_m )
		x_m = min.x_m;

	if( y_m > max.y_m )
		y_m = max.y_m;
	else
	if( y_m < min.y_m )
		y_m = min.y_m;

	if( z_m > max.z_m )
		z_m = max.z_m;
	else
	if( z_m < min.z_m )
		z_m = min.z_m;

	return *this;
}


Vector3f& Vector3f::clamp01()   /// [0,1)  0 to almost 1
{
	if( x_m >= 1.0f )
		x_m  = FLOAT_ALMOST_ONE;
	else
	if( x_m < 0.0f )
		x_m = 0.0f;

	if( y_m >= 1.0f )
		y_m  = FLOAT_ALMOST_ONE;
	else
	if( y_m < 0.0f )
		y_m = 0.0f;

	if( z_m >= 1.0f )
		z_m  = FLOAT_ALMOST_ONE;
	else
	if( z_m < 0.0f )
		z_m = 0.0f;

	return *this;
}


void Vector3f::clamp01
(
	Vector3f& result
) const   /// [0,1)  0 to almost 1
{
	if( x_m >= 1.0f )
		result.x_m = FLOAT_ALMOST_ONE;
	else
	if( x_m < 0.0f )
		result.x_m = 0.0f;
	else
		result.x_m = x_m;

	if( y_m >= 1.0f )
		result.y_m = FLOAT_ALMOST_ONE;
	else
	if( y_m < 0.0f )
		result.y_m = 0.0f;
	else
		result.y_m = y_m;

	if( z_m >= 1.0f )
		result.z_m = FLOAT_ALMOST_ONE;
	else
	if( z_m < 0.0f )
		result.z_m = 0.0f;
	else
		result.z_m = z_m;

//	result = *this;
//	result.clamp01();
}




/// constants ------------------------------------------------------------------
const Vector3f& Vector3f::ZERO()
{
	static const Vector3f k( 0.0f, 0.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::HALF()
{
	static const Vector3f k( 0.5f, 0.5f, 0.5f );
	return k;
}


const Vector3f& Vector3f::ONE()
{
	static const Vector3f k( 1.0f, 1.0f, 1.0f );
	return k;
}


const Vector3f& Vector3f::EPSILON()
{
	static const Vector3f k( FLOAT_EPSILON, FLOAT_EPSILON, FLOAT_EPSILON );
	return k;
}


const Vector3f& Vector3f::ONE_MINUS_EPSILON()
{
	static const Vector3f k( FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE );
	return k;
}


const Vector3f& Vector3f::ALMOST_ONE()
{
	static const Vector3f k( FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE,
	                         FLOAT_ALMOST_ONE );
	return k;
}


const Vector3f& Vector3f::MIN()
{
	static const Vector3f k( FLOAT_MIN_NEG, FLOAT_MIN_NEG, FLOAT_MIN_NEG );
	return k;
}


const Vector3f& Vector3f::MAX()
{
	static const Vector3f k( FLOAT_MAX, FLOAT_MAX, FLOAT_MAX );
	return k;
}


const Vector3f& Vector3f::X()
{
	static const Vector3f k( 1.0f, 0.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::Y()
{
	static const Vector3f k( 0.0f, 1.0f, 0.0f );
	return k;
}


const Vector3f& Vector3f::Z()
{
	static const Vector3f k( 0.0f, 0.0f, 1.0f );
	return k;
}
