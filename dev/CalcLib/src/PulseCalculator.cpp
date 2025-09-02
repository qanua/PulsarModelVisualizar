#pragma once

#include "pch.h"
#include "PulseCalculator.h"
#include "Math/RungeKutta.h"

#include <iostream>
#include <iterator>


// コンストラクタ
PulseCalculator::PulseCalculator()
{
	// クリティカルセクションの初期化
	::InitializeCriticalSection( &m_CriticalSection );
}


// デストラクタ
PulseCalculator::~PulseCalculator()
{
	// クリティカルセクションの破棄
	::DeleteCriticalSection( &m_CriticalSection );
}


// パルスの取得
void
PulseCalculator::GetPulsarInfo( Pulsar& pulsar )
{
	//using thread_t = System::Threading::Thread;
	//using start_t  = System::Threading::ThreadStart;

	// 初期化
	pulsar.m_vecMagneticLine.clear();
	pulsar.m_vecSkyMap.clear();
	pulsar.m_vecPolarCapNorthBegin.m_vecVertex3D.clear();
	pulsar.m_vecPolarCapNorthEnd.m_vecVertex3D.clear();
	pulsar.m_vecPolarCapSouthBegin.m_vecVertex3D.clear();
	pulsar.m_vecPolarCapSouthEnd.m_vecVertex3D.clear();
	pulsar.ResetPulse();

	const int	 count( pulsar.m_MagneticLineCount );	// 磁力線の本数
	const double dphi ( 360.0 / count );				// 方位角ステップ

	// 方位角方向にLCFLを探索
	for( int i = 0; i < count; i++ )
	{
		const double phi( dphi * i );					// LCFLの方位角
		CalculatePulse( pulsar, phi );
	}
}


// パルス計算
void
PulseCalculator::CalculatePulse( Pulsar& pulsar, double phi )
{
	using polar_t  = PulseInfo::PolarCapTraceType;
	using status_t = PulseInfo::MagneticLineStatus;

	// LCFL 算出のための角度情報
	Angle angle;
	angle.azimth = phi * RADIAN;
	angle.polar	 = POLAR_ANGLE * RADIAN;

	for( size_t i = 0; i < (int)polar_t::COUNT; i++ )
	{
		angle.inclination = ( pulsar.m_InclinationAngle + 180.0 * i ) * RADIAN;

		// 磁気モーメント
		m_MagneticMoment.x = sin( angle.inclination );
		m_MagneticMoment.z = cos( angle.inclination );

		double  open_angle(   POLAR_ANGLE );			// 二分法の始点
		double close_angle( EQUATOR_ANGLE );			// 二分法の終点

		if( CheckExistLCFL( angle, open_angle, close_angle ) )
		{
			PulseInfo info;
			//info.polar_cap_trace_type = ( (polar_t)i );
			info.SetFlag( true, false, false );

			// 二分法探索開始
			for( int i = 0; i < DICHOTOMY_STEP; i++ )
			{
				// 中点の更新
				double theta( ( open_angle + close_angle ) * 0.5 );
				angle.polar = theta;

				// L磁力線の極座標系位置の取得
				GetCartesianPosition( angle, info.magnetic_field_pos );

				// 磁力線の開閉状態によって探索範囲を更新する
				GetPulseInfo( info );

				if( info.magnetic_line_status == status_t::OPEN ) open_angle = theta;
				else close_angle = theta;
			}

			pulsar.m_vecMagneticLine.emplace_back( info.lcfl );

			if( i == (int)polar_t::FROM_NORTH )
			{
				pulsar.m_vecPolarCapNorthBegin.m_vecVertex3D.emplace_back( info.lcfl.m_vecVertex3D.front() );
				pulsar.m_vecPolarCapSouthEnd.m_vecVertex3D.emplace_back( info.lcfl.m_vecVertex3D.back()  );
			}
			else
			{
				pulsar.m_vecPolarCapNorthEnd.m_vecVertex3D.emplace_back( info.lcfl.m_vecVertex3D.back() );
				pulsar.m_vecPolarCapSouthBegin.m_vecVertex3D.emplace_back( info.lcfl.m_vecVertex3D.front() );
			}

			info.SetFlag( false, false, true );
			for( int i = 0; i < OUTER_GAP_LAYER_COUNT; i++ )
			{
				if( i == OUTER_GAP_LAYER_COUNT - 1 ) info.SetFlag( false, true, true );

				angle.polar -= OUTER_GAP_LAYER_THICKNESS * RADIAN;
				GetPulse( angle, info );
				pulsar.m_vecSkyMap.emplace_back( info.skymap );
				pulsar.AddPulse( info.vec_pulse );
			}
		}
		else
		{
			// LCFLなし
		}
	}
}


// パルスの取得
void
PulseCalculator::GetPulse( Angle& angle, PulseInfo& info )
{
	GetCartesianPosition( angle, info.magnetic_field_pos );
	GetPulseInfo( info );
}


// LCFLの存在確認
bool
PulseCalculator::CheckExistLCFL( Angle& angle, double open_angle, double close_angle )
{
	using status_t = PulseInfo::MagneticLineStatus;

	PulseInfo info;

	// 極軸付近の磁力線が開いているかどうか
	angle.polar = open_angle;
	GetCartesianPosition( angle, info.magnetic_field_pos );
	GetPulseInfo( info );

	if( info.magnetic_line_status == status_t::OPEN )
	{
		// 赤道付近の磁力線が閉じているかどうか
		angle.polar = close_angle;
		GetCartesianPosition( angle, info.magnetic_field_pos );
		GetPulseInfo( info );

		if( info.magnetic_line_status == status_t::CLOSE)
			return true;
	}
	return false;
}


// パルス情報の取得
void
PulseCalculator::GetPulseInfo( PulseInfo& info )
{
	using status_t = PulseInfo::MagneticLineStatus;

	Vector3Dd        pos( info.magnetic_field_pos );
	const bool   is_lcfl( info.IsLCFL() );
	const bool is_skymap( info.IsSkyMap() );
	const bool  is_pulse( info.IsPulse() );

	double    t( 0.0 );
	Vector3Dd curr_Bv;
	Vector3Dd prev_Bv;
	double    curr_Bphi( 0.0 );
	double    prev_Bphi( 0.0 );

	bool is_emit( false );

	const auto fn( [&]( double t, Vector3Dd v, Vector3Dd& Bv ){ GetMagneticField( t, v, Bv ); } );
	const int step_max( (int)( (double)sqrt( LIGHT_CYRINDER_RADIUS * LIGHT_CYRINDER_RADIUS ) / std::abs( STEP_LENGTH ) * 10.0 ) );

	MagneticLine line;
	line.m_vecVertex3D.reserve( step_max );

	SkyMap skymap;
	skymap.m_vecVertex2D.reserve( step_max );

	std::vector<Pulse> vec_pulse;
	vec_pulse.reserve( 180 );

	for( int i = 0; i < step_max; i++ )
	{
		// フラグの確認
		if( is_lcfl )
		{
			line.m_vecVertex3D.emplace_back( pos );
		}
		if( is_skymap || is_pulse )
		{
			if( i == 0 ) GetMagneticField( 0, pos, curr_Bv );

			curr_Bphi = ( pos.x * curr_Bv.x + pos.y * curr_Bv.y ) / sqrt( pos.x * pos.x + pos.y * pos.y );

			const bool   is_null( ( prev_Bv.z * curr_Bv.z ) < 0.0 );
			const bool is_return( ( prev_Bphi * curr_Bphi ) < 0.0 );

			if( !is_emit && is_null ) is_emit = true;
			else if( is_emit && is_return ) is_emit = false;

			if( is_emit )
			{
				CreateSkyMap( pos, curr_Bv, skymap.m_vecVertex2D );
				if( is_pulse ) CreatePulse( prev_Bv, curr_Bv, skymap.m_vecVertex2D.back(), vec_pulse );
			}

			prev_Bv   = curr_Bv;
			prev_Bphi = curr_Bphi;
		}

		// 積分終了判定
		if( i != 0 )
		{
			// 閉じた磁力線
			if( pos.Length() <= STAR_RADIUS )
			{
				if( is_lcfl   ) info.lcfl	   = std::move( line      );
				if( is_skymap ) info.skymap    = std::move( skymap    );
				if( is_pulse  ) info.vec_pulse = std::move( vec_pulse );

				info.magnetic_line_status = status_t::CLOSE;
				break;
			}
			// 想定内の開いた磁力線
			else if( LIGHT_CYRINDER_RADIUS <= sqrt( ( pos.x * pos.x ) + ( pos.y * pos.y ) ) )
			{
				if( is_skymap ) info.skymap	   = std::move( skymap	  );
				if( is_pulse  ) info.vec_pulse = std::move( vec_pulse );

				info.magnetic_line_status = status_t::OPEN;
				break;
			}
		}

		// 磁場方向の積分
		if( i < ( step_max - 1 ) ) RungeKutta( pos, t, STEP_LENGTH, curr_Bv, fn );
		else info.magnetic_line_status = status_t::OPEN;
	}
}


bool
PulseCalculator::GetMagneticField( double /*t*/, Vector3Dd v, Vector3Dd& Bv )
{
	const double r( v.Length() );
	if( r != 0 )
	{
		const double t ( STAR_RADIUS - r );
		const double sint( sin( t ) );
		const double cost( cos( t ) );
		const double R2(  r * r );
		const double R3( R2 * r );

		Bv.x = m_MagneticMoment.x * ( cost * ( 1.0 / R3 - 1.0 / r ) - sint / R2 );
		Bv.y = m_MagneticMoment.x * ( sint * ( 1.0 / R3 - 1.0 / r ) + cost / R2 );
		Bv.z = m_MagneticMoment.z / R3;

		const double A( ( Bv.Dot( v ) ) * 3.0 / r + 2.0 * m_MagneticMoment.x * ( sint * v.y + cost * v.x ) / R2 );

		Bv = ( ( A * v ) / v.Length() - Bv ) / Bv.Length();

		return true;
	}
	return false;
}


// スカイマップの生成
void
PulseCalculator::CreateSkyMap( const Vector3Dd & pos, const Vector3Dd & Bv, std::vector<Vector2Dd>& vec_skymap )
{
	const double    Vc( sqrt( pos.x * pos.x + pos.y * pos.y ) );
	const double  Bphi( Bv.x * ( -pos.y ) + Bv.y * pos.x );
	const double Vpara( -Bphi + sqrt( Bphi * Bphi + 1 - Vc * Vc ) );

	Vector3Dd Vcorot( Vpara * Bv.x - pos.y, Vpara * Bv.y + pos.x, Vpara * Bv.z );
	Vcorot = Vcorot.Normalize();

	const double sign_y( ( 0 <= Vcorot.y ) ? 1.0 : -1.0 );
	const double phi( sign_y * acos( Vcorot.x / sqrt( Vcorot.x * Vcorot.x + Vcorot.y * Vcorot.y ) ) );
	double phase( -phi - pos.Dot( Vcorot ) );
	phase = ( phase < 0 ) ? 2.0 * PI + phase : phase;

	Vector2Dd  vec_temp( phase / RADIAN, acos( Vcorot.z ) / RADIAN );
	vec_skymap.emplace_back( vec_temp );
}


// パルスの生成
void
PulseCalculator::CreatePulse( const Vector3Dd & prev_Bv, const Vector3Dd & curr_Bv, const Vector2Dd& skymap, std::vector<Pulse>& vec_pulse )
{
	double photon_count( 0.0 );
	double length( prev_Bv.Length() * curr_Bv.Length() );
	if( 0 < length )
	{
		double radi_angle( acos( prev_Bv.Dot( curr_Bv ) / ( prev_Bv.Length() * curr_Bv.Length() ) ) );
		if( 0 < radi_angle )
		{
			double		curv_radius( STEP_LENGTH / radi_angle );
			double radi_solid_angle( PI * radi_angle * radi_angle );

			photon_count = 1.0 / ( DISTANCE_PULSAR_TO_EARTH * DISTANCE_PULSAR_TO_EARTH * radi_solid_angle ) * ( STEP_LENGTH / curv_radius );
		}
	}
	Pulse pulse;
	Vector2Dd vec_temp( skymap.x, photon_count );
	pulse.m_vecVertex2D.emplace_back( vec_temp );
	pulse.m_ViewingAngle = skymap.y;
	vec_pulse.emplace_back( pulse );
}


// 直交座標位置の取得
void
PulseCalculator::GetCartesianPosition( const Angle& angle, Vector3Dd& pos )const
{
	// 極座標系での位置取得
	Vector3Dd ppos;
	GetPolarPosition( angle, ppos );

	// 直交座標系での位置
	pos.x = ppos.z * sin( angle.inclination ) + ppos.x * cos( angle.inclination );
	pos.y = ppos.y;
	pos.z = ppos.z * cos( angle.inclination ) - ppos.x * sin( angle.inclination );
}


// 極座標系位置の取得
void
PulseCalculator::GetPolarPosition( const Angle& angle, Vector3Dd & pos ) const
{
	// 磁化軸を中心とした極座標系での位置
	pos.x = STAR_RADIUS * sin( angle.polar ) * cos( angle.azimth );
	pos.y = STAR_RADIUS * sin( angle.polar ) * sin( angle.azimth );
	pos.z = STAR_RADIUS * cos( angle.polar );
}
