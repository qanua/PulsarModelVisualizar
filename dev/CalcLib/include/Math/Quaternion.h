#pragma once


/** @brief クォータニオンクラス */
template <class T>
class Quaternion
{
public:
	T	x;	// 要素 : x
	T	y;	// 要素 : y
	T	z;	// 要素 : z
	T	w;	// 要素 : w


public:
	/** @brief デフォルトコンストラクタ */
	Quaternion() :x( 0 ), y( 0 ), z( 0 ), w( 1 ){ }


	/** @brief コンストラクタ */
	template<class U> explicit Quaternion(					 U  n ) : x( (T)(   n ) ), y( (T)(   n ) ), z( (T)(   n ) ), w( (T)(   n ) ){}
	template<class U>		   Quaternion( U nx, U ny, U nz, U nw ) : x( (T)(  nx ) ), y( (T)(  ny ) ), z( (T)(  nz ) ), w( (T)(  nw ) ){}
	template<class U>		   Quaternion( const Quaternion<U>& v ) : x( (T)( v.x ) ), y( (T)( v.y ) ), z( (T)( v.z ) ), w( (T)( v.w ) ){}


	/** @brief デストラクタ */
	~Quaternion()
	{ 
	}


public:
	/** @brief 加算代入演算子 */
	Quaternion<T>& operator += ( const Quaternion<T>& v )
	{
		( *this ).x += v.x;
		( *this ).y += v.y;
		( *this ).z += v.z;
		( *this ).w += v.w;
		return *this;
	}


	/** @brief 減算代入演算子 */
	Quaternion<T>& operator -= ( const Quaternion<T>& v )
	{
		( *this ).x -= v.x;
		( *this ).y -= v.y;
		( *this ).z -= v.z;
		( *this ).w -= v.w;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Quaternion<T>& operator *= ( T n )
	{
		( *this ).x *= n;
		( *this ).y *= n;
		( *this ).z *= n;
		( *this ).w *= n;
		return *this;
	}


	/** @brief 徐算代入演算子 */
	Quaternion<T>& operator /= ( T n )
	{
		( *this ).x /= n;
		( *this ).y /= n;
		( *this ).z /= n;
		( *this ).w /= n;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Quaternion<T>& operator *= ( const Quaternion<T>& v )
	{
		( *this ).x *= v.x;
		( *this ).y *= v.y;
		( *this ).z *= v.z;
		( *this ).w *= v.w;
		return *this;
	}


	/** @brief 内積 */
	T Dot( const Quaternion<T>& v ) const
	{
		return ( ( x * v.x ) + ( y * v.y ) + ( z * v.z ) + ( w * v.w ) );
	}


	/** @brief 外積 */
	Quaternion<T> Cross( const Quaternion<T>& v ) const
	{
		return Quaternion<T>( (  v.w * x + v.z * y - v.y * z + v.x * w ),
							  ( -v.z * x + v.w * y + v.x * z + v.y * w ),
							  (  v.y * x - v.x * y + v.w * z + v.z * w ),
							  ( -v.x * x - v.y * y - v.z * z + v.w * w ) );
	}


	/** @brief ポインタの取得 */
	T* GetPtr() { return &x; }
};


template<class T> Quaternion<T> operator+( const Quaternion<T>&  v ) { return Quaternion<T>( v ); }

template<class T> Quaternion<T> operator+( const Quaternion<T>& v1, const Quaternion<T>& v2 ) { return Quaternion<T>( v1 ) += v2; }
template<class T> Quaternion<T> operator-( const Quaternion<T>& v1, const Quaternion<T>& v2 ) { return Quaternion<T>( v1 ) -= v2; }
template<class T> Quaternion<T> operator*( const Quaternion<T>& v1, const Quaternion<T>& v2 ) { return Quaternion<T>( v1 ) *= v2; }

template<class T> Quaternion<T> operator/( const Quaternion<T>&  v,	const T& n ) { return Quaternion<T>( v ) /= n; }
template<class T> Quaternion<T> operator*( const Quaternion<T>&  v,	const T& n ) { return Quaternion<T>( v ) *= n; }

template<class T> Quaternion<T> operator*( const T& n, const Quaternion<T>& v ) { return Quaternion<T>( n ) *= v; }


typedef Quaternion< float> Quaternionf;
typedef Quaternion<double> Quaterniond;