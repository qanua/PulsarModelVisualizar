#pragma once

/** @brief 色クラス*/
class Colors
{
public:
	float	red;		// Red	 値
	float	green;		// Green 値
	float	blue;		// Blue  値
	float	alpha;		// Alpha 値


public:
	/** @brief コンストラクタ */
	Colors() :red( 0.0 )
	,		green( 0.0 )
	,		 blue( 0.0 )
	,		alpha( 0.0 )
	{
	}


	/** @brief デストラクタ */
	~Colors()
	{
	}


public:
	/** @brief ポインタの取得 */
	float* GetPtr() { return &red; }


public:
	/**
	 *	@brief RGB の設定
	 *
	 *  @param[in]			r  Red  値（   0 ～ 255 ）
	 *  @param[in]			g  Green値（   0 ～ 255 ）
	 *  @param[in]			b  Blue 値（   0 ～ 255 ）
	 */
	void SetRGB( unsigned int r, unsigned int g, unsigned int b )
	{
		red	  = ConvertColor( r );
		green = ConvertColor( g );
		blue  = ConvertColor( b );
	}


private:
	/**
	 *	@brief 255 段階表示の RGB の変換
	 *
	 *  @param[in]			rgb  RGB値（   0 ～ 255 ）
	 *
	 *  @return				RGB値（ 0.0 ～ 1.0 ）
	 */
	float ConvertColor( unsigned int rgb ){ return( (float)rgb / 255.0f ); }
};
