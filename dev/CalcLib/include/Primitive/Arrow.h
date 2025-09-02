#pragma once

#include "Vertex.h"
#include "Cone.h"

#include <vector>


/** @brief 矢印クラス */
class Arrow : public Shape
{
	using line_t = Shape::LineType;
	using line_count_t = Vertex::CountType;
	using line_vec_t = Vertex::VectorType;


public:
	/** @brief アローヘッド種類の列挙 */
	enum class HeadType
	{
		CONE								// 円錐
	};


private:
	line_t					m_ShaftType;			// シャフト種類
	HeadType				m_HeadType;				// アローヘッド種類
	Vertex*					m_pLine;				// シャフト
	Cone*					m_pCone;				// 円錐のアローヘッド
	std::vector<Vector3Dd>	m_vecHeadPosition;		// アローヘッドの位置


private:
	const int			SLISES = 12;		// 経度方向の分割数
	const int			STACKS = 2;			// 緯度方向の分割数


public:
	/** @brief コンストラクタ */
	Arrow( line_t shaft, HeadType head ) : m_ShaftType(														 shaft )
	,										m_HeadType(														  head )
	,										   m_pLine( new Vertex( line_count_t::SINGLE, line_vec_t::XYZ, shaft ) )
	,										   m_pCone(												    new Cone() )
	{
		this->SetType( Shape::Type::ARROW );

		m_pCone->SetSlices( SLISES );
		m_pCone->SetStacks( STACKS );
	}


	/** @brief デストラクタ */
	~Arrow()
	{
		delete m_pLine;
		delete m_pCone;
	}


public:
	/** @brief シャフト点群の設定 */
	void SetShaftVertex( std::vector<Vector3Dd> vec_vertex )
	{
		m_pLine->SetSingleLine3D( vec_vertex );
		m_vecHeadPosition.emplace_back( vec_vertex.back() );
	}


	/** @brief シャフト幅の設定 */
	void SetShaftWidth( float width ) { m_pLine->SetWidth( width ); }


	/** @brief アローヘッド半径の設定 */
	void SetHeadRadius( double radius ) { m_pCone->SetRadius( radius ); }


	/** @brief アローヘッド高さの設定 */
	void SetHeadHeight( double height ) { m_pCone->SetHeight( height ); }


public:
	/** @brief シャフト点群の取得 */
	const std::vector<Vector3Dd>& GetShaftLine() const { return m_pLine->GetSingleLine3D(); }


	/** @brief シャフト幅の取得 */
	float GetShaftWidth() const { return m_pLine->GetWidth(); }


	/** @brief シャフト種類の取得 */
	line_t GetShaftType() const { return m_ShaftType; }


	/** @brief アローヘッド種類の取得 */
	HeadType GetHeadType() const { return m_HeadType; }


	/** @brief アローヘッド半径の取得 */
	double GetHeadRadius() const { return m_pCone->GetRadius(); }


	/** @brief アローヘッド高さの取得 */
	double GetHeadHeight() const { return m_pCone->GetHeight(); }


	/** @brief アローヘッド経度方向の分割数の取得 */
	int GetHeadSlices() const { return m_pCone->GetSlices(); }


	/** @brief アローヘッド緯度方向の分割数の取得 */
	int GetHeadStacks() const { return m_pCone->GetSlices(); }


	/** @brief アローヘッド位置の取得 */
	const std::vector<Vector3Dd>& GetHeadPosition() const { return m_vecHeadPosition; }
};