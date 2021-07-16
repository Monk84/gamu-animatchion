#pragma once

template<typename T>
struct TVec4 
{
	union 
	{
		struct 
		{
			T x;
			T y;
			T z;
			T w;
		};
		T v[4];
	};
	inline TVec4<T>() : x( ( T )0 ),y( ( T )0 ),z( ( T )0 ),w( ( T )0 ) {};
	inline TVec4<T>( T _x,T _y,T _z,T _w ) : x( _x ),y( _y ),z( _z ),w( _w ) {};
	inline TVec4<T>( T* fv ) : x( fv[0] ),y( fv[1] ),z( fv[2] ),w( fv[3] ) {};
};

template<typename T>
TVec4<T> operator+( const TVec4<T> &lhs,const TVec4<T> &rhs )
{
	return TVec4<T>( lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z,lhs.w + rhs.w );
}

template<typename T>
TVec4<T> operator*( const TVec4<T> &v,float t )
{
	return TVec4<T>( v.x*t,v.y*t,v.z*t,v.w*t );
}

template<typename T>
T dot( const TVec4<T> &lhs,const TVec4<T> &rhs )
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;