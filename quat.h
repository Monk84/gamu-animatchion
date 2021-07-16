#pragma once

#include "Vec3.h"
#include "mat4.h"

#define QUAT_EPSILON 0.000001f

struct quat
{
	union
	{
		struct
		{
			float x,y,z,w;
		};
		struct
		{
			vec3 vector;
			float scalar;
		};
		float v[4];
	};
	inline quat()
		:
		vector(),
		scalar( 0 )
	{};
	inline quat( float inx,float iny,float inz,float inw )
		:
		x( inx ),
		y( iny ),
		z( inz ),
		w( inw )
	{};
	inline quat( vec3 inv,float ins )
		:
		vector( inv ),
		scalar( ins )
	{};
	inline quat( const quat& q )
		:
		vector( q.vector ),
		scalar( q.scalar )
	{};
};

quat angleAxis( float angle,const vec3 &axis )
{
	float s = sinf( angle * 0.5f );
	vec3 vec = normalized( axis ) * s;
	return { vec.x,vec.y,vec.z,cosf( angle * 0.5f ) };
}

quat fromTo( const vec3 &from,const vec3 &to )
{
	vec3 f = normalized( from );
	vec3 t = normalized( to );
	if ( f == t )
	{
		return quat();
	}
	else if ( f == t * (-1.0f) )
	{
		vec3 ortho = vec3( 1,0,0 );
		if ( fabsf( f.y ) < fabsf( f.x ) )
		{
			ortho = vec3( 0,1,0 );
		}
		if ( fabsf( f.z ) < fabsf( f.y ) && fabsf( f.z ) < fabsf( f.x ) )
		{
			ortho = vec3( 0,0,1 );
		}
		vec3 axis = normalized( cross( f,ortho ) );
		return quat( axis,0.0f );
	}
	vec3 half = normalized( f + t );
	return quat( cross( f,half ),dot( f,half ) );
}

vec3 getAxis( const quat &q )
{
	return normalized( q.vector );
}

float getAngle( const quat &q )
{
	return 2.0f * acosf( q.scalar );
}

quat operator+( const quat &a,const quat &b )
{
	return quat( a.vector + b.vector,a.scalar + b.scalar );
}

quat operator-( const quat &a,const quat &b )
{
	return quat( a.vector - b.vector,a.scalar - b.scalar );
}

quat operator*( const quat &q,float f )
{
	return quat( q.vector * f,q.scalar * f );
}

quat operator-( const quat& q )
{
	return q * (-1.0f);
}

bool operator==( const quat &lhs,const quat &rhs )
{
	return (lhs.vector == rhs.vector && fabsf( lhs.scalar - rhs.scalar ) <= QUAT_EPSILON);
}

bool operator!=( const quat &lhs,const quat &rhs ) 
{
	return !(lhs == rhs);
}

bool sameOrientation( const quat &lhs,const quat &rhs )
{
	return (fabsf( lhs.x - rhs.x ) <= QUAT_EPSILON  &&
		fabsf( lhs.y - rhs.y ) <= QUAT_EPSILON  &&
		fabsf( lhs.z - rhs.z ) <= QUAT_EPSILON  &&
		fabsf( lhs.w - rhs.w ) <= QUAT_EPSILON) ||
		(fabsf( lhs.x + rhs.x ) <= QUAT_EPSILON  &&
			fabsf( lhs.y + rhs.y ) <= QUAT_EPSILON  &&
			fabsf( lhs.z + rhs.z ) <= QUAT_EPSILON  &&
			fabsf( lhs.w + rhs.w ) <= QUAT_EPSILON);
}

float dot( const quat &lhs,const quat &rhs )
{
	return dot( lhs.vector,rhs.vector ) + lhs.scalar*rhs.scalar;
}

float lenSq( const quat &q )
{
	return dot( q,q );
}

float len( const quat &q )
{
	float lensq = lenSq( q );
	return ( lensq < QUAT_EPSILON ) ? 0.0f : sqrtf( lensq );
}

quat normalized( const quat& q )
{
	float l = lenSq( q );
	return ( l < QUAT_EPSILON ) ? quat() : q * (1.0f / sqrtf(l));
}

void normalize( quat& q )
{
	float l = lenSq( q );
	if ( l < QUAT_EPSILON ) return;
	float normalizer = 1 / sqrtf( l );
	q.vector = q.vector * normalizer;
	q.scalar /= normalizer;
}

quat conjugate( const quat& q )
{
	return -q;
}

quat inverse( const quat &q )
{
	float lensq = lenSq( q );
	if ( lensq < QUAT_EPSILON )
	{
		return quat();
	}
	float recip = 1.0f / lensq;
	return quat( q.vector * (-recip),q.scalar * recip );
}

quat operator*( const quat &lhs,const quat &rhs )
{
	quat result;
	result.scalar = rhs.scalar*lhs.scalar - dot( lhs.vector,rhs.vector );
	result.vector = lhs.vector * rhs.scalar - rhs.vector * lhs.scalar + cross( rhs.vector,lhs.vector );
	return result;
}

vec3 operator*( const quat &q,const vec3 &v )
{
	return q.vector * 2.0f * dot( q.vector,v ) +
		v * (q.scalar * q.scalar - dot( q.vector,q.vector )) +
		cross( q.vector,v ) * 2.0f * q.scalar;
}

quat mix( const quat &from,const quat &to,float t )
{
	return from * (1.0f - t) + to * t;
}

quat nlerp( const quat &from,const quat &to,float t )
{
	return normalized( from + (to - from)*t );
}

quat operator^( const quat &q,float f )
{
	float halfAnglePowd = f * 0.5f * getAngle(q);
	vec3 axis = getAxis( q );
	float halfCos = cosf( halfAnglePowd );
	float halfSin = sinf( halfAnglePowd );
	return quat( axis * halfSin,halfCos );
}

quat slerp( const quat &start,const quat &end,float t )
{
	if ( fabsf( dot( start,end ) ) > 1.0f - QUAT_EPSILON )
	{
		return nlerp( start,end,t );
	}
	quat delta = inverse( start ) * end;
	return normalized( (delta ^ t) * start );
}

