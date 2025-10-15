#pragma once

#include "Math/Vector3D.h"

#include <vector>


struct MagneticLine
{

public:
	std::vector<Vector3Dd>		m_vecVertex3D;		// 磁力線の3次元点群


	/** @brief コンストラクタ */
	MagneticLine() {};


	/** @brief デストラクタ */
	~MagneticLine() {};
};