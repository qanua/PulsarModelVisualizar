#pragma once

#include "Shape.h"


/** @brief 円錐クラス */
class Cone : public Shape
{
private:
	double		m_Radius;		// 半径
	double		m_Height;		// 高さ
	int			m_Slices;		// 経度方向の分割数
	int			m_Stacks;		// 緯度方向の分割数


public:
	/** @brief コンストラクタ */
	Cone() : m_Radius( 0.0 )
	,		 m_Height( 0.0 )
	,		 m_Slices(	 0 )
	,		 m_Stacks(	 0 )
	{
		this->SetType( Shape::Type::CONE );
	}


	/** @brief デストラクタ */
	~Cone()
	{
	}


public:
	/** @brief 半径の設定 */
	void SetRadius( double radius ) { m_Radius = radius; }


	/** @brief 高さの設定 */
	void SetHeight( double height ) { m_Height = height; }


	/** @brief 経度方向の分割数の設定 */
	void SetSlices( int slices ) { m_Slices = slices; }


	/** @brief 緯度方向の分割数の設定 */
	void SetStacks( int stacks ) { m_Stacks = stacks; }


public:
	/** @brief 半径の取得 */
	double GetRadius() const { return m_Radius; }


	/** @brief 高さの取得 */
	double GetHeight() const { return m_Height; }


	/** @brief 経度方向の分割数の取得 */
	int GetSlices() const { return m_Slices; }


	/** @brief 緯度方向の分割数の取得 */
	int GetStacks() const { return m_Stacks; }
};