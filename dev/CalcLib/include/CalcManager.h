#pragma once

#include "PulsarAsset.h"
#include "ModelCalculator.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"


namespace CalcLib {

	/** @class CalcManager
	 *
	 *  @brief 計算を管理
	 */
	class CalcManager {

	public:
		/** @brief コンストラクタ */
		CalcManager();


		/** @brief デストラクタ */
		~CalcManager();


		/** @brief コピーの禁止 */
		CalcManager(const CalcManager&) = delete;
		CalcManager& operator=(const CalcManager&) = delete;


		/** @brief インスタンスの取得（シングルトン）
		 *
		 *  @return			CalcManagerのインスタンス
		 */
		static CalcManager& getInstance();


		/** @brief InclinationAngleの設定
		 *
		 *  @param[in]      degree		パルサーの回転軸と磁化軸のなす角 [度]
		 */
		void setInclinationAngle(int degree);


		/** @brief 磁力線の頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getMagneticLine(float* buffer);


		/** @brief ポーラーキャップ北極側の開磁力線の頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getPolarCapNorthOpened(float* buffer);


		/** @brief ポーラーキャップ南極側の閉磁力線の頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getPolarCapSouthClosed(float* buffer);


		/** @brief ポーラーキャップ北極側の閉磁力線の頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getPolarCapNorthClosed(float* buffer);


		/** @brief ポーラーキャップ南極側の開磁力線の頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getPolarCapSouthOpened(float* buffer);


		/** @brief スカイマップの頂点を取得
		 *
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getSkyMap(float* buffer);


		/** @brief パルスプロファイルの頂点を取得
		 *
		 *  @param[in]      normalize   正規化の有無
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         格納するデータ数
		 */
		int getPulseProfile(bool normalize, float* buffer);


	private:
		/** @brief パルサー情報 */
		PulsarAsset* p_pulsar_;

		/** @brief クリティカルセクション */
		CRITICAL_SECTION* p_critical_section_;


		/** @brief パルサーモデルの計算 */
		void calculatePulsarModel();


		/** @brief 頂点の3次元座標を配列で取得
		 *
		 *  @param[in]      vertices	3次元座標の可変長配列
		 *  @param[out]		buffer      値を格納するfloat配列
		 *
		 *  @return         頂点数
		 */
		int getVertices3Dd(std::vector<Vector3Dd>& vertices, float* buffer);
	};
}
