#pragma once

#include <cmath>


/** @class Vector2D
 *
 *  @brief 2次元ベクトルのクラステンプレート
 */
template <class T>
class Vector2D {

public:

	T x;	// 要素 : x
	T y;	// 要素 : y


public:

	/** @brief デフォルトコンストラクタ */
	Vector2D() :x(0), y(0) {}


	/** @brief コンストラクタ */
	template<class U> explicit Vector2D(			     U n) :x((T)(   n)), y((T)(   n )) {}
	template<class U> explicit Vector2D(      Vector2D<U>* v) :x((T)(v->x)), y((T)(v->y )) {}
	template<class U>		   Vector2D(	      U nx, U ny) :x((T)(  nx)), y((T)(  ny )) {}
	template<class U>		   Vector2D(const Vector2D<U>& v) :x((T)( v.x)), y((T)( v.y )) {}


	/** @brief デストラクタ */
	~Vector2D() {}


public:

	/** @brief 加算代入演算子 */
	Vector2D<T>& operator += (const Vector2D<T>& v)
	{
		(*this).x += v.x;
		(*this).y += v.y;
		return *this;
	}


	/** @brief 減算代入演算子 */
	Vector2D<T>& operator -= (const Vector2D<T>& v)
	{
		(*this).x -= v.x;
		(*this).y -= v.y;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Vector2D<T>& operator *= (T n)
	{
		(*this).x *= n;
		(*this).y *= n;
		return *this;
	}


	/** @brief 徐算代入演算子 */
	Vector2D<T>& operator /= (T n)
	{
		(*this).x /= n;
		(*this).y /= n;
		return *this;
	}


	/** @brief 乗算代入演算子 */
	Vector2D<T>& operator *= (const Vector2D<T>& v)
	{
		(*this).x *= v.x;
		(*this).y *= v.y;
		return *this;
	}


	/** @brief 長さ */
	T Length() const
	{
		return (T)(std::sqrt((double)((x * x) + (y * y))));
	}


	/** @brief 内積 */
	T Dot(const Vector2D<T>& v) const
	{
		return ((x * v.x) + (y * v.y));
	}


	/** @brief 正規化 */
	Vector2D<T>& Normalize()
	{
		const double length((*this).Length());
		(*this).x /= length;
		(*this).y /= length;
		return *this;
	}


	/** @brief ポインタの取得 */
	T* GetPtr() { return &x; }
};


template<class T> Vector2D<T> operator+(const Vector2D<T>& v) { return Vector2D<T>(v); }

template<class T> Vector2D<T> operator+(const Vector2D<T>& v1, const Vector2D<T>& v2) { return Vector2D<T>(v1) += v2; }
template<class T> Vector2D<T> operator-(const Vector2D<T>& v1, const Vector2D<T>& v2) { return Vector2D<T>(v1) -= v2; }
template<class T> Vector2D<T> operator*(const Vector2D<T>& v1, const Vector2D<T>& v2) { return Vector2D<T>(v1) *= v2; }

template<class T> Vector2D<T> operator/(const Vector2D<T>& v, const T& n) { return Vector2D<T>(v) /= n; }
template<class T> Vector2D<T> operator*(const Vector2D<T>& v, const T& n) { return Vector2D<T>(v) *= n; }

template<class T> Vector2D<T> operator*(const T& n, const Vector2D<T>& v) { return Vector2D<T>(n) *= v; }

typedef Vector2D<   int> Vector2Di;
typedef Vector2D< float> Vector2Df;
typedef Vector2D<double> Vector2Dd;