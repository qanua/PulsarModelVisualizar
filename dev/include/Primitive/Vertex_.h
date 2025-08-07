#pragma once

#include "Shape.h"
#include "Line.h"
#include "Math/Constants.h"

#include <vector>
#include <cmath>


/** @brief 線分クラス */
class Vertex : public Shape
{
	using line_t  = Shape::LineType;
	using shape_t = Shape::Type;


private:
	std::vector<Line>*							m_p_vecLine2D;			// 2次元点群
	std::vector<Line>*							m_p_vecLine3D;			// 3次元点群
	float										m_Width;				// 幅
	float										m_Length;				// 長さ
	line_t										m_Type;					// 種類


public:
	/** @brief コンストラクタ */
	Vertex( line_t line_type, shape_t shape_type ) : m_Width(		0.0 )
	,												  m_Type( line_type )
	{ 
		m_p_vecLine2D = nullptr;
		m_p_vecLine3D = nullptr;

		this->SetType( shape_type );
	}


	/** @brief デストラクタ */
	~Vertex()
	{
	}


public:
	/** @brief 2次元点群の設定 */
	void SetVertex2D( std::vector<Line>& vec_line )
	{
		m_p_vecLine2D = &vec_line;
	}


	/** @brief 3次元点群の設定 */
	void SetVertex3D( std::vector<Line>& vec_line )
	{
		m_p_vecLine3D = &vec_line;
	}


	/** @brief 幅の設定 */
	void SetWidth( float width ) { m_Width = width; }


	/** @brief 長さの設定 */
	void SetLength( float length ) { m_Length = length; }


public:
	/** @brief 2次元点群の取得 */
	const std::vector<Line>& GetVertex2D() const { return *m_p_vecLine2D; }


	/** @brief 3次元点群の取得 */
	const std::vector<Line>& GetVertex3D() const { return *m_p_vecLine3D; }


	/** @brief 幅の取得 */
	float GetWidth() const { return m_Width; }


	/** @brief 種類の取得 */
	line_t GetType() const { return m_Type; }
};