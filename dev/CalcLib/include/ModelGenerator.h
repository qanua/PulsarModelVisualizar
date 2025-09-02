#pragma once

#include "Data/Pulsar.h"
#include "PulseCalculator.h"
#include "Scene/Scene.h"
#include "Primitive/Shape.h"
#include "Primitive/Vertex.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

namespace ModelGenerator {

	/// <summary>
	/// CalcManager の概要
	/// </summary>
	class CalcManager {

#pragma region メンバ変数
	private:
		Pulsar* m_pPulsar;							// パルサー
		PulseCalculator* m_pPulseCalculator;		// パルス計算機
		CRITICAL_SECTION* m_pCriticalSection;		// クリティカルセクション

		Scene* m_pPulsarModel;						// パルサーモデル
		Scene* m_pPolarCapNorthBegin;				// ポーラーキャップ北始点
		Scene* m_pPolarCapNorthEnd;					// ポーラーキャップ北終点
		Scene* m_pPolarCapSouthBegin;				// ポーラーキャップ南始点
		Scene* m_pPolarCapSouthEnd;					// ポーラーキャップ南終点
		Scene* m_pSkyMap;							// スカイマップ
		Scene* m_pPulseProfile;						// パルス波形
#pragma endregion


#pragma region 定数
		const double INIT_INCLINATION_ANGLE = 57.0;	// 磁化軸の傾き   [degree]
		const double INIT_VIEWING_ANGLE = 57.0;		// 視線方向の傾き [degree]
		const int INIT_MAGNETIC_LINE_COUNT = 60;	// 磁力線の本数
		const double PULSE_DISPLAY_DEPTH = 1.0;		// パルス表示間隔
#pragma endregion


#pragma region 初期化 / 終了処理
	private:
		/** @brief コンストラクタ */
		CalcManager();

		// コピー禁止
		CalcManager(const CalcManager&) = delete;
		CalcManager& operator=(const CalcManager&) = delete;

	protected:
		/** @brief デストラクタ */
		~CalcManager();

	public:
		/** @brief インスタンスの取得 */
		static CalcManager& GetInstance();
#pragma endregion


	private:
		// 設定・更新
		void SetupPulsar();
		void UpdateInclinationAngle();

	private:
		// シーン生成
		inline Scene* GetScene(int type);
		inline double GetPolarAngle(int type);

		void CreateScene();
		void CreatePulsarModel();
		void CreatePolarCap();
		void CreateSkyMap();
		void CreatePulse();

	public:
		int GetMagneticLine(float* buffer);
		int GetMagneticLineVerticesCount();
		void GetMagneticLineVertices(float* buffer);
	};
}
