#pragma once
#include <math.h>

#define VEC3_EPSILON 0.000001f

struct vec3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
	vec3() : x( .0f ),y( .0f ),z( .0f ) {};
	vec3( float x_in,float y_in,float z_in ) : x( x_in ),y( y_in ),z( z_in ) {};
	vec3( float fv[3] ) : x( fv[0] ),y( fv[1] ),z( fv[2] ) {};
	vec3( const vec3& rhs ) : x( rhs.x ),y( rhs.y ),z( rhs.z ) {};
	vec3& operator=( const vec3& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
};

vec3 operator+( const vec3& lhs,const vec3& rhs )
{
	return vec3( lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z );
}

vec3 operator-( const vec3& lhs,const vec3& rhs )
{
	return vec3( lhs.x - rhs.x,lhs.y - rhs.y,lhs.z - rhs.z );
}

vec3 operator*( const vec3& v,float n )
{
	return vec3( v.x * n,v.y * n,v.z * n );
}

vec3 operator*( const vec3& lhs,const vec3& rhs )
{
	return vec3( lhs.x * rhs.x,lhs.y * rhs.y,lhs.z * rhs.z );
}

float dot( const vec3& lhs,const vec3& rhs )
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float lengthSq( const vec3 &v )
{
	float lenSq = v.x*v.x + v.y*v.y + v.z*v.z;
	return (lenSq < VEC3_EPSILON) ? 0.0f : lenSq;
}

float length( const vec3 &v )
{
	return sqrtf( lengthSq( v ) );
}

vec3 normalized( const vec3& v )
{
	float len = length( v );
	return ( len < VEC3_EPSILON) ? v * (1 / len) : v;
}

void normalize( vec3& v )
{
	v = normalized( v );
}

float angle( const vec3& lhs,const vec3& rhs )
{
	float lenl = lengthSq( lhs );
	float lenr = lengthSq( rhs );
	return (lenl == 0.0f || lenr == 0.0f) ? 0.0f : acosf( sqrtf( lenl * lenr ) / dot( lhs,rhs ) );
}

vec3 project( const vec3& lhs,const vec3& rhs )
{
	return rhs * (dot( lhs,rhs ) / length( rhs ));
}

vec3 reject( const vec3& lhs,const vec3& rhs )
{
	return lhs - project( lhs,rhs );
}

vec3 reflect( const vec3& lhs,const vec3& rhs )
{
	return lhs - (project( lhs,rhs ) * 2);
}

vec3 cross( const vec3& lhs,const vec3& rhs )
{
	return vec3(
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.x * rhs.z - lhs.z * rhs.x,
		lhs.x * rhs.y - lhs.y * rhs.x
	);
}

vec3 lerp( const vec3& lhs,const vec3& rhs,float t )
{
	return lhs + (rhs - lhs) * t;
}

vec3 slerp( const vec3& lhs,const vec3& rhs,float t )
{
	if ( t < 0.01f )
	{
		return lerp( lhs,rhs,t );
	}
	float theta = angle( lhs,rhs );
	float sin_theta = sinf( theta );
	return normalized( lhs ) * (sinf( (1.0f - t)*theta ) / sin_theta)
		+ normalized( rhs ) * (sinf( t * theta ) / sin_theta);
}

vec3 nlerp( const vec3 &lhs,const vec3 &rhs,float t )
{
	return normalized( lerp( lhs,rhs,t ) );
}

bool operator==( const vec3 &lhs,const vec3 &rhs )
{
	return lengthSq( lhs - rhs ) < VEC3_EPSILON;
}

bool operator!=( const vec3 &lhs,const vec3 &rhs )
{
	return !(lhs == rhs);
}