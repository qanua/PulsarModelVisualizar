#pragma once

#include "pch.h"
#include "ModelCalculator.h"

#include <iostream>
#include <iterator>


namespace CalcLib {

	ModelCalculator::ModelCalculator()
	{
		// クリティカルセクションの初期化
		::InitializeCriticalSection(&critical_section_);
	}


	ModelCalculator::~ModelCalculator()
	{
		// クリティカルセクションの破棄
		::DeleteCriticalSection(&critical_section_);
	}


	void ModelCalculator::getResult(PulsarAsset& pulsar)
	{
		// パルサー情報のリセット
		pulsar.vec_lcfls_.clear();
		pulsar.vec_skymaps_.clear();
		pulsar.vec_polarcap_n_start_.vec_3dd_.clear();
		pulsar.vec_polarcap_s_end_.vec_3dd_.clear();
		pulsar.vec_polarcap_s_start_.vec_3dd_.clear();
		pulsar.vec_polarcap_n_end_.vec_3dd_.clear();
		pulsar.resetPulse();

		calculatePulsarModel(pulsar);
	}


	void ModelCalculator::calculatePulsarModel(PulsarAsset& pulsar)
	{
		const int mag_line_count(pulsar.MAGNETIC_LINE_COUNT);
		const double dphi(360.0 / mag_line_count);
		const int polarcap_count = (int)PolarCapDirection::NORTH_AND_SOUTH;

		// 方位角方向に探索
		for (int i = 0; i < mag_line_count; i++) {

			CalculationAssets assets(dphi * i * RADIAN, 0, 0);

			// 南北から探索
			for (size_t j = 0; j < polarcap_count; j++) {

				// 南極側は180度を加算する
				assets.inclination_angle_ = (pulsar.inclination_angle_ + 180.0 * j) * RADIAN;

				// 磁気モーメントの格納
				magnetic_moment_.x = sin(assets.inclination_angle_);
				magnetic_moment_.z = cos(assets.inclination_angle_);

				double open_angle(0.0);		// 二分法の始点（磁極付近） [degree]
				double close_angle(80.0);	// 二分法の終点（赤道付近） [degree]
				double theta(0.0);			// 二分法の中点 [degree]

				if (findLCFL(open_angle * RADIAN, close_angle * RADIAN, assets)) {

					// 開いた磁力線と閉じた磁力線との間でLCFLを探索する
					// 20: 二分法のステップ数
					for (int k = 0; k < 20; k++) {

						// 中点の更新
						theta = (open_angle + close_angle) * 0.5;

						// 磁力線の開閉状態を確認
						getMagneticLineState(theta * RADIAN, assets);

						// 探索範囲の更新
						if (assets.magnetic_line_state_ == MagneticLineState::OPEN) {

							open_angle = theta;
						}
						else if (assets.magnetic_line_state_ == MagneticLineState::CLOSE) {

							close_angle = theta;
						}
					}

					// LCFLの格納
					getLCFL(close_angle * RADIAN, assets);
					pulsar.vec_lcfls_.emplace_back(assets.lcfl_);

					// ポーラーキャップの格納
					if (j == (int)PolarCapDirection::NORTH) {

						pulsar.vec_polarcap_n_start_.vec_3dd_.emplace_back(assets.lcfl_.vec_3dd_.front());
						pulsar.vec_polarcap_s_end_.vec_3dd_.emplace_back(assets.lcfl_.vec_3dd_.back());
					}
					else if (j == (int)PolarCapDirection::SOUTH) {

						pulsar.vec_polarcap_n_end_.vec_3dd_.emplace_back(assets.lcfl_.vec_3dd_.back());
						pulsar.vec_polarcap_s_start_.vec_3dd_.emplace_back(assets.lcfl_.vec_3dd_.front());
					}

					// 一時保存のLCFLを削除
					assets.lcfl_.vec_3dd_.clear();

					// OuterGapからのパルスを格納
					// 10:  OuterGapの層の数
					for (int k = 0; k < 10; k++) {

						// 0.02: OuterGapの層の厚さ
						close_angle -= 0.02;

						if (k != 9) {

							getSkymapAndPulse(false, true, close_angle * RADIAN, assets);
						}
						else {

							// UpperBoundaryのスカイマップを格納
							getSkymapAndPulse(true, true, close_angle * RADIAN, assets);
							pulsar.vec_skymaps_.emplace_back(assets.skymap_);

						}
						// パルス波形の格納
						pulsar.addPulse(assets.vec_pulse_);
					}
				}
			}
		}
	}


	bool ModelCalculator::findLCFL(double open_angle, double close_angle, CalculationAssets& assets) const
	{
		bool result = false;

		// 磁極付近からの磁力線が開くか確認
		getMagneticLineState(open_angle, assets);

		// 赤道付近からの磁力線が閉じるか確認
		if (assets.magnetic_line_state_ == MagneticLineState::OPEN) {

			getMagneticLineState(close_angle, assets);

			if (assets.magnetic_line_state_ == MagneticLineState::CLOSE) {

				result = true;
			}
		}

		return result;
	}


	void ModelCalculator::getMagneticLineState(double polar_angle, CalculationAssets& assets) const
	{
		// パラメータの設定
		MagneticIntegrationParams p_par(polar_angle, assets);

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// 磁力線が星に戻って来る
			if (0 < i && p_par.pos_.Length() <= STAR_RADIUS) {

				assets.magnetic_line_state_ = MagneticLineState::CLOSE;
				break;
			}
			// 磁力線が光円柱を越える
			else if (0 < i && LIGHT_CYRINDER_RADIUS < std::hypot(p_par.pos_.x, p_par.pos_.y)) {

				assets.magnetic_line_state_ = MagneticLineState::OPEN;
				break;
			}

			RungeKutta(p_par.t_, STAR_RADIUS, p_par.pos_, p_par.cur_Bv_);
		}
	}


	void ModelCalculator::getLCFL(double close_angle, CalculationAssets& assets) const
	{
		// パラメータの設定
		MagneticIntegrationParams p_par(close_angle, assets);

		// LCFLのメモリ確保
		assets.lcfl_.vec_3dd_.reserve(MAX_LINE_LENGTH);

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// LCFLの格納
			assets.lcfl_.vec_3dd_.emplace_back(p_par.pos_);

			if (0 < i && p_par.pos_.Length() <= STAR_RADIUS) {

				break;
			}

			RungeKutta(p_par.t_, STAR_RADIUS, p_par.pos_, p_par.cur_Bv_);
		}
	}


	void ModelCalculator::getSkymapAndPulse(bool get_skymap, bool get_pulse, double polar_angle, CalculationAssets& assets) const
	{
		if (get_skymap || get_pulse) {

			// パラメータの設定
			MagneticIntegrationParams p_par(polar_angle, assets);

			if (get_skymap) {

				// パルス波形のメモリ確保
				assets.vec_pulse_.reserve(180);
			}
			if (get_pulse) {

				// スカイマップのメモリ確保
				assets.skymap_.vec_2dd_.reserve(MAX_LINE_LENGTH);
			}

			// 磁場方向に積分
			for (int i = 0; i < MAX_LINE_LENGTH; i++) {

				// 磁場の初期値
				if (i == 0) {

					calculateMagneticField(0, p_par.pos_, p_par.cur_Bv_);
				}

				p_par.cur_Bphi_ = (p_par.pos_.x * p_par.cur_Bv_.x +
								   p_par.pos_.y * p_par.cur_Bv_.y) /
								   std::hypot(p_par.pos_.x, p_par.pos_.y);

				const bool is_null((p_par.pre_Bv_.z * p_par.cur_Bv_.z) < 0.0);
				const bool is_return((p_par.pre_Bphi_ * p_par.cur_Bphi_) < 0.0);

				if (!p_par.is_outer_gap_ && is_null) {

					p_par.is_outer_gap_ = true;
				}
				else if (p_par.is_outer_gap_ && is_return) {

					p_par.is_outer_gap_ = false;
				}

				// 放射領域（OuterGap）からの光子のみ採用する
				if (p_par.is_outer_gap_)
				{
					calculateSkymapAndPulse(get_skymap, get_pulse, p_par.pos_, p_par.pre_Bv_, p_par.cur_Bv_,
											assets.skymap_.vec_2dd_, assets.vec_pulse_);
				}

				p_par.pre_Bv_ = p_par.cur_Bv_;
				p_par.pre_Bphi_ = p_par.cur_Bphi_;

				if (0 < i &&
					(p_par.pos_.Length() <= STAR_RADIUS ||
					 LIGHT_CYRINDER_RADIUS < std::hypot(p_par.pos_.x, p_par.pos_.y))) {

					break;
				}

				RungeKutta(p_par.t_, STAR_RADIUS, p_par.pos_, p_par.cur_Bv_);
			}
		}
	}


	void ModelCalculator::calculateSkymapAndPulse(
		bool get_skymap, bool get_pulse, const Vector3Dd& pos,
		const Vector3Dd& pre_Bv, const Vector3Dd& cur_Bv,
		std::vector<Vector2Dd>& vec_skymap, std::vector<Pulse>& vec_pulse) const
	{
		if (get_skymap && get_pulse) {

			// スカイマップの格納
			calculateSkyMap(pos, cur_Bv, vec_skymap);

			// パルス波形の格納
			calculatePulse(pos, pre_Bv, cur_Bv, vec_skymap, vec_pulse);
		}
		else if (!get_skymap && get_pulse) {

			// スカイマップの一時保持
			SkyMap skymap;
			skymap.vec_2dd_.reserve(MAX_LINE_LENGTH);
			calculateSkyMap(pos, cur_Bv, skymap.vec_2dd_);

			// パルス波形の格納
			calculatePulse(pos, pre_Bv, cur_Bv, skymap.vec_2dd_, vec_pulse);
		}
	}


	void ModelCalculator::calculatePulse(
		const Vector3Dd& pos, const Vector3Dd& pre_Bv, const Vector3Dd& cur_Bv,
		std::vector<Vector2Dd>& vec_skymap, std::vector<Pulse>& vec_pulse) const
	{
		// パルス波形の算出
		double photon_count(0.0);
		double length(pre_Bv.Length() * cur_Bv.Length());

		// パルサーから地球までの距離
		const double distance = 1.0;

		if (0 < length) {

			double radi_angle(acos(pre_Bv.Dot(cur_Bv) / (pre_Bv.Length() * cur_Bv.Length())));

			if (0 < radi_angle) {

				double curv_radius(STAR_RADIUS / radi_angle);
				double radi_solid_angle(PI * radi_angle * radi_angle);

				photon_count = 1.0 / (distance * distance * radi_solid_angle) * (STAR_RADIUS / curv_radius);
			}
		}

		Pulse pulse;
		Vector2Dd vec_pulse_temp(vec_skymap.back().x, photon_count);
		pulse.vec_2dd_.emplace_back(vec_pulse_temp);
		pulse.viewing_angle_ = vec_skymap.back().y;
		vec_pulse.emplace_back(pulse);
	}


	void ModelCalculator::calculateSkyMap(const Vector3Dd& pos, const Vector3Dd& Bv, std::vector<Vector2Dd>& vec_skymap) const
	{
		const double Vc(std::hypot(pos.x, pos.y));
		const double Bphi(Bv.x * (-pos.y) + Bv.y * pos.x);
		const double Vpara(-Bphi + sqrt(Bphi * Bphi + 1 - Vc * Vc));

		Vector3Dd Vcorot(Vpara * Bv.x - pos.y, Vpara * Bv.y + pos.x, Vpara * Bv.z);
		Vcorot = Vcorot.Normalize();

		const double sign_y((0 <= Vcorot.y) ? 1.0 : -1.0);
		const double phi(sign_y * acos(Vcorot.x / std::hypot(Vcorot.x, Vcorot.y)));
		double phase(-phi - pos.Dot(Vcorot));

		phase = (phase < 0) ? 2.0 * PI + phase : phase;

		Vector2Dd vec_temp(phase / RADIAN, acos(Vcorot.z) / RADIAN);
		vec_skymap.emplace_back(vec_temp);
	}
}