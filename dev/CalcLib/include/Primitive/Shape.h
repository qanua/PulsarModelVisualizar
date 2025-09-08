#pragma once

#include "Material/Colors.h"
#include "Math/Vector2D.h"
#include "Math/Vector3D.h"
#include "Math/Matrix4x4.h"


/** @brief 図形クラス */
class Shape
{
private:
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
};