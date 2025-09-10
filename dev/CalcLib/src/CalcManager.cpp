#pragma once

#include "pch.h"
#include "CalcManager.h"

namespace ModelGenerator {

#pragma region 初期化 / 終了処理
	// コンストラクタ
	CalcManager::CalcManager()
		: m_pPulsar(				   new Pulsar())
		//, m_pPulseCalculator( new PulseCalculator())
		, m_pCriticalSection(new CRITICAL_SECTION())
		//, m_pPulsarModel(				new Scene())
		//, m_pPolarCapNorthBegin(		new Scene())
		//, m_pPolarCapNorthEnd(			new Scene())
		//, m_pPolarCapSouthBegin(		new Scene())
		//, m_pPolarCapSouthEnd(			new Scene())
		//, m_pSkyMap(					new Scene())
		//, m_pPulseProfile(				new Scene())
	{
		// クリティカルセクションの初期化
		::InitializeCriticalSection( m_pCriticalSection );

		// 初期設定
		SetupPulsar();

		// シーン生成
		CreateScene();
	}


	// デストラクタ
	CalcManager::~CalcManager()
	{
		// リソースの開放
		delete m_pPulsar;
		//delete m_pPulseCalculator;
		//delete m_pPulsarModel;
		//delete m_pPolarCapNorthBegin;
		//delete m_pPolarCapNorthEnd;
		//delete m_pPolarCapSouthBegin;
		//delete m_pPolarCapSouthEnd;
		//delete m_pSkyMap;
		//delete m_pPulseProfile;

		// クリティカルセクションの破棄
		::DeleteCriticalSection(m_pCriticalSection);
		delete m_pCriticalSection;
	}


	// インスタンスの取得
	CalcManager& CalcManager::GetInstance()
	{
		static CalcManager instance;
		return instance;
	}
#pragma endregion


	// パルサーデータ初期設定
	void CalcManager::SetupPulsar() {
		m_pPulsar->m_InclinationAngle = INIT_INCLINATION_ANGLE;
		m_pPulsar->m_ViewingAngle = INIT_VIEWING_ANGLE;
		m_pPulsar->m_MagneticLineCount = INIT_MAGNETIC_LINE_COUNT;
	}


	// シーン生成
	void CalcManager::CreateScene() {
		// パルス計算
		PulseCalculator calclator;
		calclator.GetPulsarInfo(*m_pPulsar);

		// 構成要素の生成
		//CreatePolarCap();		// ポーラーキャップ
		//CreateSkyMap();			// スカイマップ
		//CreatePulse();			// パルス
	}


	// ポーラーキャップの生成
	void CalcManager::CreatePolarCap() {
		//using polar_t = Scene::PolarCap;

		//for (int i = 0; i < 4; i++) {
		//	std::vector<Vector3Dd>* vec_polar(nullptr);
		//	Scene* p_scene(nullptr);

		//	switch (i) {
		//	case 0:
		//		vec_polar = &(m_pPulsar->GetPolarCapNorthBeginVertex());
		//		p_scene = m_pPolarCapNorthBegin;
		//		break;
		//	case 1:
		//		vec_polar = &(m_pPulsar->GetPolarCapNorthEndVertex());
		//		p_scene = m_pPolarCapNorthEnd;
		//		break;
		//	case 2:
		//		vec_polar = &(m_pPulsar->GetPolarCapSouthBeginVertex());
		//		p_scene = m_pPolarCapSouthBegin;
		//		break;
		//	case 3:
		//		vec_polar = &(m_pPulsar->GetPolarCapSouthEndVertex());
		//		p_scene = m_pPolarCapSouthEnd;
		//		break;
		//	}

		//	Shape* p_shape(nullptr);
		//	for (int j = 0; j < (int)polar_t::COUNT; j++) {
		//		switch (j) {
		//		case(int)polar_t::MAGNETIC_LINE: {
		//			Vertex* p_vertex(new Vertex());

		//			p_vertex->SetSingleLine3D(*vec_polar);

		//			p_shape = p_vertex;
		//		}
		//		break;
		//		}
		//	}
		//	p_scene->SetElement(p_shape);
		//}
	}


	// スカイマップの生成
	void CalcManager::CreateSkyMap() {
		//using sky_t = Scene::SkyMap;

		//Shape* p_shape;
		//for (int j = 0; j < (int)sky_t::COUNT; j++)
		//{
		//	switch (j)
		//	{
		//	case(int)sky_t::MAGNETIC_LINE:
		//	{
		//		Vertex* p_vertex(new Vertex(line_count_t::MULTIPLE, line_vec_t::XY, line_t::SOLID));

		//		p_vertex->SetMainColor(0, 255, 0, 1.0);
		//		p_vertex->SetWidth(0.5);

		//		std::vector<std::vector<Vector2Dd>>* p_vec_vec_line(&p_vertex->GetMultipleLine2D());
		//		m_pPulsar->GetSkyMapVertex(*p_vec_vec_line);
		//		p_vertex->SetMultipleLine2D(*p_vec_vec_line);

		//		p_shape = p_vertex;
		//	}
		//	break;
		//	}
		//}
		//m_pSkyMap->SetElement(p_shape);
	}


	// パルスの生成
	void CalcManager::CreatePulse() {
		//using pulse_t = Scene::Pulse;

		//Shape* p_shape;
		//for (int j = 0; j < (int)pulse_t::COUNT; j++)
		//{
		//	switch (j)
		//	{
		//	case(int)pulse_t::PULSE:
		//	{
		//		Vertex* p_vertex(new Vertex(line_count_t::MULTIPLE, line_vec_t::XZ, line_t::SOLID));

		//		p_vertex->SetMainColor(0, 255, 0, 1.0);
		//		p_vertex->SetSubColor(0, 0, 0, 1.0);
		//		p_vertex->SetWidth(0.5);

		//		m_pPulsar->NormalizePulse();

		//		std::vector<std::vector<Vector2Dd>>* p_vec_vec_line(&p_vertex->GetMultipleLine2D());
		//		m_pPulsar->GetPulseVertex(*p_vec_vec_line);

		//		p_vertex->SetMultipleLine2D(*p_vec_vec_line);

		//		std::vector<double> y_dimension;
		//		size_t size(p_vec_vec_line->size());
		//		y_dimension.reserve(size);
		//		for (size_t i = 0; i < size; i++)
		//			y_dimension.emplace_back(PULSE_DISPLAY_DEPTH * i);

		//		p_vertex->SetThridDimensionMultipleValue(y_dimension);

		//		p_shape = p_vertex;
		//	}
		//	break;
		//	}
		//}
		//m_pPulseProfile->SetElement(p_shape);
	}

	int CalcManager::GetMagneticLine(float* buffer)
	{
		std::vector<std::vector<Vector3Dd>>* p_vec_vec_v3d(new std::vector<std::vector<Vector3Dd>>);
		m_pPulsar->GetMagneticLineVertex(*p_vec_vec_v3d);
		int counter(0);

		for (unsigned int i = 0; i < p_vec_vec_v3d->size(); i++) {
			std::vector<Vector3Dd> vec_v3d((*p_vec_vec_v3d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v3d.size(); j++) {
					buffer[counter++] = vec_v3d[j].x;
					buffer[counter++] = vec_v3d[j].y;
					buffer[counter++] = vec_v3d[j].z;
				}
			}
			else {
				counter += vec_v3d.size() * 3;
			}
		}
		return counter;
	}

	int CalcManager::GetPolarCapNorthOpened(float* buffer)
	{
		return GetVertices3Dd(buffer, m_pPulsar->GetPolarCapNorthBeginVertex());
	}

	int CalcManager::GetPolarCapNorthClosed(float* buffer)
	{
		return GetVertices3Dd(buffer, m_pPulsar->GetPolarCapNorthEndVertex());
	}

	int CalcManager::GetPolarCapSouthOpened(float* buffer)
	{
		return GetVertices3Dd(buffer, m_pPulsar->GetPolarCapSouthBeginVertex());
	}

	int CalcManager::GetPolarCapSouthClosed(float* buffer)
	{
		return GetVertices3Dd(buffer, m_pPulsar->GetPolarCapSouthEndVertex());
	}

	int CalcManager::GetVertices3Dd(float* buffer, std::vector<Vector3Dd>& vertices)
	{
		int counter(0);

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

    int CalcManager::GetSkyMap(float* buffer)
    {
		std::vector<std::vector<Vector2Dd>>* p_vec_vec_v2d(new std::vector<std::vector<Vector2Dd>>);
		m_pPulsar->GetSkyMapVertex(*p_vec_vec_v2d);
		int counter(0);

		for (unsigned int i = 0; i < p_vec_vec_v2d->size(); i++) {
			std::vector<Vector2Dd> vec_v2d((*p_vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = vec_v2d[j].x;
					buffer[counter++] = vec_v2d[j].y;
				}
			}
			else {
				counter += vec_v2d.size() * 2;
			}
		}
		return counter;
    }

	int CalcManager::GetPulseProfile(float* buffer, bool normalize)
	{
		std::vector<std::vector<Vector2Dd>>* p_vec_vec_v2d(new std::vector<std::vector<Vector2Dd>>);

		if (normalize)
		{
			m_pPulsar->NormalizePulse();
		}
		m_pPulsar->GetPulseVertex(*p_vec_vec_v2d);
		int counter(0);

		for (unsigned int i = 0; i < p_vec_vec_v2d->size(); i++) {
			std::vector<Vector2Dd> vec_v2d((*p_vec_vec_v2d)[i]);

			if (buffer) {
				for (unsigned int j = 0; j < vec_v2d.size(); j++) {
					buffer[counter++] = vec_v2d[j].x;
					buffer[counter++] = vec_v2d[j].y + i;
				}
			}
			else {
				counter += vec_v2d.size() * 2;
			}
		}
		return counter;
	}
#pragma endregion
}