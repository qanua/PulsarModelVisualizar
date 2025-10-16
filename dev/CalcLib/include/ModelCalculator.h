#pragma once

#include "PulsarAsset.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

#include <windows.h>
#include <vector>


namespace CalcLib {

	/** @class ModelCalculator
	 *
	 *  @brief パルサーモデルの計算
	 */
	class ModelCalculator {

	public:

		/** @brief コンストラクタ */
		ModelCalculator();


		/** @brief デストラクタ */
		~ModelCalculator();


		/**
		 *	@brief 計算結果の取得
		 *
		 *  @param[in/out]	pulsar	パルサー情報
		 */
		void getResult(PulsarAsset& pulsar);


	private:

		using LCFL = PulsarAsset::LCFL;
		using Pulse = PulsarAsset::Pulse;
		using SkyMap = PulsarAsset::SkyMap;

	/** @class CalculationAssets
	 *
	 *  @brief 計算に必要な情報
	 */
		class CalculationAssets
		{
		public:

			/** @brief ポーラーキャップの方位
			 * 
			 *  磁力線の描画開始地点を指定する
			 */
			enum class PolarCapDirection
			{
				/** 北極側 */
				NORTH = 0,

				/** 南極側 */
				SOUTH,

				/** 北極・南極の両方 */
				NORTH_AND_SOUTH
			};


			/** @brief 磁力線の開閉状態
			 *
			 *  LCFLを挟む磁力線の状態を判断する
			 */
			enum class MagneticLineState
			{
				/** 磁力線は閉じている */
				CLOSE = 0,

				/** 磁力線は開いている */
				OPEN
			};


			/** @brief 磁力線の方位角 [rad] */
			double azimuthal_angle_;

			/** @brief 磁力線の極角 [rad] */
			double polar_angle_;

			/** @brief 磁化軸の傾き [rad] */
			double inclination_angle_;

			/** @brief 磁力線の開閉状態 */
			MagneticLineState magnetic_line_state_;

			/** @brief 1本のLCFL */
			LCFL lcfl_;

			/** @brief 磁場の計算地点 */
			Vector3Dd magnetic_field_pos_;

			/** @brief 1本のスカイマップ線に対するパルス波形 */
			std::vector<Pulse> vec_pulse_;

			/** @brief 1本のスカイマップ線 */
			SkyMap skymap_;

			/** @brief コンストラクタ */
			CalculationAssets(double azimuth, double polar, double inclination) {

				azimuthal_angle_ = azimuth;
				polar_angle_ = polar;
				inclination_angle_ = inclination;
				magnetic_line_state_ = MagneticLineState::OPEN;
			}


			/** @brief デストラクタ */
			~CalculationAssets() {};
		};


		using PolarCapDirection = CalculationAssets::PolarCapDirection;
		using MagneticLineState = CalculationAssets::MagneticLineState;


		/** @brief クリティカルセクション */
		CRITICAL_SECTION critical_section_;

		/** @brief 磁気モーメント */
		Vector3Dd magnetic_moment_;

		/** @brief 円周率 */
		const double PI = 3.14159265359;

		/** @brief 度→ラジアン変換 */
		const double RADIAN = PI / 180.0;

		/** @brief 中性子星の半径 */
		const double STAR_RADIUS = 0.00209;

		/** @brief 光円柱の半径 */
		const double LIGHT_CYRINDER_RADIUS = 1.0;

		/** @brief 磁力線などの長さの最大値
		 *
		 *  磁力線などを積分で求める際に使用する
		 *  光円柱の半径と中性子星の半径の比に依存する
		 *  10: 1スケールあたりの分割数
		 */
		const int MAX_LINE_LENGTH = std::abs(LIGHT_CYRINDER_RADIUS) / std::abs(STAR_RADIUS) * 10;

		/** @brief パルサーから地球までの距離 */
		const double DISTANCE_PULSAR_TO_EARTH = 1.0;

		/** @brief OuterGapの層の厚さ */
		const double OUTER_GAP_LAYER_THICKNESS = 0.02;

		/** @brief OuterGapの層の数 */
		const int OUTER_GAP_LAYER_COUNT = 10;


		/**
		 *	@brief パルサーモデルの計算
		 *
		 *  @param[in/out]	pulsar			パルサー情報
		 */
		void calculatePulsarModel(PulsarAsset& pulsar);


		/** @brief LCFLの探索
		 *
		 *  @param[in]		open_angle		磁極付近から伸びる磁力線の極角 [rad]
		 *  @param[in]		close_angle		赤道付近から伸びる磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 *
		 *  @return			true ：LCFLあり
		 *					false：LCFLなし
		 */
		bool findLCFL(double open_angle, double close_angle, CalculationAssets& assets) const;


		/** @brief 磁力線の開閉状態の取得
		 *
		 *  @param[in]		polar_angle		計算地点の磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 */
		void getMagneticLineState(double polar_angle, CalculationAssets& assets) const;


		/** @brief LCFLの取得
		 *
		 *  @param[in]		close_angle		計算地点の閉じた磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 */
		void getLCFL(double close_angle, CalculationAssets& assets) const;


		/** @brief パルス波形の取得
		 *	
		 *  @param[in]		polar_angle		計算地点の磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 */
		void getPulse(double polar_angle, CalculationAssets& assets) const;


		/** @brief スカイマップの取得
		 *
		 *  @param[in]		polar_angle		計算地点の磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 */
		void getSkyMap(double polar_angle, CalculationAssets& assets) const;


		/** @brief 磁場の計算
		*
		*  @param[in]		t				時間（依存している場合に使用）
		*  @param[in]		v				位置ベクトル
		*  @param[in]		Bv				磁場ベクトル
		*/
		void calculateMagneticField(double /*t*/, Vector3Dd v, Vector3Dd& bv) const;


		/** @brief パルス波形の計算
		 *
		 *  @param[in]		pos				計算地点
		 *  @param[in]		pre_Bv			磁場の前回値
		 *  @param[in]		cur_Bv			磁場の現在値
		 *  @param[out]		assets			計算に必要な情報
		 */
		void calculatePulse(
			const Vector3Dd& pos, const Vector3Dd& pre_Bv,const Vector3Dd& cur_Bv, CalculationAssets& assets) const;


		/** @brief スカイマップの計算
		 *
		 *  @param[in]		pos				計算地点
		 *  @param[in]		Bv				磁場
		 *  @param[out]		vec_skymap		スカイマップの点群データ
		 */
		 void calculateSkyMap(const Vector3Dd& pos, const Vector3Dd& Bv, std::vector<Vector2Dd>& vec_skymap) const;


		/** @brief 直交座標の取得
		 *
		 *  @param[in]		azimth			方位角 [rad]
		 *  @param[in]		polar			極角 [rad]
		 *  @param[in]		inclination		磁化軸の傾き [rad]
		 *  @param[out]		pos				直交座標
		 */
		void getCartesianPosition(double azimth, double polar, double inclination, Vector3Dd& pos) const;


		/** @brief 極座標の取得
		 *
		 *  @param[in]		azimth			方位角 [rad]
		 *  @param[in]		polar			極角 [rad]
		 *  @param[out]		pos				極座標
		 */
		void getPolarPosition(double azimth, double polar, Vector3Dd& pos) const;
	};
}