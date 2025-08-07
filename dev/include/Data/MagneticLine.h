#pragma once

#include "Math/Vector3D.h"

#include <vector>


struct MagneticLine
{
	/** @brief トレース開始極種類 */
	enum class TraceStartPolarType
	{
		NORTH,
		SOUTH
	};


public:
	double						m_Azimth;			// 方位角 [degree]
	double						m_Polar;			// 極角   [degree]
	std::vector<Vector3Dd>		m_vecVertex3D;		// 磁力線の3次元点群
	TraceStartPolarType			m_TraceType;		// トレース開始極種類


	/** @brief コンストラクタ */
	MagneticLine() :m_Azimth( 0.0 ), m_Polar( 0.0 ){};


	/** @brief デストラクタ */
	~MagneticLine(){};
};