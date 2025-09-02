#pragma once

#include "Math/Constants.h"

#include <math.h>


/** @brief 4 x 4 行列クラス */
class Matrix4x4
{
private:
	double m_11, m_12, m_13, m_14;					// 4 x 4 行列の 1 行目の 1 ～ 4 列目
	double m_21, m_22, m_23, m_24;					// 4 x 4 行列の 2 行目の 1 ～ 4 列目
	double m_31, m_32, m_33, m_34;					// 4 x 4 行列の 3 行目の 1 ～ 4 列目
	double m_41, m_42, m_43, m_44;					// 4 x 4 行列の 4 行目の 1 ～ 4 列目
	double m_quat_x, m_quat_y, m_quat_z, m_quat_w;	// クォータニオンの x, y, z, w 座標


public:
	/** @brief コンストラクタ */
	Matrix4x4() : m_11( 1.0 ), m_12( 0.0 ), m_13( 0.0 ), m_14( 0.0 )
	,			  m_21( 0.0 ), m_22( 1.0 ), m_23( 0.0 ), m_24( 0.0 )
	,			  m_31( 0.0 ), m_32( 0.0 ), m_33( 1.0 ), m_34( 0.0 )
	,			  m_41( 0.0 ), m_42( 0.0 ), m_43( 0.0 ), m_44( 1.0 )
	,			  m_quat_x( 0.0 ), m_quat_y( 0.0 ), m_quat_z( 0.0 ), m_quat_w( 1.0 )
	{
	}


	/** @brief デストラクタ */
	~Matrix4x4()
	{
	}


public:
	/**
	 *	@brief 単位行列の取得
	 *
	 *  @param[out]			matrix  変換行列
	 *
	 *  @attention			matrix は 16 要素
	 */
	void GetIdentityMatrix( double matrix[16] )const
	{
		matrix[ 0] = 1.0;
		matrix[ 1] = 0.0;
		matrix[ 2] = 0.0;
		matrix[ 3] = 0.0;

		matrix[ 4] = 0.0;
		matrix[ 5] = 1.0;
		matrix[ 6] = 0.0;
		matrix[ 7] = 0.0;

		matrix[ 8] = 0.0;
		matrix[ 9] = 0.0;
		matrix[10] = 1.0;
		matrix[11] = 0.0;

		matrix[12] = 0.0;
		matrix[13] = 0.0;
		matrix[14] = 0.0;
		matrix[15] = 1.0;
	}


	/**
	 *	@brief 平行移動行列の乗算
	 *
	 *  @param[in]			GL		OpenGLフラグ
	 *  @param[in]			x		要素 : x
	 *  @param[in]			y		要素 : y
	 *  @param[in]			z		要素 : z
	 *  @param[in/out]		matrix  変換行列（in : 乗算前 / out : 乗算後）
	 *
	 *  @attention			matrix は 16 要素
	 */
	void MultTransMatrix( bool GL, double x, double y, double z, double matrix[16] )
	{
		// 平行移動行列を設定
		m_11 = 1.0;
		m_12 = 0.0;
		m_13 = 0.0;
		m_14 = x;

		m_21 = 0.0;
		m_22 = 1.0;
		m_23 = 0.0;
		m_24 = y;

		m_31 = 0.0;
		m_32 = 0.0;
		m_33 = 1.0;
		m_34 = z;

		m_41 = 0.0;
		m_42 = 0.0;
		m_43 = 0.0;
		m_44 = 1.0;

		// 乗算
		MultMatrix( matrix );

		// 変換行列の取得
		if( GL )GetGLMatrix( matrix );
		else GetMatrix( matrix );
	}


	/**
	 *	@brief 回転行列の乗算
	 *
	 *  @param[in]			GL		OpenGLフラグ
	 *  @param[in]			angle	回転角度 [degree]
	 *  @param[in]			x		x 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			y		y 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			z		z 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in/out]		matrix  変換行列（in : 乗算前 / out : 乗算後）
	 *
	 *  @attention			matrix は 16 要素
	 */
	void MultRotMatrix( bool GL, double angle, double x, double y, double z, double matrix[16] )
	{
		// 回転ベクトルを変換行列に変換
		ConvertRotVectorToQuaternion( angle, x, y, z );
		ConvertQuaternionToRotMatrix();

		// 乗算
		MultMatrix( matrix );

		// 変換行列の取得
		if( GL )GetGLMatrix( matrix );
		else GetMatrix( matrix );
	}


	/**
	 *	@brief 変換行列の取得
	 *
	 *  @param[in]			GL		OpenGLの変換行列フラグ
	 *  @param[in]			quat	クォータニオン
	 *  @param[out]			matrix  変換行列
	 *
	 *  @attention			matrix は 16 要素
	 */
	void GetMatrix( bool GL, const Quaterniond& quat, double matrix[16] )
	{
		// クォータニオンを設定
		m_quat_x = quat.x;
		m_quat_y = quat.y;
		m_quat_z = quat.z;
		m_quat_w = quat.w;

		// クォータニオンを回転行列に変換
		ConvertQuaternionToRotMatrix();

		// 変換行列の取得
		if( GL )GetGLMatrix( matrix );
		else GetMatrix( matrix );
	}


	/**
	 *	@brief クォータニオンの取得
	 *
	 *  @param[in]			  quat  クォータニオン
	 */
	void GetQuaternion( Quaterniond& quat )const
	{
		quat.x = m_quat_x;
		quat.y = m_quat_y;
		quat.z = m_quat_z;
		quat.w = m_quat_w;
	}


private:
	/**
	 *	@brief 回転ベクトルをクォータニオンに変換
	 *
	 *  @param[in]			angle  回転角度 [degree]
	 *  @param[in]			x	   x 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			y	   y 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			z	   z 軸まわりの回転率（ 0.0 ～ 1.0 ）
	 */
	void ConvertRotVectorToQuaternion( double angle, double x, double y, double z )
	{
		double rad = ( angle * RADIAN ) * 0.5;

		m_quat_x = x * sin( rad );
		m_quat_y = y * sin( rad );
		m_quat_z = z * sin( rad );
		m_quat_w = cos( rad );
	}


	/** @brief クォータニオンを回転行列に変換 */
	void ConvertQuaternionToRotMatrix()
	{
		m_11 = 1.0 - 2.0 * ( ( m_quat_y * m_quat_y ) + ( m_quat_z * m_quat_z ) );
		m_12 = 2.0 * ( ( m_quat_x * m_quat_y ) - ( m_quat_z * m_quat_w ) );
		m_13 = 2.0 * ( ( m_quat_z * m_quat_x ) + ( m_quat_y * m_quat_w ) );
		m_14 = 0.0;

		m_21 = 2.0 * ( ( m_quat_x * m_quat_y ) + ( m_quat_z * m_quat_w ) );
		m_22 = 1.0 - 2.0 * ( ( m_quat_z * m_quat_z ) + ( m_quat_x * m_quat_x ) );
		m_23 = 2.0 * ( ( m_quat_y * m_quat_z ) - ( m_quat_x * m_quat_w ) );
		m_24 = 0.0;

		m_31 = 2.0 * ( ( m_quat_z * m_quat_x ) - ( m_quat_y * m_quat_w ) );
		m_32 = 2.0 * ( ( m_quat_y * m_quat_z ) + ( m_quat_x * m_quat_w ) );
		m_33 = 1.0 - 2.0 * ( ( m_quat_x * m_quat_x ) + ( m_quat_y * m_quat_y ) );
		m_34 = 0.0;

		m_41 = 0.0;
		m_42 = 0.0;
		m_43 = 0.0;
		m_44 = 1.0;
	}


	/**
	 *	@brief 行列の乗算
	 *
	 *  @param[in]			matrix  変換行列（in : 乗算前 / out : 乗算後）
	 *
	 *  @attention			matrix は 16 要素
	 */
	void MultMatrix( double matrix[16] )
	{
		// 乗算結果
		double result[4][4];

		// 乗数
		double multiplier[4][4] = { m_11, m_12, m_13, m_14,
									m_21, m_22, m_23, m_24,
									m_31, m_32, m_33, m_34,
									m_41, m_42, m_43, m_44 };

		// 被乗数
		double multiplicand[4][4] = { matrix[ 0], matrix[ 1],matrix[ 2], matrix[ 3],
				   					  matrix[ 4], matrix[ 5],matrix[ 6], matrix[ 7],
				   					  matrix[ 8], matrix[ 9],matrix[10], matrix[11],
				   					  matrix[12], matrix[13],matrix[14], matrix[15] };

		for( int i = 0; i < 4; i++ )
		{
			for( int j = 0; j < 4; j++ )
			{
				result[i][j] = 0.0;
				for( int k = 0; k < 4; k++ )
				{ 
					result[i][j] += ( multiplier[i][k] ) * ( multiplicand[k][j] );
				}
			}
		}

		m_11 = result[0][0];
		m_12 = result[0][1];
		m_13 = result[0][2];
		m_14 = result[0][3];

		m_21 = result[1][0];
		m_22 = result[1][1];
		m_23 = result[1][2];
		m_24= result[1][3];

		m_31 = result[2][0];
		m_32 = result[2][1];
		m_33 = result[2][2];
		m_34 = result[2][3];

		m_41 = result[3][0];
		m_42 = result[3][1];
		m_43 = result[3][2];
		m_44 = result[3][3];
	}


	/**
	 *	@brief 変換行列の取得
	 *
	 *  @param[out]			matrix  変換行列
	 *
	 *  @attention			matrix は 16 要素
	 */
	void GetMatrix( double matrix[16] )const
	{
		matrix[ 0] = m_11;
		matrix[ 1] = m_12;
		matrix[ 2] = m_13;
		matrix[ 3] = m_14;

		matrix[ 4] = m_21;
		matrix[ 5] = m_22;
		matrix[ 6] = m_23;
		matrix[ 7] = m_24;

		matrix[ 8] = m_31;
		matrix[ 9] = m_32;
		matrix[10] = m_33;
		matrix[11] = m_34;

		matrix[12] = m_41;
		matrix[13] = m_42;
		matrix[14] = m_43;
		matrix[15] = m_44;
	}


	/**
	 *	@brief OpenGL の変換行列の取得
	 *
	 *  @param[out]			matrix  変換行列
	 *
	 *  @attention			matrix は 16 要素
	 */
	void GetGLMatrix( double matrix[16] )const
	{
		// OpenGL 用に変換（列優先データ）
		matrix[ 0] = m_11;
		matrix[ 4] = m_12;
		matrix[ 8] = m_13;
		matrix[12] = m_14;	// x

		matrix[ 1] = m_21;
		matrix[ 5] = m_22;
		matrix[ 9] = m_23;
		matrix[13] = m_24;	// y

		matrix[ 2] = m_31;
		matrix[ 6] = m_32;
		matrix[10] = m_33;
		matrix[14] = m_34;	// z

		matrix[ 3] = m_41;
		matrix[ 7] = m_42;
		matrix[11] = m_43;
		matrix[15] = m_44;
	}
};