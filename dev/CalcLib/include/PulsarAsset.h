#pragma once

#include "Data/MagneticLine.h"
#include "Data/Pulse.h"
#include "Data/SkyMap.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

#include <vector>


/** @brief パルサークラス */
struct Pulsar
{
public:
	double									m_InclinationAngle;			// 磁化軸の傾き   [degree]
	double									m_ViewingAngle;				// 視線方向の傾き [degree]
	int										m_MagneticLineCount;		// 磁力線の本数

	std::vector<MagneticLine>				m_vecMagneticLine;			// 磁力線
	std::vector<Pulse>						m_vecPulse;					// パルス
	std::vector<SkyMap>						m_vecSkyMap;				// スカイマップ
	MagneticLine							m_vecPolarCapNorthBegin;	// ポーラーキャップ北始点
	MagneticLine							m_vecPolarCapNorthEnd;		// ポーラーキャップ北終点
	MagneticLine							m_vecPolarCapSouthBegin;	// ポーラーキャップ南始点
	MagneticLine							m_vecPolarCapSouthEnd;		// ポーラーキャップ南終点


private:
	double									m_MaxPhotonCount;


public:
	/** @brief コンストラクタ */
	Pulsar() :m_InclinationAngle(					  0.0 )
	,		m_ViewingAngle(					  0.0 )
	,  m_MagneticLineCount(					    0 )
	,	  m_MaxPhotonCount(					  0.0 )
	{
		// 各種データ
		 m_vecMagneticLine.reserve( m_MagneticLineCount );
		       m_vecSkyMap.reserve( m_MagneticLineCount );

		for( int i = 0; i < 180; i++ )
		{
			Pulse* p_pulse( new Pulse );
			for( int j = 0; j < 360; j++ )
			{
				Vector2Dd pos( (double)j, 0.0 );
				p_pulse->m_vecVertex2D.emplace_back( pos );
			}
			m_vecPulse.emplace_back( *p_pulse );
		}
	}


	/** @brief デストラクタ */
	~Pulsar()
	{
	}


public:
	/** @brief 磁力線の点群データ取得
	 *
	 *  @param[out]			vec_vec_vertex  磁力線の点群データ
	 */
	void GetMagneticLineVertex( std::vector<std::vector<Vector3Dd>>& vec_vec_vertex )
	{
		// 一時データの生成、サイズ確保
		std::vector<std::vector<Vector3Dd>> vec_vec_temp;
		size_t line_count( m_vecMagneticLine.size() );
		vec_vec_temp.reserve( line_count );

		for( size_t i = 0; i < line_count; i++ )
		{
			// 一時データの生成、サイズ確保
			std::vector<Vector3Dd> vertex_temp;
			size_t vertex_count( m_vecMagneticLine[i].m_vecVertex3D.size() );
			vertex_temp.reserve( vertex_count );

			for( size_t j = 0; j < vertex_count; j++ )
			{
				// 点群データの格納
				Vector3Dd* const p_vertex = &( m_vecMagneticLine[i].m_vecVertex3D[j] );
				vertex_temp.emplace_back( p_vertex );
			}
			vec_vec_temp.emplace_back( vertex_temp );
		}
		vec_vec_vertex = std::move( vec_vec_temp );
	}


	/** @brief ポーラーキャップ北始点の取得 */
	std::vector<Vector3Dd>& GetPolarCapNorthBeginVertex(){ return m_vecPolarCapNorthBegin.m_vecVertex3D; }


	/** @brief ポーラーキャップ北終点の取得 */
	std::vector<Vector3Dd>& GetPolarCapNorthEndVertex(){ return m_vecPolarCapNorthEnd.m_vecVertex3D; }


	/** @brief ポーラーキャップ南始点の取得 */
	std::vector<Vector3Dd>& GetPolarCapSouthBeginVertex(){ return m_vecPolarCapSouthBegin.m_vecVertex3D; }


	/** @brief ポーラーキャップ南終点の取得 */
	std::vector<Vector3Dd>& GetPolarCapSouthEndVertex(){ return m_vecPolarCapSouthEnd.m_vecVertex3D; }


	/** @brief スカイマップの点群データ取得
	 *
	 *  @param[out]			vec_vec_vertex  スカイマップの点群データ
	 */
	void GetSkyMapVertex( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex )
	{
		// 一時データの生成、サイズ確保
		std::vector<std::vector<Vector2Dd>> vec_vec_temp;
		size_t line_count( m_vecSkyMap.size() );
		vec_vec_temp.reserve( line_count );

		for( size_t i = 0; i < line_count; i++ )
		{
			// 一時データの生成、サイズ確保
			std::vector<Vector2Dd> vertex_temp;
			size_t vertex_count( m_vecSkyMap[i].m_vecVertex2D.size() );
			vertex_temp.reserve( vertex_count );

			for( size_t j = 0; j < vertex_count; j++ )
			{
				// 点群データの格納
				Vector2Dd* const p_vertex = &( m_vecSkyMap[i].m_vecVertex2D[j] );
				vertex_temp.emplace_back( p_vertex );
			}
			vec_vec_temp.emplace_back( vertex_temp );
		}
		vec_vec_vertex = std::move( vec_vec_temp );
	}


	/** @brief パルス波形の点群データ取得
	 *
	 *  @param[out]			vec_vec_vertex  パルス波形の点群データ
	 */
	void GetPulseVertex( std::vector<std::vector<Vector2Dd>>& vec_vec_vertex )
	{
		// 一時データの生成、サイズ確保
		std::vector<std::vector<Vector2Dd>> vec_vec_temp;
		size_t line_count( m_vecPulse.size() );
		vec_vec_temp.reserve( line_count );

		for( size_t i = 0; i < line_count; i++ )
		{
			// 一時データの生成、サイズ確保
			std::vector<Vector2Dd> vertex_temp;
			size_t vertex_count( m_vecPulse[i].m_vecVertex2D.size() );
			vertex_temp.reserve( vertex_count );

			for( size_t j = 0; j < vertex_count; j++ )
			{
				// 点群データの格納
				Vector2Dd* const p_vertex = &( m_vecPulse[i].m_vecVertex2D[j] );
				vertex_temp.emplace_back( &m_vecPulse[i].m_vecVertex2D[j] );
			}
			vec_vec_temp.emplace_back( vertex_temp );
		}
		vec_vec_vertex = std::move( vec_vec_temp );
	}


public:
	/** @brief パルスの追加 */
	void AddPulse( std::vector<Pulse> vec_pulse)
	{
		for( size_t i = 0; i < vec_pulse.size(); i++ )
		{
			Pulse add_pulse( vec_pulse[i] );
			for( size_t j = 0; j < add_pulse.m_vecVertex2D.size(); j++ )
			{
				int viewing_angle( (int)add_pulse.m_ViewingAngle );
				int			phase( (int)add_pulse.m_vecVertex2D[j].x );

				if( ( 0 <= phase		 ) && (			phase <= 360 ) && 
					( 0 <= viewing_angle ) && ( viewing_angle <= 180 ) )
				{
					// 光子数の合算
					m_vecPulse[viewing_angle].m_vecVertex2D[phase].y += add_pulse.m_vecVertex2D[j].y;

					// 最大光子数の更新（正規化に使用）
					double count( m_vecPulse[viewing_angle].m_vecVertex2D[phase].y );
					if( m_MaxPhotonCount < count ) m_MaxPhotonCount = count;
				}
			}
		}
	}


	/** @brief 正規化 */
	void NormalizePulse()
	{
		if( m_MaxPhotonCount <= 0 ) return;

		// 180本のパルス波形を探索
		for( size_t i = 0; i < m_vecPulse.size(); i++ )
		{
			Pulse* p_pulse( &m_vecPulse[i] );

			// 360度の位相を走査して光子数を正規化する
			for( size_t phase = 0; phase < p_pulse->m_vecVertex2D.size(); phase++ )
				p_pulse->m_vecVertex2D[phase].y /= m_MaxPhotonCount * 0.1;
		}

		// 初期化
		m_MaxPhotonCount = 0;
	}


	/** @brief 初期化 */
	void ResetPulse()
	{
		// 180本のパルス波形を探索
		for( size_t index = 0; index < m_vecPulse.size(); index++ )
		{
			Pulse pulse( m_vecPulse[index] );

			// 360度の位相を走査して光子数をリセットする
			for( size_t phase = 0; phase < pulse.m_vecVertex2D.size(); phase++ )
				pulse.m_vecVertex2D[phase].y = 0.0;
		}
	}
};