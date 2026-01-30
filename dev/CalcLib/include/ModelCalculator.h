#pragma once

#include "PulsarAsset.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

#include <windows.h>
#include <vector>
#include <functional>


namespace CalcLib {

	/** @brief 円周率 */
	const double PI = 3.14159265359;

	/** @brief 度→ラジアン変換 */
	const double RADIAN = PI / 180.0;

	/** @brief 中性子星の半径 */
	const double STAR_RADIUS = 0.00209;

	/** @brief 光円柱の半径 */
	const double LIGHT_CYRINDER_RADIUS = 1.0;


	/** @class ModelCalculator
	 *
	 *  @brief パルサーモデルの計算
	 */
	class ModelCalculator {

	public:

		/** @brief 磁力線などの長さの最大値
		 *
		 *  磁力線などを積分で求める際に使用する
		 *  光円柱の半径と中性子星の半径の比に依存する
		 *  10: 1スケールあたりの分割数
		 */
		const int MAX_LINE_LENGTH = static_cast<int>(std::abs(LIGHT_CYRINDER_RADIUS) / std::abs(STAR_RADIUS) * 10);


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
		struct CalculationAssets {

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
			~CalculationAssets() {}
		};


		/** @class MagneticIntegrationParams
		 *
		 *  @brief 磁場の積分計算に必要なパラメータ
		 */
		struct MagneticIntegrationParams {

		public:

			/** @brief 時間 */
			double t_;

			/** @brief 位置ベクトル */
			Vector3Dd& pos_;

			/** @brief 磁場ベクトルの前回値 */
			Vector3Dd pre_Bv_;

			/** @brief 磁場ベクトルの現在値 */
			Vector3Dd cur_Bv_;

			/** @brief 共回転方向の磁場の大きさの前回値 */
			double pre_Bphi_;

			/** @brief 共回転方向の磁場の大きさの現在値 */
			double cur_Bphi_;

			/** @brief OuterGapかどうかの判定フラグ */
			bool is_outer_gap_;


			/** @brief コンストラクタ */
			MagneticIntegrationParams(double polar_angle, CalculationAssets& assets)
				: t_(0), pos_(assets.magnetic_field_pos_)
				, cur_Bv_(0), pre_Bv_(0)
				, cur_Bphi_(0.0), pre_Bphi_(0.0)
				, is_outer_gap_(false)
				
			{
				getCartesianPosition(
					assets.azimuthal_angle_,
					assets.polar_angle_ = polar_angle,
					assets.inclination_angle_,
					assets.magnetic_field_pos_
				);
			}


			/** @brief デストラクタ */
			~MagneticIntegrationParams() {}


		private:

			/** @brief 直交座標の取得
			 *
			 *  @param[in]		azimth			方位角 [rad]
			 *  @param[in]		polar			極角 [rad]
			 *  @param[in]		inclination		磁化軸の傾き [rad]
			 *  @param[out]		pos				直交座標
			 */
			inline void getCartesianPosition(double azimuth, double polar, double inclination, Vector3Dd& pos) const
			{
				// 極座標の取得
				Vector3Dd ppos;
				getPolarPosition(azimuth, polar, ppos);

				// 直交座標の取得
				pos.x = ppos.z * sin(inclination) + ppos.x * cos(inclination);
				pos.y = ppos.y;
				pos.z = ppos.z * cos(inclination) - ppos.x * sin(inclination);
			}


			/** @brief 極座標の取得
			 *
			 *  @param[in]		azimth			方位角 [rad]
			 *  @param[in]		polar			極角 [rad]
			 *  @param[out]		pos				極座標
			 */
			inline void getPolarPosition(double azimuth, double polar, Vector3Dd& pos) const
			{
				// 磁化軸を中心とした極座標
				pos.x = STAR_RADIUS * sin(polar) * cos(azimuth);
				pos.y = STAR_RADIUS * sin(polar) * sin(azimuth);
				pos.z = STAR_RADIUS * cos(polar);
			}
		};


		using PolarCapDirection = CalculationAssets::PolarCapDirection;
		using MagneticLineState = CalculationAssets::MagneticLineState;


		/** @brief クリティカルセクション */
		CRITICAL_SECTION critical_section_;

		/** @brief 磁気モーメント */
		Vector3Dd magnetic_moment_;


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


		/** @brief スカイマップ・パルス波形の取得
		 *
		 *  @param[in]		get_skymap		スカイマップの取得有無フラグ
		 *  @param[in]		get_pulse		パルス波形の取得有無フラグ
		 *  @param[in]		polar_angle		計算地点の磁力線の極角 [rad]
		 *  @param[in/out]	assets			計算に必要な情報
		 */
		void getSkymapAndPulse(bool get_skymap, bool get_pulse, double polar_angle, CalculationAssets& assets) const;


		/** @brief スカイマップ・パルス波形の取得
		 *
		 *  @param[in]		get_skymap		スカイマップの取得有無フラグ
		 *  @param[in]		get_pulse		パルス波形の取得有無フラグ
		 *  @param[in]		pos				計算地点
		 *  @param[in]		pre_Bv			磁場の前回値
		 *  @param[in]		cur_Bv			磁場の現在値
		 *  @param[out]		vec_skymap		スカイマップの点群データ
		 *  @param[out]		vec_pulse		パルス波形
		 */
		void calculateSkymapAndPulse(
			bool get_skymap, bool get_pulse, const Vector3Dd& pos,
			const Vector3Dd& pre_Bv, const Vector3Dd& cur_Bv,
			std::vector<Vector2Dd>& vec_skymap, std::vector<Pulse>& vec_pulse) const;


		/** @brief パルス波形の計算
		 *
		 *  @param[in]		pos				計算地点
		 *  @param[in]		pre_Bv			磁場の前回値
		 *  @param[in]		cur_Bv			磁場の現在値
		 *  @param[out]		vec_skymap		スカイマップの点群データ
		 *  @param[out]		vec_pulse		パルス波形
		 */
		void calculatePulse(
			const Vector3Dd& pos, const Vector3Dd& pre_Bv,const Vector3Dd& cur_Bv,
			std::vector<Vector2Dd>& vec_skymap, std::vector<Pulse>& vec_pulse) const;


		/** @brief スカイマップの計算
		 *
		 *  @param[in]		pos				計算地点
		 *  @param[in]		Bv				磁場
		 *  @param[out]		vec_skymap		スカイマップの点群データ
		 */
		 void calculateSkyMap(const Vector3Dd& pos, const Vector3Dd& Bv, std::vector<Vector2Dd>& vec_skymap) const;


		/** @brief ルンゲ・クッタ法（4次）
		 *
		 *  @param[in]		t				時間
		 *  @param[in]		dt				時間刻み
		 *  @param[in/out]	f				微分関数
		 *  @param[in/out]	df				微分
		 */
		 inline void RungeKutta(double t, const double dt, Vector3Dd& f, Vector3Dd& df) const
		 {
			 Vector3Dd df1, df2, df3;
			 Vector3Dd g;

			 const double DT2(dt * 0.5);
			 const double DT6(dt / 6.0);

			 ModelCalculator::calculateMagneticField(t, f, df1);
			 g = f + DT2 * df1;

			 ModelCalculator::calculateMagneticField(t + DT2, g, df2);
			 g = f + DT2 * df2;

			 ModelCalculator::calculateMagneticField(t + DT2, g, df3);
			 g = f + dt * df3;

			 ModelCalculator::calculateMagneticField(t + dt, g, df);
			 f += DT6 * (df1 + 2.0 * (df2 + df3) + df);

			 t += dt;
		 }


		 /** @brief 磁場の計算
		 *
		 *  @param[in]		t				時間（依存している場合に使用）
		 *  @param[in]		v				位置ベクトル
		 *  @param[in/out]	Bv				磁場ベクトル
		 */
		 inline void calculateMagneticField(double /*t*/, Vector3Dd v, Vector3Dd& bv) const
		 {
			 const double r(v.Length());

			 if (r != 0) {
				 const double t(STAR_RADIUS - r);
				 const double sint(sin(t));
				 const double cost(cos(t));
				 const double R2(r * r);
				 const double R3(R2 * r);

				 bv.x = magnetic_moment_.x * (cost * (1.0 / R3 - 1.0 / r) - sint / R2);
				 bv.y = magnetic_moment_.x * (sint * (1.0 / R3 - 1.0 / r) + cost / R2);
				 bv.z = magnetic_moment_.z / R3;

				 const double A((bv.Dot(v)) * 3.0 / r + 2.0 * magnetic_moment_.x * (sint * v.y + cost * v.x) / R2);

				 bv = ((A * v) / v.Length() - bv) / bv.Length();
			 }
		 }
	};
}