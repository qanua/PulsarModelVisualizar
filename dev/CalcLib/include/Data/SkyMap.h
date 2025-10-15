#pragma once

#include "Math/Vector2D.h"

#include <vector>


struct SkyMap
{


public:
	std::vector<Vector2Dd>		m_vecVertex2D;	// スカイマップの2次元点群


	/** @brief コンストラクタ */
	SkyMap() {};


	/** @brief デストラクタ */
	~SkyMap() {};
};