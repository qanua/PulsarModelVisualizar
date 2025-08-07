#pragma once

#include "Math/Vector2D.h"

#include <vector>


struct Pulse
{
public:
	double						m_ViewingAngle;		// 視線方向 [degree]
	double						m_DisplayY;			// y軸方向のパルス波形表示位置
	std::vector<Vector2Dd>		m_vecVertex2D;		// パルス波形の2次元点群


	/** @brief コンストラクタ */
	Pulse() :m_ViewingAngle( 0.0 ), m_DisplayY( 0.0 )
	{
		m_vecVertex2D.reserve( 360 );
	};


	/** @brief デストラクタ */
	~Pulse(){};
};