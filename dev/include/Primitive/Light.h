#pragma once

#include "Material/Colors.h"
#include "Math/Quaternion.h"


/** @brief ライトクラス */
class Light
{
private:
	Quaternionf*	m_pPosition;		// 位置
	Colors*			m_pDiffuse;			// 拡散光
	Colors*			m_pAmbient;			// 環境光
	Colors*			m_pSpecular;		// 鏡面光


public:
	/** @brief コンストラクタ */
	Light() :m_pPosition( new Quaternionf() )
	,		  m_pDiffuse( 	   new Colors() )
	,		  m_pAmbient( 	   new Colors() )
	,		 m_pSpecular( 	   new Colors() )
	{
	}


	/** @brief デストラクタ */
	~Light()
	{
		delete m_pPosition;
		delete m_pDiffuse;
		delete m_pAmbient;
		delete m_pSpecular;
	}


public:
	/**
	 *	@brief 位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 *  @param[in]			w  1.0 は点光源 / 0.0 は平行光線
	 */
	void SetPosition( float x, float y, float z, float w )
	{
		m_pPosition->x = x;
		m_pPosition->y = y;
		m_pPosition->z = z;
		m_pPosition->w = w;
	}


	/**
	 *	@brief 拡散光の設定
	 *
	 *  @param[in]			red	   R値（ 0 ～ 255 ）
	 *  @param[in]			green  G値（ 0 ～ 255 ）
	 *  @param[in]			blue   B値（ 0 ～ 255 ）
	 */
	void SetDiffuse( int red, int green, int blue ){ m_pDiffuse->SetRGB( red, green, blue ); }


	/**
	 *	@brief 環境光の設定
	 *
	 *  @param[in]			red	   R値（ 0 ～ 255 ）
	 *  @param[in]			green  G値（ 0 ～ 255 ）
	 *  @param[in]			blue   B値（ 0 ～ 255 ）
	 */
	void SetAmbient( int red, int green, int blue ){ m_pAmbient->SetRGB( red, green, blue ); }


	/**
	 *	@brief 鏡面光の設定
	 *
	 *  @param[in]			red	   R値（ 0 ～ 255 ）
	 *  @param[in]			green  G値（ 0 ～ 255 ）
	 *  @param[in]			blue   B値（ 0 ～ 255 ）
	 */
	void SetSpecular( int red, int green, int blue ){ m_pSpecular->SetRGB( red, green, blue ); }


public:
	/** @brief 位置の取得 */
	Quaternionf& GetPosition() const { return *m_pPosition; };


	/** @brief 拡散光の取得 */
	Colors& GetDiffuse() const { return *m_pDiffuse; }


	/** @brief 環境光の取得 */
	Colors& GetAmbient() const { return *m_pAmbient; }


	/** @brief 鏡面光の取得 */
	Colors& GetSpecular() const { return *m_pSpecular; }
};