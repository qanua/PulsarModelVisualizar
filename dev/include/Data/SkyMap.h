#pragma once

#include "Math/Vector2D.h"

#include <vector>


struct SkyMap
{
	/** @brief トレース開始極種類 */
	enum class TraceStartPolarType
	{
		NORTH,
		SOUTH
	};


public:
	double						m_Azimth;		// スカイマップ線を引いた磁力線の方位角 [degree]
	double						m_Polar;		// スカイマップ線を引いた磁力線の極角   [degree]
	std::vector<Vector2Dd>		m_vecVertex2D;	// スカイマップの2次元点群
	TraceStartPolarType			m_TraceType;	// トレース開始極種類


	/** @brief コンストラクタ */
	SkyMap() :m_Azimth( 0.0 ), m_Polar( 0.0 ){};


	/** @brief デストラクタ */
	~SkyMap(){};
};