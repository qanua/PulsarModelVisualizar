#pragma once

#include "Primitive/Camera.h"
#include "Primitive/Light.h"
#include "Primitive/Shape.h"
#include "Math/Vector2D.h"
#include "Math/Constants.h"

#include <vector>


/** @brief シーンクラス */
class Scene
{
public:
	/** @brief ポーラーキャップ要素の列挙（描画順） */
	enum class PolarCap
	{
		MAGNETIC_LINE,				// 磁力線
		COUNT						// カウント
	};


private:
	std::vector<Shape*>		m_vec_pShape;			// 図形


public:
	/** @brief コンストラクタ */
	Scene() {}


	/** @brief デストラクタ */
	~Scene() {}


public:
	/** @brief シーン要素の設定 */
	void SetElement( Shape* shape ) { m_vec_pShape.push_back( shape ); }


public:
	/** @brief シーン要素の取得 */
	std::vector<Shape*> GetElement() const { return m_vec_pShape; }
};