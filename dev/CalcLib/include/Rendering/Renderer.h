#pragma once

#include "Handle.h"
#include "Scene/Scene.h"
#include "Primitive/Sphere.h"
#include "Primitive/Cylinder.h"
#include "Primitive/Vertex.h"
#include "Primitive/Arrow.h"

#include <windows.h>

using namespace System::Windows::Forms;
using namespace System::Collections::Generic;


/** @brief 描画クラス */
ref class Renderer
{
private:
	Dictionary<PictureBox^, Handle^>^		m_dicGLWindowHandle;	// 描画画面とハンドル
	HWND									m_hWnd;					// ウィンドウハンドル
	HDC										m_hDC;					// デバイスコンテキストハンドル
	HGLRC									m_hGLRC;				// レンダリングコンテキストハンドル


public:
	/** @brief コンストラクタ */
	Renderer();

	/** @brief デストラクタ */
	~Renderer();


public:
	/**
	 *	@brief 描画画面の設定
	 *
	 *  @param[in]			pct  描画するピクチャボックス
	 *  @return				成功したら true / それ以外は false を返す
	 */
	bool SetupGLWindow( PictureBox^ pct );


private:
	/**
	 *	@brief ピクセルフォーマットの設定
	 *
	 *  @param[in]			hdc  デバイスコンテキストハンドル
	 *  @return				成功したら 1 / それ以外は 0 を返す
	 */
	int SetupPixelFormat( HDC hdc );


	/**
	 *	@brief 描画画面のハンドル設定
	 *
	 *  @param[in]			handle  描画画面のハンドル
	 */
	void SetGLWindowHandle( Handle^ handle );


public:
	/**
	 *	@brief ステートの設定
	 *
	 *  @param[in]			camera  カメラ		
	 */
	void SetupGLStates( const Camera& camera );


	/**
	 *	@brief ビューポートの設定
	 *
	 *  @param[in]			width	ビューポートの幅
	 *  @param[in]			height  ビューポートの高さ
	 *  @param[in]			camera  カメラ
	 */
	void SetupGLViewport( int width, int height, const Camera& camera );


	/**
	 *	@brief ライトの設定
	 *
	 *  @param[in]			light  ライト
	 */
	void SetupGLLighting( const Light& light );


public:
	/**
	 *	@brief 描画開始
	 *
	 *  @param[in]			pct  描画するピクチャボックス
	 *  @return				成功したら true / それ以外は false を返す
	 */
	bool BeginRender( PictureBox^ pct );


	/**
	 *	@brief レンダリング終了
	 *
	 *  @return				成功したら true / それ以外は false を返す
	 */
	bool EndRender();


	/**
	 *	@brief 図形の描画
	 *
	 *  @param[in]			vec_p_shape  描画する図形
	 */
	void RenderShape( const std::vector<Shape*>& vec_p_shape );


	/**
	 *	@brief 複数線分の描画
	 *
	 *  @param[in]			shape  描画する図形
	 */
	void RenderMultipleLine( const Shape& shape );


	/**
	*	@brief 単一線分の描画
	*
	*  @param[in]			shape  描画する図形
	*/
	void RenderSingleLine( const Shape& shape );


	/**
	 *	@brief 線種別 glBegin
	 *
	 *  @param[in]			type  線の種類
	 */
	inline void LineTypeGLBegin( const Shape::LineType type );
};



