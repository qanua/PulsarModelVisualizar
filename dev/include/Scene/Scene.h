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
	/** @brief シーン種類の列挙 */
	enum class Type
	{
		PULSAR_MODEL = 0,			// パルサーモデル
		POLAR_CAP_NORTH_BEGIN,		// 北ポーラーキャップ始点
		POLAR_CAP_NORTH_END,		// 北ポーラーキャップ終点
		POLAR_CAP_SOUTH_BEGIN,		// 南ポーラーキャップ始点
		POLAR_CAP_SOUTH_END,		// 南ポーラーキャップ終点
		SKY_MAP,					// スカイマップ
		PULSE_PROFILE,				// パルス波形
		COUNT,						// カウント
		NONE = -1					// シーン種類なし
	};


	/** @brief パルサーモデル要素の列挙（描画順） */
	enum class PulsarModel
	{
		ROTATION_AXIS,				// 回転軸
		MAGNETIC_AXIS,				// 磁化軸
		STAR,						// 天体
		MAGNETIC_LINE,				// 磁力線
		LIGTH_CYLINDER,				// 光円柱
		//VIEWING_DIRECTION,		// 視線方向
		COUNT						// カウント
	};


	/** @brief ポーラーキャップ要素の列挙（描画順） */
	enum class PolarCap
	{
		MAGNETIC_LINE,				// 磁力線
		COUNT						// カウント
	};


	/** @brief スカイマップ要素の列挙（描画順） */
	enum class SkyMap
	{
		MAGNETIC_LINE,
		COUNT
	};


	/** @brief パルス要素の列挙（描画順） */
	enum class Pulse
	{
		PULSE,
		COUNT
	};


private:
	Camera*					m_pCamera;				// カメラ
	Light*					m_pLight;				// ライト
	std::vector<Shape*>		m_vec_pShape;			// 図形

	Vector2Dd				m_WindowSize;			// 描画画面サイズ
	Vector2Di				m_MouseDownPosition;	// マウス押下位置


public:
	/** @brief コンストラクタ */
	Scene():m_pCamera( new Camera() )
	,		 m_pLight(	new Light() )
	{
	}


	/** @brief デストラクタ */
	~Scene()
	{
		delete m_pCamera;
		delete m_pLight;
	}


public:
	/**	@brief カメラの設定 */
	void SetCamera( Camera* camera ) { m_pCamera = camera; }


	/** @brief ライトの設定 */
	void SetLight( Light* light ) { m_pLight = light; }


	/** @brief シーン要素の設定 */
	void SetElement( Shape* shape ) { m_vec_pShape.push_back( shape ); }


	/**
	 *  @brief 描画画面サイズの設定
	 *
	 *  @param[in]			width   幅
	 *  @param[in]			height  高さ
	 */
	void SetWindowSize( double width, double height )
	{
		m_WindowSize.x = width;
		m_WindowSize.y = height;
	}


	/**
	 *  @brief マウス押下位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 */
	void SetMouseDownPosition( int x, int y )
	{
		m_MouseDownPosition.x = x;
		m_MouseDownPosition.y = y;
	}


	/**
	 *  @brief マウス移動位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 */
	void SetMouseMovePosition( int x, int y )
	{
		// カメラの回転更新
		m_pCamera->UpdateRotated( ( x - m_MouseDownPosition.x ) / m_WindowSize.x,
								  ( y - m_MouseDownPosition.y ) / m_WindowSize.y );
	}


	/**
	 *  @brief マウス離上位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 */
	void SetMouseUpPosition( int x, int y )
	{
		// カメラ回転終了
		m_pCamera->EndRotated();
	}


public:
	/** @brief カメラの取得 */
	Camera& GetCamera() const { return *m_pCamera; }


	/** @brief ライトの取得 */
	Light& GetLight() const { return *m_pLight; }


	/** @brief シーン要素の取得 */
	std::vector<Shape*> GetElement() const { return m_vec_pShape; }
};