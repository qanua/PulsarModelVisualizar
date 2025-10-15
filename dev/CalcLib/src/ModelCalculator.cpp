#pragma once

#include "pch.h"
#include "ModelCalculator.h"
#include "Math/RungeKutta.h"

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


	void ModelCalculator::GetResult(Pulsar& pulsar)
	{
		// パルサー情報のリセット
		pulsar.m_vecMagneticLine.clear();
		pulsar.m_vecSkyMap.clear();
		pulsar.m_vecPolarCapNorthBegin.m_vecVertex3D.clear();
		pulsar.m_vecPolarCapNorthEnd.m_vecVertex3D.clear();
		pulsar.m_vecPolarCapSouthBegin.m_vecVertex3D.clear();
		pulsar.m_vecPolarCapSouthEnd.m_vecVertex3D.clear();
		pulsar.ResetPulse();

		calculatePulsarModel(pulsar);
	}


	void ModelCalculator::calculatePulsarModel(Pulsar& pulsar)
	{
		const int mag_line_count(pulsar.m_MagneticLineCount);
		const double dphi(360.0 / mag_line_count);
		const int polarcap_count = (int)PolarCapDirection::NORTH_AND_SOUTH;

		// 方位角方向に探索
		for (int i = 0; i < mag_line_count; i++) {

			CalculationAssets assets(dphi * i * RADIAN, 0, 0);

			// 南北から探索
			for (size_t j = 0; j < polarcap_count; j++) {

				// 南極側は180度を加算する
				assets.inclination_angle_ = (pulsar.m_InclinationAngle + 180.0 * j) * RADIAN;

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
					pulsar.m_vecMagneticLine.emplace_back(assets.lcfl_);

					// ポーラーキャップの格納
					if (j == (int)PolarCapDirection::NORTH) {

						pulsar.m_vecPolarCapNorthBegin.m_vecVertex3D.emplace_back(assets.lcfl_.m_vecVertex3D.front());
						pulsar.m_vecPolarCapSouthEnd.m_vecVertex3D.emplace_back(assets.lcfl_.m_vecVertex3D.back());
					}
					else if (j == (int)PolarCapDirection::SOUTH) {

						pulsar.m_vecPolarCapNorthEnd.m_vecVertex3D.emplace_back(assets.lcfl_.m_vecVertex3D.back());
						pulsar.m_vecPolarCapSouthBegin.m_vecVertex3D.emplace_back(assets.lcfl_.m_vecVertex3D.front());
					}

					// 一時保存のLCFLを削除
					assets.lcfl_.m_vecVertex3D.clear();

					// パルスの格納
					for (int k = 0; k < OUTER_GAP_LAYER_COUNT; k++) {

						close_angle -= OUTER_GAP_LAYER_THICKNESS;
						getPulse(close_angle * RADIAN, assets);
						pulsar.AddPulse(assets.vec_pulse_);
					}

					// LCFLのスカイマップの格納
					getSkyMap(close_angle * RADIAN, assets);
					pulsar.m_vecSkyMap.emplace_back(assets.skymap_);
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
		getCartesianPosition(
			assets.azimuthal_angle_,
			assets.polar_angle_ = polar_angle,
			assets.inclination_angle_,
			assets.magnetic_field_pos_
		);

		double t(0.0);
		Vector3Dd pos(assets.magnetic_field_pos_);
		Vector3Dd tmp;

		const auto fn([&](double t, Vector3Dd v, Vector3Dd& Bv) { calculateMagneticField(t, v, Bv); });

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// 磁力線が星に戻って来る
			if (0 < i && pos.Length() <= STAR_RADIUS) {

				assets.magnetic_line_state_ = MagneticLineState::CLOSE;
				break;
			}
			// 磁力線が光円柱を越える
			else if (0 < i && LIGHT_CYRINDER_RADIUS < std::hypot(pos.x, pos.y)) {

				assets.magnetic_line_state_ = MagneticLineState::OPEN;
				break;
			}

			RungeKutta(pos, t, STAR_RADIUS, tmp, fn);
		}
	}


	void ModelCalculator::getLCFL(double close_angle, CalculationAssets& assets) const
	{
		getCartesianPosition(
			assets.azimuthal_angle_,
			assets.polar_angle_ = close_angle,
			assets.inclination_angle_,
			assets.magnetic_field_pos_
		);

		double t(0.0);
		Vector3Dd pos(assets.magnetic_field_pos_);
		Vector3Dd tmp;

		const auto fn([&](double t, Vector3Dd v, Vector3Dd& bv) { calculateMagneticField(t, v, bv); });

		// LCFLのメモリ確保
		assets.lcfl_.m_vecVertex3D.reserve(MAX_LINE_LENGTH);

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// LCFLの格納
			assets.lcfl_.m_vecVertex3D.emplace_back(pos);

			if (0 < i && pos.Length() <= STAR_RADIUS) {

				break;
			}

			RungeKutta(pos, t, STAR_RADIUS, tmp, fn);
		}
	}


	void ModelCalculator::getPulse(double polar_angle, CalculationAssets& assets) const
	{
		getCartesianPosition(
			assets.azimuthal_angle_,
			assets.polar_angle_ = polar_angle,
			assets.inclination_angle_,
			assets.magnetic_field_pos_
		);

		double t(0.0);
		Vector3Dd pos(assets.magnetic_field_pos_);
		Vector3Dd cur_Bv;
		Vector3Dd pre_Bv;
		double cur_Bphi(0.0);
		double pre_Bphi(0.0);
		bool is_outer_gap(false);

		const auto fn([&](double t, Vector3Dd v, Vector3Dd& bv) { calculateMagneticField(t, v, bv); });

		// パルス波形のメモリ確保
		assets.vec_pulse_.reserve(180);

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// 磁場の初期値
			if (i == 0) {

				calculateMagneticField(0, pos, cur_Bv);
			}

			cur_Bphi = (pos.x * cur_Bv.x + pos.y * cur_Bv.y) / std::hypot(pos.x, pos.y);

			const bool is_null((pre_Bv.z * cur_Bv.z) < 0.0);
			const bool is_return((pre_Bphi * cur_Bphi) < 0.0);

			if (!is_outer_gap && is_null) {

				is_outer_gap = true;
			}
			else if (is_outer_gap && is_return) {

				is_outer_gap = false;
			}

			// 放射領域（OuterGap）からの光子のみ採用する
			if (is_outer_gap)
			{
				calculatePulse(pos, pre_Bv, cur_Bv, assets);
			}

			pre_Bv = cur_Bv;
			pre_Bphi = cur_Bphi;

			if (0 < i &&
				(pos.Length() <= STAR_RADIUS || LIGHT_CYRINDER_RADIUS < std::hypot(pos.x, pos.y))) {

				break;
			}

			RungeKutta(pos, t, STAR_RADIUS, cur_Bv, fn);
		}
	}


	void ModelCalculator::getSkyMap(double polar_angle, CalculationAssets& assets) const
	{
		getCartesianPosition(
			assets.azimuthal_angle_,
			assets.polar_angle_ = polar_angle,
			assets.inclination_angle_,
			assets.magnetic_field_pos_
		);

		double t(0.0);
		Vector3Dd pos(assets.magnetic_field_pos_);
		Vector3Dd cur_Bv;
		Vector3Dd pre_Bv;
		double cur_Bphi(0.0);
		double pre_Bphi(0.0);
		bool is_outer_gap(false);

		const auto fn([&](double t, Vector3Dd v, Vector3Dd& bv) { calculateMagneticField(t, v, bv); });

		// スカイマップのメモリ確保
		assets.skymap_.m_vecVertex2D.reserve(MAX_LINE_LENGTH);

		// 磁場方向に積分
		for (int i = 0; i < MAX_LINE_LENGTH; i++) {

			// 磁場の初期値
			if (i == 0) {

				calculateMagneticField(0, pos, cur_Bv);
			}

			cur_Bphi = (pos.x * cur_Bv.x + pos.y * cur_Bv.y) / std::hypot(pos.x, pos.y);

			const bool is_null((pre_Bv.z * cur_Bv.z) < 0);
			const bool is_return((pre_Bphi * cur_Bphi) < 0);

			if (!is_outer_gap && is_null) {

				is_outer_gap = true;
			}
			else if (is_outer_gap && is_return) {

				is_outer_gap = false;
			}

			// 放射領域（OuterGap）からの光子のみ採用する
			if (is_outer_gap) {

				calculateSkyMap(pos, cur_Bv, assets.skymap_.m_vecVertex2D);
			}

			pre_Bv = cur_Bv;
			pre_Bphi = cur_Bphi;

			if (0 < i &&
				(pos.Length() <= STAR_RADIUS || LIGHT_CYRINDER_RADIUS < std::hypot(pos.x, pos.y))) {

				break;
			}

			RungeKutta(pos, t, STAR_RADIUS, cur_Bv, fn);
		}
	}


	void ModelCalculator::calculateMagneticField(double /*t*/, Vector3Dd v, Vector3Dd& bv) const
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


	void ModelCalculator::calculatePulse(const Vector3Dd& pos, const Vector3Dd& pre_Bv, const Vector3Dd& cur_Bv, CalculationAssets& assets) const
	{
		// スカイマップの算出
		SkyMap skymap;
		skymap.m_vecVertex2D.reserve(MAX_LINE_LENGTH);
		calculateSkyMap(pos, cur_Bv, skymap.m_vecVertex2D);

		// パルス波形の算出
		double photon_count(0.0);
		double length(pre_Bv.Length() * cur_Bv.Length());

		if (0 < length) {

			double radi_angle(acos(pre_Bv.Dot(cur_Bv) / (pre_Bv.Length() * cur_Bv.Length())));

			if (0 < radi_angle) {

				double curv_radius(STAR_RADIUS / radi_angle);
				double radi_solid_angle(PI * radi_angle * radi_angle);

				photon_count = 1.0 / (DISTANCE_PULSAR_TO_EARTH * DISTANCE_PULSAR_TO_EARTH * radi_solid_angle) * (STAR_RADIUS / curv_radius);
			}
		}

		Pulse pulse;
		Vector2Dd vec_pulse_temp(skymap.m_vecVertex2D.back().x, photon_count);
		pulse.m_vecVertex2D.emplace_back(vec_pulse_temp);
		pulse.m_ViewingAngle = skymap.m_vecVertex2D.back().y;
		assets.vec_pulse_.emplace_back(pulse);
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


	void ModelCalculator::getCartesianPosition(double azimuth, double polar, double inclination, Vector3Dd& pos) const
	{
		// 極座標の取得
		Vector3Dd ppos;
		getPolarPosition(azimuth, polar, ppos);

		// 直交座標の取得
		pos.x = ppos.z * sin(inclination) + ppos.x * cos(inclination);
		pos.y = ppos.y;
		pos.z = ppos.z * cos(inclination) - ppos.x * sin(inclination);
	}


	void ModelCalculator::getPolarPosition(double azimuth, double polar, Vector3Dd& pos) const
	{
		// 磁化軸を中心とした極座標
		pos.x = STAR_RADIUS * sin(polar) * cos(azimuth);
		pos.y = STAR_RADIUS * sin(polar) * sin(azimuth);
		pos.z = STAR_RADIUS * cos(polar);
	}
}
