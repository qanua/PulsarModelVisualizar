#pragma once

#include "Shape.h"


/** @brief 球クラス */
class Sphere : public Shape
{
private:
	double		m_Radius;		// 半径
	int			m_Slices;		// 経度方向の分割数
	int			m_Stacks;		// 緯度方向の分割数


public:
	/** @brief コンストラクタ */
	Sphere() : m_Radius( 0.0 )
	,		   m_Slices(   0 )
	,		   m_Stacks(   0 )
	{ 
		this->SetType( Shape::Type::SPHERE );
	}


	/** @brief デストラクタ */
	~Sphere()
	{
	}


public:
	/** @brief 半径の設定 */
	void SetRadius( double radius ) { m_Radius = radius; }


	/** @brief 経度方向の分割数の設定 */
	void SetSlices( int slices ) { m_Slices = slices; }


	/** @brief 緯度方向の分割数の設定 */
	void SetStacks( int stacks ) { m_Stacks = stacks; }


public:
	/** @brief 半径の取得 */
	double GetRadius() const { return m_Radius; }


	/** @brief 経度方向の分割数の取得 */
	int GetSlices() const { return m_Slices; }


	/** @brief 緯度方向の分割数の取得 */
	int GetStacks() const { return m_Stacks; }
};