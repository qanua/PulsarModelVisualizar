#pragma once

#include "Shape.h"
#include "Math/Constants.h"

#include <vector>
#include <cmath>


/** @brief 点群クラス */
class Vertex : public Shape
{
	using line_t  = Shape::LineType;
	using shape_t = Shape::Type;


public:
	/** @brief 線分本数種類の列挙 */
	enum class CountType
	{
		SINGLE,			// 単一
		MULTIPLE		// 複数
	};


	/** @brief ベクトル種類の列挙 */
	enum class VectorType
	{
		XYZ,				// XYZ-3次元
		XY,					// XY-2次元
		YZ,					// XY-2次元
		XZ					// ZX-2次元
	};


private:
	std::vector<Vector2Dd>						m_vecVector2D;					// 2次元単一線分
	std::vector<Vector3Dd>						m_vecVector3D;					// 3次元単一線分
	std::vector<std::vector<Vector2Dd>>*		m_p_vec_vecVector2D;			// 2次元複数線分
	std::vector<std::vector<Vector3Dd>>*		m_p_vec_vecVector3D;			// 3次元複数線分
	CountType									m_CountType;					// 線分本数
	VectorType									m_VectorType;					// ベクトル種類
	line_t										m_LineType;						// 線分の種類
	float										m_Width;						// 幅
	float										m_Length;						// 長さ
	double										m_ThridDimensionSingleValue;	// 単一三次元値
	std::vector<double>							m_ThridDimensionMultipleValue;	// 複数三次元値

public:
	/** @brief コンストラクタ */
	Vertex( CountType count_type, VectorType vec_type, line_t line_type ) : m_CountType( count_type )
	,																	   m_VectorType(   vec_type )
	,																		 m_LineType(  line_type )
	,																		    m_Width(		0.0 )
	,																		   m_Length(		0.0 )
	,														m_ThridDimensionSingleValue(		0.0 )
	{
		this->SetType( shape_t::VERTEX );
		m_p_vec_vecVector2D = new std::vector<std::vector<Vector2Dd>>;
		m_p_vec_vecVector3D = new std::vector<std::vector<Vector3Dd>>;
	}


	/** @brief デストラクタ */
	~Vertex()
	{
		delete[] m_p_vec_vecVector2D;
		delete[] m_p_vec_vecVector3D;
	}


public:
	/** @brief 2次元単一線分の設定 */
	void SetSingleLine2D( std::vector<Vector2Dd> vec_vertex ){ m_vecVector2D = vec_vertex; }


	/** @brief 3次元単一線分の設定 */
	void SetSingleLine3D( std::vector<Vector3Dd> vec_vertex ){ m_vecVector3D = vec_vertex; }


	/** @brief 2次元複数線分の設定 */
	void SetMultipleLine2D( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex ){ m_p_vec_vecVector2D = &vec_vec_vertex; }


	/** @brief 3次元複数線分の設定 */
	void SetMultipleLine3D( std::vector<std::vector<Vector3Dd>>& vec_vec_vertex ){ m_p_vec_vecVector3D = &vec_vec_vertex; }


	/** @brief 幅の設定 */
	void SetWidth( float width ) { m_Width = width; }


	/** @brief 長さの設定 */
	void SetLength( float length ) { m_Length = length; }


	/** @brief 2次元傾きの設定
	 *
	 *  @attention			angle の単位は [degree]
	 *  @attention			z軸回転
	 *  @attention			傾きは時計回り
	 */
	void SetAngle2D( double angle )
	{
		const double theta( angle * RADIAN );
		const double r( m_Length * 0.5 );
		const double x( r * sin( theta ) );
		const double y( r * cos( theta ) );

		std::vector<Vector2Dd> vec_line;
		vec_line.push_back( Vector2Dd(  x,  y ) );
		vec_line.push_back( Vector2Dd( -x, -y ) );

		SetSingleLine2D( vec_line );
	}


	/** @brief 3次元傾きの設定
	 *
	 *  @attention			angle の単位は [degree]
	 *  @attention			y軸回転
	 *  @attention			傾きは時計回り
	 */
	void SetAngle3D( double angle )
	{
		const double theta( angle * RADIAN );
		const double r( m_Length * 0.5 );
		const double x( r * sin( theta ) );
		const double z( r * cos( theta ) );

		std::vector<Vector3Dd> vec_line;
		vec_line.push_back( Vector3Dd(  x, 0.0,  z ) );
		vec_line.push_back( Vector3Dd( -x, 0.0, -z ) );

		SetSingleLine3D( vec_line );
	}


	/** @brief 単一3次元値の設定 */
	void SetThridDimensionSingleValue( double value ){ m_ThridDimensionSingleValue = value; }


	/** @brief 単一3次元値の設定 */
	void SetThridDimensionMultipleValue( std::vector<double> value ){ m_ThridDimensionMultipleValue = value; }


public:
	/** @brief 2次元単一線分の取得 */
	const std::vector<Vector2Dd>& GetSingleLine2D() const{ return m_vecVector2D; }


	/** @brief 3次元単一線分の取得 */
	const std::vector<Vector3Dd>& GetSingleLine3D() const{ return m_vecVector3D; }


	/** @brief 2次元複数線分の取得 */
	std::vector<std::vector<Vector2Dd>>& GetMultipleLine2D() { return *m_p_vec_vecVector2D; }


	/** @brief 3次元複数線分の取得 */
	std::vector<std::vector<Vector3Dd>>& GetMultipleLine3D() { return *m_p_vec_vecVector3D; }


	/** @brief 幅の取得 */
	float GetWidth() const { return m_Width; }


	/** @brief 線分本数種類の取得 */
	CountType GetCountType() const { return m_CountType; }


	/** @brief ベクトル種類の取得 */
	VectorType GetVectorType() const { return m_VectorType; }


	/** @brief 線分種類の取得 */
	line_t GetLineType() const { return m_LineType; }


	/** @brief 3次元値の取得 */
	double GetThridDimensionSingleValue() const { return m_ThridDimensionSingleValue; }


	/** @brief 3次元値の取得 */
	const std::vector<double>& GetThridDimensionMultipleValue() const { return m_ThridDimensionMultipleValue; }
};