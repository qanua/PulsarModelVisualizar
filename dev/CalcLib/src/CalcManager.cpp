#pragma once

#include "pch.h"
#include "CalcManager.h"

namespace CalcLib {

	CalcManager::CalcManager()
		: p_pulsar_(new Pulsar())
		, p_critical_section_(new CRITICAL_SECTION())
	{
		// クリティカルセクションの初期化
		::InitializeCriticalSection(p_critical_section_);

		// 初期値の設定
		p_pulsar_->m_InclinationAngle = 57;
		p_pulsar_->m_ViewingAngle = 90;
		p_pulsar_->m_MagneticLineCount = 60;

		// パルサーモデルの計算
		calculatePulsarModel();
	}


	CalcManager::~CalcManager()
	{
		// リソースの開放
		delete p_pulsar_;

		// クリティカルセクションの破棄
		::DeleteCriticalSection(p_critical_section_);
		delete p_critical_section_;
	}


	CalcManager& CalcManager::getInstance()
	{
		static CalcManager instance;
		return instance;
	}


	void CalcManager::calculatePulsarModel()
	{
		static ModelCalculator calclator;
		calclator.GetResult(*p_pulsar_);
	}


	void CalcManager::setInclinationAngle(int degree)
	{
		p_pulsar_->m_InclinationAngle = degree;
		calculatePulsarModel();
	}


	int CalcManager::getMagneticLine(float* buffer)
	{
		// 磁力線の頂点を取得
		std::vector<std::vector<Vector3Dd>>* p_vec_vec_v3d(new std::vector<std::vector<Vector3Dd>>);
		p_pulsar_->GetMagneticLineVertex(*p_vec_vec_v3d);

		// 頂点数
		int counter(0);

		// float配列に格納
		for (unsigned int i = 0; i < p_vec_vec_v3d->size(); i++) {
			std::vector<Vector3Dd> vec_v3d((*p_vec_vec_v3d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v3d.size(); j++) {
					buffer[counter++] = vec_v3d[j].y;
					buffer[counter++] = vec_v3d[j].z;
					buffer[counter++] = vec_v3d[j].x;
				}
			}
			else {
				counter += vec_v3d.size() * 3;
			}
		}
		return counter;
	}


	int CalcManager::getPolarCapNorthOpened(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->GetPolarCapNorthBeginVertex(), buffer);
	}


	int CalcManager::getPolarCapNorthClosed(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->GetPolarCapNorthEndVertex(), buffer);
	}


	int CalcManager::getPolarCapSouthOpened(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->GetPolarCapSouthBeginVertex(), buffer);
	}


	int CalcManager::getPolarCapSouthClosed(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->GetPolarCapSouthEndVertex(), buffer);
	}


	int CalcManager::getVertices3Dd(std::vector<Vector3Dd>& vertices, float* buffer)
	{
		// 頂点数
		int counter(0);

		// float配列に格納
		if (buffer) {
			for (unsigned int i = 0; i < vertices.size(); i++) {
				buffer[counter++] = vertices[i].x;
				buffer[counter++] = vertices[i].y;
				buffer[counter++] = vertices[i].z;
			}
		}
		else {
			counter = vertices.size() * 3;
		}
		return counter;
	}


	int CalcManager::getSkyMap(float* buffer)
	{
		// スカイマップの頂点を取得
		std::vector<std::vector<Vector2Dd>>* p_vec_vec_v2d(new std::vector<std::vector<Vector2Dd>>);
		p_pulsar_->GetSkyMapVertex(*p_vec_vec_v2d);

		// 頂点数
		int counter(0);

		// float配列に格納
		for (unsigned int i = 0; i < p_vec_vec_v2d->size(); i++) {
			std::vector<Vector2Dd> vec_v2d((*p_vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = vec_v2d[j].x;
					buffer[counter++] = std::abs(vec_v2d[j].y - 180);	// 上下を反転
				}
			}
			else {
				counter += vec_v2d.size() * 2;
			}
		}
		return counter;
	}


	int CalcManager::getPulseProfile(float* buffer, bool normalize)
	{
		// 正規化
		if (normalize)
		{
			p_pulsar_->NormalizePulse();
		}

		// パルスプロファイルの頂点を取得
		std::vector<std::vector<Vector2Dd>>* p_vec_vec_v2d(new std::vector<std::vector<Vector2Dd>>);
		p_pulsar_->GetPulseVertex(*p_vec_vec_v2d);

		// 頂点数
		int counter(0);

		// float配列に格納
		for (unsigned int i = 0; i < p_vec_vec_v2d->size(); i++) {
			std::vector<Vector2Dd> vec_v2d((*p_vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = vec_v2d[j].x;	// Phase
					buffer[counter++] = vec_v2d[j].y;	// Intensity(Relative)
				}
			}
			else {
				counter += vec_v2d.size() * 2;
			}
		}
		return counter;
	}
}