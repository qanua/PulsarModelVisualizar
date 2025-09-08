#pragma once

#include "Data/Pulsar.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"
#include "Math/Constants.h"

#include <windows.h>
#include <vector>


/** @brief パルス計算クラス */
class PulseCalculator
{
private:
	// 角度情報構造体
	struct Angle
	{
	public:
		double					azimth;			// 方位角       [rad]
		double					polar;			// 極角         [rad]
		double					inclination;	// 磁化軸の傾き [rad]


	public:
		Angle() :azimth( 0.0 ), polar( 0.0 ), inclination( 0.0 ){};
		~Angle(){};
	};


	// パルス情報構造体
	struct PulseInfo
	{
		/** @brief ポーラーキャップトレース種類の列挙 */
		enum class PolarCapTraceType
		{
			FROM_NORTH = 0,
			FROM_SOUTH,
			COUNT
		};


		/** @brief 磁力線状態の列挙 */
		enum class MagneticLineStatus
		{
			CLOSE = 0,
			OPEN  = 1
		};


	public:
		//PolarCapTraceType		polar_cap_trace_type;	// ポーラーキャップトレース種類
		MagneticLineStatus		magnetic_line_status;	// 磁力線状態

		Vector3Dd				magnetic_field_pos;		// 磁場計算位置

		MagneticLine			lcfl;					// 1本のLCFL
		SkyMap					skymap;					// 1本のスカイマップ線
		std::vector<Pulse>		vec_pulse;				// 1本のスカイマップ線に対応するパルス波形


	private:
		bool					is_lcfl;				// LCFL格納フラグ
		bool					is_skymap;				// スカイマップ格納フラグ
		bool					is_pulse;				// パルス格納フラグ


	public:
		PulseInfo() :is_lcfl( false )
		,		   is_skymap( false )
		,			is_pulse( false ){};

		~PulseInfo(){};


	public:
		/** @brief フラグの設定 */
		void SetFlag( bool lcfl_flag, bool skymap_flag, bool pulse_flag )
		{
			is_lcfl	  = lcfl_flag;
			is_skymap = skymap_flag;
			is_pulse  = pulse_flag;
		}


	public:
		/** @brief LCFLフラグの取得 */
		bool IsLCFL(){ return is_lcfl; };


		/** @brief スカイマップフラグの取得 */
		bool IsSkyMap(){ return is_skymap; };


		/** @brief パルスフラグの取得 */
		bool IsPulse(){ return is_pulse; };
	};


private:
	CRITICAL_SECTION	m_CriticalSection;							// クリティカルセクション
	Vector3Dd			m_MagneticMoment;							// 磁気モーメント


private:
	const double		POLAR_ANGLE				  = 0.0;			// 磁極付近の極角
	const double		EQUATOR_ANGLE			  = 80.0;			// 赤道付近の極角
	const int			DICHOTOMY_STEP			  = 20;				// 二分法のステップ数
	const double		STEP_LENGTH				  = STAR_RADIUS;	// 積分ステップの長さ
	const double		DISTANCE_PULSAR_TO_EARTH  = 1.0;			// パルサーから地球までの距離
	const double		OUTER_GAP_LAYER_THICKNESS = 0.02;			// Outer Gap の層の厚さ
	const int			OUTER_GAP_LAYER_COUNT	  = 10;				// Outer Gap の層の数


public:
	/** @brief コンストラクタ */
	PulseCalculator();


	/** @brief デストラクタ */
	~PulseCalculator();


public:
	/**
	 *	@brief パルス情報の取得
	 *
	 *  @param[in/out]		pulsar			パルサー情報
	 */
	void GetPulsarInfo( Pulsar& pulsar );


	/**
	 *	@brief パルス計算
	 *
	 *  @param[in/out]		pulsar			パルサー情報
	 *  @param[in]			phi				LCFL の方位角 [degree]
	 */
	void CalculatePulse( Pulsar& pulsar, double phi );


private:
	/**
	 *	@brief パルスの取得
	 *
	 *  @param[in]			angle  角度情報
	 *  @param[in/out]		info   パルス情報
	 */
	void GetPulse( Angle& angle, PulseInfo& info );


	/** @brief LCFLの存在確認
	 *
	 *  @param[in]			angle		 角度情報
	 *  @param[in]			begin_theta  LCFLを挟む磁極付近の磁力線の極角 [degree]
	 *  @param[in]			end_theta	 LCFLを挟む赤道付近の磁力線の極角 [degree]
	 *
	 *  @return				true：ある / false：ない
	 */
	bool CheckExistLCFL( Angle& angle, double open_angle, double close_angle );


	/** @brief パルス情報の取得
	 *
	 *  @param[in/out]		info  パルス情報
	 */
	void GetPulseInfo( PulseInfo& info );


	/** @brief 磁場の取得
	*
	*  @param[in]			t   時間（依存している場合）
	*  @param[in]			v   位置
	*  @param[in]			Bv  磁場
	*
	*  @return				true：成功 / false：失敗
	*/
	bool GetMagneticField( double /*t*/, Vector3Dd v, Vector3Dd& Bv );


	/** @brief スカイマップの生成
	 *
	 *  @param[in]			pos		    磁力線の位置
	 *  @param[in]			Bv		    磁場
	 *  @param[out]			vec_skymap  スカイマップの点群データ
	 */
	void CreateSkyMap( const Vector3Dd& pos, const Vector3Dd& Bv, std::vector<Vector2Dd>& vec_skymap );


	/** @brief パルスの生成
	 *
	 *  @param[in]			prev_Bv	   過去の磁場
	 *  @param[in]			curr_Bv	   現在の磁場
	 *  @param[in]			skymap     スカイマップの点群データ
	 *  @param[out]			vec_pulse  パルスの点群データ
	 */
	void CreatePulse( const Vector3Dd& prev_Bv, const Vector3Dd& curr_Bv, const Vector2Dd& skymap, std::vector<Pulse>& vec_pulse );


	/** @brief 直交座標位置の取得
	 *
	 *  @param[in]			angle  角度情報
	 *  @param[out]			pos	   直交座標系位置
	 */
	inline void GetCartesianPosition( const Angle& angle, Vector3Dd& pos ) const;


	/** @brief 極座標系位置の取得
	 *
	 *  @param[in]			angle  角度情報
	 *  @param[out]			pos	   の極座標系位置
	 */
	inline void GetPolarPosition( const Angle& angle, Vector3Dd& pos ) const;
};