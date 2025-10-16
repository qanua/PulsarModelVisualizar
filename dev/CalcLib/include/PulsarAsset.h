#pragma once


#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

#include <vector>


/** @brief パルサークラス */
struct PulsarAsset
{
public:

	/** @class LCFL（LastClosedFieldLine）
	 *
	 *  @brief 光円柱に接するようにして閉じる磁力線
	 */
	struct LCFL
	{
	public:
		/** @brief LCFLの点群（3次元 double） */
		std::vector<Vector3Dd> vec_3dd_;


		/** @brief コンストラクタ */
		LCFL() {};


		/** @brief デストラクタ */
		~LCFL() {};
	};


	/** @class パルス波形
	 *
	 *  @brief OuterGapから放射される光子の観測結果
	 */
	struct Pulse
	{
	public:
		/** @brief 視線方向の傾き [degree] */
		int viewing_angle_;

		/** @brief パルス波形の点群（2次元 double） */
		std::vector<Vector2Dd> vec_2dd_;


		/** @brief コンストラクタ */
		Pulse() :viewing_angle_(0) {

			vec_2dd_.reserve(360);
		};


		/** @brief デストラクタ */
		~Pulse() {};
	};


	/** @class スカイマップ
	 *
	 *  @brief 磁力線上から放射された光子が観測される位相をマッピングしたもの
	 * 　　　　このときの磁力線はOuterGapの外側（UpperBoundary）を採用している
	 */
	struct SkyMap
	{
	public:
		/** @brief スカイマップの点群（2次元 double） */
		std::vector<Vector2Dd> vec_2dd_;


		/** @brief コンストラクタ */
		SkyMap() {};


		/** @brief デストラクタ */
		~SkyMap() {};
	};


	/** @brief 磁化軸の傾き [degree] */
	double inclination_angle_;

	/** @brief LCFLの点群 */
	std::vector<LCFL> vec_lcfls_;

	/** @brief パルス波形の点群 */
	std::vector<Pulse> vec_pulses_;

	/** @brief スカイマップの点群 */
	std::vector<SkyMap>	vec_skymaps_;

	/** @brief ポーラーキャップ北極側の始点 */
	LCFL vec_polarcap_n_start_;

	/** @brief ポーラーキャップ南極側の終点 */
	LCFL vec_polarcap_s_end_;

	/** @brief ポーラーキャップ南極側の始点 */
	LCFL vec_polarcap_s_start_;

	/** @brief ポーラーキャップ北極側の終点 */
	LCFL vec_polarcap_n_end_;

	/** @brief 片極の磁力線の本数 */
	const int MAGNETIC_LINE_COUNT = 60;


	/** @brief コンストラクタ */
	PulsarAsset() :inclination_angle_(0.0), max_photon_count_(0.0) {

		// 両極の点群を格納するためのメモリ確保
		vec_lcfls_.reserve(MAGNETIC_LINE_COUNT * 2);
		vec_skymaps_.reserve(MAGNETIC_LINE_COUNT * 2);

		// パルス波形にを初期値(0)を格納
		for (int i = 0; i < 180; i++) {

			Pulse* p_pulse(new Pulse);

			for (int j = 0; j < 360; j++) {

				// pos(x, y) -> x: 位相 y: 光子数(Intensity)
				Vector2Dd pos((double)j, 0.0);
				p_pulse->vec_2dd_.emplace_back(pos);
			}
			vec_pulses_.emplace_back(*p_pulse);
		}
	}


	/** @brief デストラクタ */
	~PulsarAsset() {}


	/** @brief LCFLの点群を取得
	 *
	 *  @param[out]		vec_vec_vertex		LCFLの点群
	 */
	void getLCFLVertices(std::vector<std::vector<Vector3Dd>>& vec_vec_vertex)
	{
		// 一時データ
		std::vector<std::vector<Vector3Dd>> vec_vec_temp;

		size_t line_count(vec_lcfls_.size());
		vec_vec_temp.reserve(line_count);

		for (size_t i = 0; i < line_count; i++) {

			// 一時データ
			std::vector<Vector3Dd> vertex_temp;
			size_t vertex_count(vec_lcfls_[i].vec_3dd_.size());
			vertex_temp.reserve(vertex_count);

			// 点群の集約
			for (size_t j = 0; j < vertex_count; j++) {

				Vector3Dd* const p_vertex = &(vec_lcfls_[i].vec_3dd_[j]);
				vertex_temp.emplace_back(p_vertex);
			}

			vec_vec_temp.emplace_back(vertex_temp);
		}

		// 点群の格納
		vec_vec_vertex = std::move(vec_vec_temp);
	}


	/** @brief ポーラーキャップ北極側の始点を取得
	 *
	 *  @return			ポーラーキャップの点群
	 */
	inline std::vector<Vector3Dd>& getPolarCapNorthStartVertices() {

		return vec_polarcap_n_start_.vec_3dd_;
	}


	/** @brief ポーラーキャップ南極側の終点を取得
	 *
	 *  @return			ポーラーキャップの点群
	 */
	inline std::vector<Vector3Dd>& getPolarCapSouthEndVertices() {

		return vec_polarcap_s_end_.vec_3dd_;
	}


	/** @brief ポーラーキャップ北極側の終点を取得
	 *
	 *  @return			ポーラーキャップの点群
	 */
	inline std::vector<Vector3Dd>& getPolarCapNorthEndVertices() {

		return vec_polarcap_n_end_.vec_3dd_;
	}


	/** @brief ポーラーキャップ南極側の始点を取得
	 *
	 *  @return			ポーラーキャップの点群
	 */
	inline std::vector<Vector3Dd>& getPolarCapSouthStartVertices() {

		return vec_polarcap_s_start_.vec_3dd_;
	}


	/** @brief スカイマップの点群を取得
	 *
	 *  @param[out]		vec_vec_vertex		スカイマップの点群
	 */
	void getSkyMapVertex( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex )
	{
		// 一時データ
		std::vector<std::vector<Vector2Dd>> vec_vec_temp;

		size_t line_count(vec_skymaps_.size());
		vec_vec_temp.reserve(line_count);

		for (size_t i = 0; i < line_count; i++) {

			// 一時データ
			std::vector<Vector2Dd> vertex_temp;

			size_t vertex_count(vec_skymaps_[i].vec_2dd_.size());
			vertex_temp.reserve(vertex_count);

			// 点群の集約
			for (size_t j = 0; j < vertex_count; j++) {

				Vector2Dd* const p_vertex = &(vec_skymaps_[i].vec_2dd_[j]);
				vertex_temp.emplace_back(p_vertex);
			}

			vec_vec_temp.emplace_back(vertex_temp);
		}

		// 点群の格納
		vec_vec_vertex = std::move(vec_vec_temp);
	}


	/** @brief パルス波形の点群を取得
	 *
	 *  @param[out]		vec_vec_vertex		パルス波形の点群
	 */
	void getPulseVertex( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex )
	{
		// 一時データ
		std::vector<std::vector<Vector2Dd>> vec_vec_temp;

		size_t line_count(vec_pulses_.size());
		vec_vec_temp.reserve(line_count);

		for (size_t i = 0; i < line_count; i++) {

			// 一時データ
			std::vector<Vector2Dd> vertex_temp;

			size_t vertex_count(vec_pulses_[i].vec_2dd_.size());
			vertex_temp.reserve(vertex_count);

			// 点群の集約
			for (size_t j = 0; j < vertex_count; j++) {

				Vector2Dd* const p_vertex = &(vec_pulses_[i].vec_2dd_[j]);
				vertex_temp.emplace_back(&vec_pulses_[i].vec_2dd_[j]);
			}

			vec_vec_temp.emplace_back(vertex_temp);
		}

		// 点群の格納
		vec_vec_vertex = std::move(vec_vec_temp);
	}


	/** @brief パルス波形の追加
	 *
	 *  @param[in]		vec_pulse		追加するパルス波形
	 */
	void addPulse( std::vector<Pulse> vec_pulse)
	{
		for (size_t i = 0; i < vec_pulse.size(); i++) {

			Pulse add_pulse(vec_pulse[i]);

			for (size_t j = 0; j < add_pulse.vec_2dd_.size(); j++) {

				int viewing_angle(add_pulse.viewing_angle_);
				int	phase((int)add_pulse.vec_2dd_[j].x);

				if ((0 <= phase) && (phase <= 360) &&
					(0 <= viewing_angle) && (viewing_angle <= 180)) {

					// 光子数の合算
					vec_pulses_[viewing_angle].vec_2dd_[phase].y += add_pulse.vec_2dd_[j].y;

					double count(vec_pulses_[viewing_angle].vec_2dd_[phase].y);

					// 最大光子数の更新（正規化に使用する）
					if (max_photon_count_ < count) {

						max_photon_count_ = count;
					}
				}
			}
		}
	}


	/** @brief パルス波形の正規化 */
	void normalizePulse()
	{
		if (0 < max_photon_count_) {

			size_t pulse_count = vec_pulses_.size();

			// 180本のパルス波形を探索
			for (size_t i = 0; i < pulse_count; i++)
			{
				Pulse* p_pulse(&vec_pulses_[i]);

				size_t phase_count = p_pulse->vec_2dd_.size();

				// 360度の位相を走査して光子数を正規化する
				for (size_t phase = 0; phase < phase_count; phase++) {

					p_pulse->vec_2dd_[phase].y /= max_photon_count_ * 0.1;
				}
			}

			// 初期化
			max_photon_count_ = 0;
		}
	}


	/** @brief パルス波形の初期化 */
	void resetPulse()
	{
		size_t pulse_count = vec_pulses_.size();

		// 180本のパルス波形を探索
		for (size_t index = 0; index < pulse_count; index++) {

			Pulse pulse(vec_pulses_[index]);

			size_t phase_count = pulse.vec_2dd_.size();

			// 360度の位相を走査して光子数をリセットする
			for (size_t phase = 0; phase < phase_count; phase++) {

				pulse.vec_2dd_[phase].y = 0.0;
			}
		}
	}


private:

	/** @brief パルス波形の最大光子数
	 *
	 *  正規化に使用する
	 */
	double max_photon_count_;
};