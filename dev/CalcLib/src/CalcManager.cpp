#pragma once

#include "pch.h"
#include "CalcManager.h"


namespace CalcLib {

	CalcManager::CalcManager()
		: p_pulsar_(new PulsarAsset())
		, p_critical_section_(new CRITICAL_SECTION())
	{
		// クリティカルセクションの初期化
		::InitializeCriticalSection(p_critical_section_);
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
		calclator.getResult(*p_pulsar_);
	}


	void CalcManager::setInclinationAngle(int degree)
	{
		p_pulsar_->inclination_angle_ = degree;
		calculatePulsarModel();
	}


	int CalcManager::getMagneticLine(float* buffer)
	{
		// 磁力線の頂点を取得
		std::vector<std::vector<Vector3Dd>> vec_vec_v3d;
		p_pulsar_->getLCFLVertices(vec_vec_v3d);

		// 頂点数
		int counter(0);

		auto size(vec_vec_v3d.size());

		// float配列に格納
		for (unsigned int i = 0; i < size; i++) {
			std::vector<Vector3Dd> vec_v3d((vec_vec_v3d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v3d.size(); j++) {
					buffer[counter++] = static_cast<float>(vec_v3d[j].y);
					buffer[counter++] = static_cast<float>(vec_v3d[j].z);
					buffer[counter++] = static_cast<float>(vec_v3d[j].x);
				}
			}
			else {
				counter += static_cast<int>(vec_v3d.size() * 3);
			}
		}
		return counter;
	}


	int CalcManager::getPolarCapNorthOpened(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->getPolarCapNorthStartVertices(), buffer);
	}


	int CalcManager::getPolarCapSouthClosed(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->getPolarCapSouthEndVertices(), buffer);
	}


	int CalcManager::getPolarCapNorthClosed(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->getPolarCapNorthEndVertices(), buffer);
	}


	int CalcManager::getPolarCapSouthOpened(float* buffer)
	{
		return getVertices3Dd(p_pulsar_->getPolarCapSouthStartVertices(), buffer);
	}


	int CalcManager::getVertices3Dd(std::vector<Vector3Dd>& vertices, float* buffer)
	{
		// 頂点数
		int counter(0);

		// float配列に格納
		if (buffer) {
			for (unsigned int i = 0; i < vertices.size(); i++) {
				buffer[counter++] = static_cast<float>(vertices[i].x);
				buffer[counter++] = static_cast<float>(vertices[i].y);
				buffer[counter++] = static_cast<float>(vertices[i].z);
			}
		}
		else {
			counter = static_cast<int>(vertices.size() * 3);
		}
		return counter;
	}


	int CalcManager::getSkyMap(float* buffer)
	{
		// スカイマップの頂点を取得
		std::vector<std::vector<Vector2Dd>> vec_vec_v2d;
		p_pulsar_->getSkyMapVertex(vec_vec_v2d);

		// 頂点数
		int counter(0);

		auto size(vec_vec_v2d.size());

		// float配列に格納
		for (unsigned int i = 0; i < size; i++) {
			std::vector<Vector2Dd> vec_v2d((vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = static_cast<float>(vec_v2d[j].x);
					buffer[counter++] = static_cast<float>(std::abs(vec_v2d[j].y - 180));	// 上下を反転
				}
			}
			else {
				counter += static_cast<int>(vec_v2d.size() * 2);
			}
		}
		return counter;
	}


	int CalcManager::getPulseProfile(bool normalize, float* buffer)
	{
		// 正規化
		if (normalize)
		{
			p_pulsar_->normalizePulse();
		}

		// パルスプロファイルの頂点を取得
		std::vector<std::vector<Vector2Dd>> vec_vec_v2d;
		p_pulsar_->getPulseVertex(vec_vec_v2d);

		// 頂点数
		int counter(0);

		auto size(vec_vec_v2d.size());

		// float配列に格納
		for (unsigned int i = 0; i < size; i++) {
			std::vector<Vector2Dd> vec_v2d((vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = static_cast<float>(vec_v2d[j].x);	// Phase
					buffer[counter++] = static_cast<float>(vec_v2d[j].y);	// Intensity(Relative)
				}
			}
			else {
				counter += static_cast<int>(vec_v2d.size() * 2);
			}
		}
		return counter;
	}
}