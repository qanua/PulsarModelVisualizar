#pragma once

#include "Math/Constants.h"

#include <vector>
#include <cmath>


/** @brief 点群クラス */
class Vertex
{
private:
	std::vector<Vector2Dd>						m_vecVector2D;					// 2次元単一線分
	std::vector<Vector3Dd>						m_vecVector3D;					// 3次元単一線分
	std::vector<std::vector<Vector2Dd>>*		m_p_vec_vecVector2D;			// 2次元複数線分
	std::vector<std::vector<Vector3Dd>>*		m_p_vec_vecVector3D;			// 3次元複数線分
	double										m_ThridDimensionSingleValue;	// 単一三次元値
	std::vector<double>							m_ThridDimensionMultipleValue;	// 複数三次元値

public:
	/** @brief コンストラクタ */
	Vertex() : m_ThridDimensionSingleValue(		0.0 )
	{
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
	//void SetSingleLine2D( std::vector<Vector2Dd> vec_vertex ){ m_vecVector2D = vec_vertex; }


	/** @brief 3次元単一線分の設定 */
	//void SetSingleLine3D( std::vector<Vector3Dd> vec_vertex ){ m_vecVector3D = vec_vertex; }


	/** @brief 2次元複数線分の設定 */
	//void SetMultipleLine2D( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex ){ m_p_vec_vecVector2D = &vec_vec_vertex; }


	/** @brief 3次元複数線分の設定 */
	//void SetMultipleLine3D( std::vector<std::vector<Vector3Dd>>& vec_vec_vertex ){ m_p_vec_vecVector3D = &vec_vec_vertex; }


	/** @brief 単一3次元値の設定 */
	//void SetThridDimensionSingleValue( double value ){ m_ThridDimensionSingleValue = value; }


	/** @brief 単一3次元値の設定 */
	//void SetThridDimensionMultipleValue( std::vector<double> value ){ m_ThridDimensionMultipleValue = value; }


public:
	/** @brief 2次元単一線分の取得 */
	//const std::vector<Vector2Dd>& GetSingleLine2D() const{ return m_vecVector2D; }


	/** @brief 3次元単一線分の取得 */
	//const std::vector<Vector3Dd>& GetSingleLine3D() const{ return m_vecVector3D; }


	/** @brief 2次元複数線分の取得 */
	//std::vector<std::vector<Vector2Dd>>& GetMultipleLine2D() { return *m_p_vec_vecVector2D; }


	/** @brief 3次元複数線分の取得 */
	//std::vector<std::vector<Vector3Dd>>& GetMultipleLine3D() { return *m_p_vec_vecVector3D; }


	/** @brief 3次元値の取得 */
	//double GetThridDimensionSingleValue() const { return m_ThridDimensionSingleValue; }


	/** @brief 3次元値の取得 */
	//const std::vector<double>& GetThridDimensionMultipleValue() const { return m_ThridDimensionMultipleValue; }
};