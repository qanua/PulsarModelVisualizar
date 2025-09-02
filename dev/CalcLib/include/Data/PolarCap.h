#pragma once

#include "Math/Vector3D.h"

#include <vector>


struct PolarCap
{
	/** @brief トレース開始極種類 */
	enum class TraceStartPolarType
	{
		NORTH,
		SOUTH
	};


public:
	double						m_Azimth;		// LCFLの方位角 [degree]
	double						m_Polar;		// LCFLの極角   [degree]
	Vector3Dd					m_PolarCap;		// LCFLの根元位置
	TraceStartPolarType			m_TraceType;	// トレース開始極種類


	/** @brief コンストラクタ */
	PolarCap() :m_Azimth( 0.0 ), m_Polar( 0.0 ){};


	/** @brief デストラクタ */
	~PolarCap(){};
};