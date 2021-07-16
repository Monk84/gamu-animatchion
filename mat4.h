#pragma once
#include "vec4.h"
#include "Vec3.h"
#include <math.h>
#include <iostream>

#define MAT4_EPSILON 0.000001f

struct mat4
{
	union
	{
		float v[16];
		struct
		{
			vec4 right;
			vec4 up;
			vec4 forward;
			vec4 position;
		};
		struct
		{
			//		col0	  col1      col2      col3
			/*row0*/float xx; float xy; float xz; float xw;
			/*row1*/float yx; float yy; float yz; float yw;
			/*row2*/float zx; float zy; float zz; float zw;
			/*row3*/float tx; float ty; float tz; float tw;
		};
		struct
		{
			float c0r0; float c1r0; float c2r0; float c3r0;
			float c0r1; float c1r1; float c2r1; float c3r1;
			float c0r2; float c1r2; float c2r2; float c3r2;
			float c0r3; float c1r3; float c2r3; float c3r3;
		};
		struct
		{
			float r0c0; float r0c1; float r0c2; float r0c3;
			float r1c0; float r1c1; float r1c2; float r1c3;
			float r2c0; float r2c1; float r2c2; float r2c3;
			float r3c0; float r3c1; float r3c2; float r3c3;
		};
	}; //end union
	inline mat4()
		:
		right(),
		up(),
		forward(),
		position()
	{};
	inline mat4( float *fv )
		:
		right( fv ),
		up( fv + 4 ),
		forward( fv + 8 ),
		position( fv + 12 )
	{};
	inline mat4(
		float _00,float _01,float _02,float _03,
		float _10,float _11,float _12,float _13,
		float _20,float _21,float _22,float _23,
		float _30,float _31,float _32,float _33 ) :
		xx( _00 ),xy( _01 ),xz( _02 ),xw( _03 ),
		yx( _10 ),yy( _11 ),yz( _12 ),yw( _13 ),
		zx( _20 ),zy( _21 ),zz( _22 ),zw( _23 ),
		tx( _30 ),ty( _31 ),tz( _32 ),tw( _33 ) {};
	inline mat4( const vec4 &a,const vec4 &b,const vec4 &c,const vec4 &d )
		:
		right( a ),
		up( b ),
		forward( c ),
		position( d )
	{};
};

bool operator==( const mat4 &a,const mat4 &b )
{
	for ( int i = 0; i < 16; ++i )
	{
		if ( abs(a.v[i] - b.v[i]) > MAT4_EPSILON )
		{
			return false;
		}
	}
	return true;
}

bool operator!=( const mat4 &a,const mat4 &b )
{
	return !(a == b);
}

mat4 operator+( const mat4 &a,const mat4 &b )
{
	return mat4( a.right + b.right,a.up + b.up,a.forward + b.forward,a.position + b.position );
}

mat4 operator*( const mat4 &m,float t )
{
	return mat4( m.right * t,m.up * t,m.forward * t,m.position * t );
}

#define M4D(aRow, bCol) \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + \
    a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + \
    a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + \
    a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

mat4 operator*( const mat4 &a,const mat4 &b )
{
	return mat4(
		M4D( 0,0 ),M4D( 1,0 ),M4D( 2,0 ),M4D( 3,0 ),//Col 0
		M4D( 0,1 ),M4D( 1,1 ),M4D( 2,1 ),M4D( 3,1 ),//Col 1
		M4D( 0,2 ),M4D( 1,2 ),M4D( 2,2 ),M4D( 3,2 ),//Col 2
		M4D( 0,3 ),M4D( 1,3 ),M4D( 2,3 ),M4D( 3,3 ) //Col 3
	);
}

vec4 operator*( const mat4 &a,const vec4 &b )
{
	return vec4( M4D( 0,0 ),M4D( 1,0 ),M4D( 2,0 ),M4D( 3,0 ) );
}

#define M4V4D( mRow,x,y,z,w ) \
	x * m.v[ 0 * 4 + mRow ] + \
	y * m.v[ 1 * 4 + mRow ] + \
	z * m.v[ 2 * 4 + mRow ] + \
	w * m.v[ 3 * 4 + mRow ]

vec3 transformVector( const mat4 &m,const vec3 &v )
{
	return vec3(
		M4V4D( 0,v.x,v.y,v.z,0.0f ),
		M4V4D( 1,v.x,v.y,v.z,0.0f ),
		M4V4D( 2,v.x,v.y,v.z,0.0f )
	);
}

vec3 transformPoint( const mat4 &m,const vec3 &v )
{
	return vec3(
		M4V4D( 0,v.x,v.y,v.z,1.0f ),
		M4V4D( 1,v.x,v.y,v.z,1.0f ),
		M4V4D( 2,v.x,v.y,v.z,1.0f )
	);
}

vec3 transformPoint( const mat4 &m,const vec3 &v,float &w )
{
	float _w = w;
	w = M4V4D( 3,v.x,v.y,v.z,1.0f );
	return vec3(
		M4V4D( 0,v.x,v.y,v.z,_w ),
		M4V4D( 1,v.x,v.y,v.z,_w ),
		M4V4D( 2,v.x,v.y,v.z,_w )
	);
}

#define M4SWAP( x,y ) {float t = x; x = y; y = t; }

void transpose( mat4 &m )
{
	M4SWAP( m.xy,m.yx );
	M4SWAP( m.xz,m.zx );
	M4SWAP( m.xw,m.tx );
	M4SWAP( m.yz,m.zy );
	M4SWAP( m.yw,m.ty );
	M4SWAP( m.zw,m.tz );
}

mat4 transposed( const mat4 &m )
{
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
	);
}

#define M4_3X3MINOR( x,c0,c1,c2,r0,r1,r2 ) \
	x[c0*4+r0] * ( x[c1*4+r1] * x[c2*4+r2] - x[c1*4+r2] * x[c2*4+r1] ) \
	- x[c1*4+r0] * ( x[c0*4+r1] * x[c2*4+r2] - x[c2*4+r1] * x[c0*4+r2] ) \
	+ x[c2*4+r0] * ( x[c0*4+r1] * x[c1*4+r2] - x[c0*4+r2] * x[c1*4+r1] )

float determinant( const mat4 &m )
{
	return m.v[0] * M4_3X3MINOR( m.v,1,2,3,1,2,3 )
		- m.v[4] * M4_3X3MINOR( m.v,0,2,3,1,2,3 )
		+ m.v[8] * M4_3X3MINOR( m.v,0,1,3,1,2,3 )
		- m.v[12] * M4_3X3MINOR( m.v,0,1,2,1,2,3 );
}

mat4 adjugate( const mat4 &m )
{
	return mat4(
		//col0
		M4_3X3MINOR( m.v,1,2,3,1,2,3 ),
		-M4_3X3MINOR( m.v,0,2,3,1,2,3 ),
		M4_3X3MINOR( m.v,0,1,3,1,2,3 ),
		-M4_3X3MINOR( m.v,0,1,2,1,2,3 ),
		//col1
		-M4_3X3MINOR( m.v,1,2,3,0,2,3 ),
		M4_3X3MINOR( m.v,0,2,3,0,2,3 ),
		-M4_3X3MINOR( m.v,0,1,3,0,2,3 ),
		M4_3X3MINOR( m.v,0,1,2,0,2,3 ),
		//col2
		M4_3X3MINOR( m.v,1,2,3,0,1,3 ),
		-M4_3X3MINOR( m.v,0,2,3,0,1,3 ),
		M4_3X3MINOR( m.v,0,1,3,0,1,3 ),
		-M4_3X3MINOR( m.v,0,1,2,0,1,3 ),
		//col3
		-M4_3X3MINOR( m.v,1,2,3,0,1,2 ),
		M4_3X3MINOR( m.v,0,2,3,0,1,2 ),
		-M4_3X3MINOR( m.v,0,1,3,0,1,2 ),
		M4_3X3MINOR( m.v,0,1,2,0,1,2 )
	);
}

mat4 inverse( const mat4 &m )
{
	float det = determinant( m );
	if ( det == 0 )
	{
		//std::cout << "inverse: matrix determinant is 0\n";
		return mat4();
	}
	return adjugate( m ) * (1 / det);
}

void invert( mat4 &m )
{
	m = inverse( m );
}

mat4 frustum( float left,float right,float bottom,float top,float n,float f )
{
	if ( left == right || top == bottom || n == f )
	{
		//std::cout << "Invalid frustum\n";
		return mat4();
	}
	return mat4(
		(2.0f * n) / (right - left), 0, 0, 0,
		0, (2.0f * n) / (top - bottom), 0, 0,
		(right + left) / (right - left), (top + bottom) / (top - bottom), (-(f + n) / (f - n)), -1,
		0, 0, (-2.0f * f * n) / (f - n), 0
	);
}
mat4 perspective( float fov,float aspect,float n,float f )
{
	float ymax = n * tanf( fov * 3.14159265359f / 360.0f );
	float xmax = ymax * aspect;
	return frustum( -xmax,xmax,-ymax,ymax,n,f );
}

mat4 ortho( float left,float right,float bottom,float top,
	float n,float f ) {
	if ( left == right || top == bottom || n == f ) {
		return mat4(); // Error
	}
	return mat4(
		2.0f / (right - left),0,0,0,
		0,2.0f / (top - bottom),0,0,
		0,0,-2.0f / (f - n),0,
		-((right + left) / (right - left)),-((top + bottom) / (top - bottom)),-((f + n) / (f - n)),1
	);
}

mat4 lookAt( const vec3 &position,const vec3 &target,const vec3 &up )
{
	vec3 f = normalized( target - position ) * -1.0f;
	vec3 r = cross( up,f ); // Right handed
	if ( r == vec3( 0,0,0 ) ) {
		return mat4(); // Error
	}
	normalize( r );
	vec3 u = normalized( cross( f,r ) ); // Right handed
	vec3 t = vec3(
		-dot( r,position ),
		-dot( u,position ),
		-dot( f,position )
	);
	return mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x,u.x,f.x,0,
		r.y,u.y,f.y,0,
		r.z,u.z,f.z,0,
		t.x,t.y,t.z,1
	);
}


