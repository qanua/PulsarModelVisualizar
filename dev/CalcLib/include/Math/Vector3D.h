#pragma once

#include <cmath>

/** @brief ３次元ベクトルクラス */
template <class T>
class Vector3D
{
public:
	T	x;	// 要素 : x
	T	y;	// 要素 : y
	T	z;	// 要素 : z


public:
	/** @brief デフォルトコンストラクタ */
	Vector3D() :x( 0 ), z( 0 ), y( 0 ){ }


	/** @brief コンストラクタ */
	template<class U> explicit Vector3D(				 U  n ) : x( (T)(    n ) ), y( (T)(    n ) ), z( (T)(    n ) ){}
	template<class U> explicit Vector3D(       Vector3D<U>* v ) : x( (T)( v->x ) ), y( (T)( v->y ) ), z( (T)( v->z ) ){}
	template<class U>		   Vector3D(	 U nx, U ny, U nz ) : x( (T)(   nx ) ), y( (T)(   ny ) ), z( (T)(   nz ) ){}
	template<class U>		   Vector3D( const Vector3D<U>& v ) : x( (T)(  v.x ) ), y( (T)(  v.y ) ), z( (T)(  v.z ) ){}


	/** @brief デストラクタ */
	~Vector3D()
	{ 
	}


public:
	/** @brief 加算代入演算子 */
	Vector3D<T>& operator += ( const Vector3D<T>& v )
	{
		( *this ).x += v.x;
		( *this ).y += v.y;
		( *this ).z += v.z;
		return *this;
	}


	/** @brief 減算代入演算子 */
	Vector3D<T>& operator -= ( const Vector3D<T>& v )
	{
		( *this ).x -= v.x;
		( *this ).y -= v.y;
		( *this ).z -= v.z;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Vector3D<T>& operator *= ( T n )
	{
		( *this ).x *= n;
		( *this ).y *= n;
		( *this ).z *= n;
		return *this;
	}


	/** @brief 徐算代入演算子 */
	Vector3D<T>& operator /= ( T n )
	{
		( *this ).x /= n;
		( *this ).y /= n;
		( *this ).z /= n;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Vector3D<T>& operator *= ( const Vector3D<T>& v )
	{
		( *this ).x *= v.x;
		( *this ).y *= v.y;
		( *this ).z *= v.z;
		return *this;
	}


	/** @brief 長さ */
	T Length() const
	{
		return (T)( std::sqrt( (double)( ( x * x ) + ( y * y ) + ( z * z ) ) ) );
	}

	/** @brief 内積 */
	T Dot( const Vector3D<T>& v ) const
	{
		return ( ( x * v.x ) + ( y * v.y ) + ( z * v.z ) );
	}

	/** @brief 正規化 */
	Vector3D<T>& Normalize()
	{
		const double length( ( *this ).Length() );
		( *this ).x /= length;
		( *this ).y /= length;
		( *this ).z /= length;
		return *this;
	}

	/** @brief ポインタの取得 */
	T* GetPtr() { return &x; }
};


template<class T> Vector3D<T> operator+( const Vector3D<T>& v ) { return Vector3D<T>( v ); }

template<class T> Vector3D<T> operator+( const Vector3D<T>& v1, const Vector3D<T>& v2 ) { return Vector3D<T>( v1 ) += v2; }
template<class T> Vector3D<T> operator-( const Vector3D<T>& v1, const Vector3D<T>& v2 ) { return Vector3D<T>( v1 ) -= v2; }
template<class T> Vector3D<T> operator*( const Vector3D<T>& v1, const Vector3D<T>& v2 ) { return Vector3D<T>( v1 ) *= v2; }

template<class T> Vector3D<T> operator/( const Vector3D<T>& v, const T& n ) { return Vector3D<T>( v ) /= n; }
template<class T> Vector3D<T> operator*( const Vector3D<T>& v, const T& n ) { return Vector3D<T>( v ) *= n; }

template<class T> Vector3D<T> operator*( const T& n, const Vector3D<T>& v ) { return Vector3D<T>( n ) *= v; }


typedef Vector3D< float> Vector3Df;
typedef Vector3D<double> Vector3Dd;