#pragma once

#include "Material/Colors.h"
#include "Math/Vector2D.h"
#include "Math/Vector3D.h"
#include "Math/Matrix4x4.h"


/** @brief 図形クラス */
class Shape
{
public:
	/** @brief 図形種類の列挙 */
	enum class Type
	{
		SPHERE,			// 球
		CYLINDER,		// 円柱
		CONE,			// 円錐
		ARROW,			// 矢印
		VERTEX			// 点群
	};


	/** @brief 線種類の列挙 */
	enum class LineType
	{
		SOLID,			// 実線
		DOTS,			// 点線
		POINT			// 点群
	};


private:
	Type			m_Type;				// 図形の種類
	Colors*			m_pMainColors;		// 図形のメイン色
	Colors*			m_pSubColors;		// 図形のサブ色
	Matrix4x4*		m_pMatrix4x4;		// 4x4変換行列
	double*			m_pMultMatrix;		// 変換行列


public:
	/** @brief コンストラクタ */
	Shape() : m_pMainColors(	new Colors() )
	,		   m_pSubColors(    new Colors() )
	,		   m_pMatrix4x4( new Matrix4x4() )
	{
		// 単位ベクトルで初期化
		m_pMultMatrix = new double[16];
		m_pMatrix4x4->GetIdentityMatrix( m_pMultMatrix );
	}
	

	/** @brief デストラクタ */
	~Shape()
	{ 
		delete	  m_pMainColors;
		delete	  m_pSubColors;
		delete	  m_pMatrix4x4;
		delete [] m_pMultMatrix;
	}


protected:
	/** @brief 種類の設定 */
	void SetType( Type type ) { m_Type = type; }


public:
	/**
	 *	@brief メイン色の設定 
	 *
	 *  @param[in]			red	   R値		 （   0 ～ 255 ）
	 *  @param[in]			green  G値		 （   0 ～ 255 ）
	 *  @param[in]			blue   B値		 （   0 ～ 255 ）
	 *  @param[in]			alpha  アルファ値（ 0.0 ～ 1.0 ）
	 */
	void SetMainColor( unsigned int red, unsigned int green, unsigned int blue, float alpha )
	{ 
		m_pMainColors->SetRGB( red, green, blue );
		m_pMainColors->alpha = alpha;
	};

	/**
	 *	@brief サブ色の設定 
	 *
	 *  @param[in]			red	   R値		 （   0 ～ 255 ）
	 *  @param[in]			green  G値		 （   0 ～ 255 ）
	 *  @param[in]			blue   B値		 （   0 ～ 255 ）
	 *  @param[in]			alpha  アルファ値（ 0.0 ～ 1.0 ）
	 */
	void SetSubColor( unsigned int red, unsigned int green, unsigned int blue, float alpha )
	{ 
		m_pSubColors->SetRGB( red, green, blue );
		m_pSubColors->alpha = alpha;
	};


	/**
	 *	@brief 平行移動の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 */
	void SetTranslated( double x, double y, double z )
	{ 
		m_pMatrix4x4->MultTransMatrix( true, x, y, z, m_pMultMatrix );
	};


	/**
	 *	@brief 回転の設定
	 *
	 *  @param[in]			angle  回転角度 [degree]
	 *  @param[in]			x	   x軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			y	   y軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			z	   z軸まわりの回転率（ 0.0 ～ 1.0 ）
	 */
	void SetRotated( double angle, double x, double y, double z )
	{ 
		m_pMatrix4x4->MultRotMatrix( true, angle, x, y, z, m_pMultMatrix );
	};


public:
	/** @brief 種類の取得 */
	Type GetType()const { return m_Type; }


	/** @brief メイン色の取得 */
	Colors& GetMainColor()const { return *m_pMainColors; }


	/** @brief サブ色の取得 */
	Colors& GetSubColor()const { return *m_pSubColors; }


	/** @brief 変換行列の設定 */
	double* GetMatrix()const { return m_pMultMatrix; }
};