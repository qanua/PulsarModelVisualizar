#pragma once

#include <windows.h>

/** @brief ハンドルクラス */
ref class Handle
{
private:
	HWND	m_hWnd;			// ウィンドウハンドル
	HDC		m_hDC;			// デバイスコンテキスト
	HGLRC	m_hGLRC;		// レンダリングコンテキスト

public:
	/** @brief コンストラクタ */
	Handle::Handle( HWND h_Wnd, HDC h_DC, HGLRC h_GLRC ) : m_hWnd(  h_Wnd )
	,														m_hDC(   h_DC )
	,													  m_hGLRC( h_GLRC )
	{
	}


	/** @brief デストラクタ */
	Handle::~Handle()
	{
		if (m_hWnd && m_hDC)
		{
			ReleaseDC( m_hWnd, m_hDC );

			m_hWnd  = nullptr;
			m_hDC   = nullptr;
			m_hGLRC = nullptr;
		}
	}


public:
	/** @brief ウィンドウハンドルの取得 */
	HWND GetHWND(){ return m_hWnd; }


	/** @brief デバイスコンテキストの取得 */
	HDC GetHDC(){ return m_hDC; }


	/** @brief レンダリングコンテキストの取得 */
	HGLRC GetHGLRC(){ return m_hGLRC; }
};

