#pragma once

#include "Material/Colors.h"
#include "Math/Vector3D.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

#include <cmath>


/** @brief カメラクラス */
class Camera
{
private:
	Colors*			m_pColors;			// 視界背景色
	Vector3Dd*		m_pEye;				// 視点位置
	Vector3Dd*		m_pCenter;			// 注視点位置
	Vector3Dd*		m_pUp;				// 視界の上方向位置
	double			m_Left;				// 視界の左側位置
	double			m_Right;			// 視界の右側位置
	double			m_Bottom;			// 視界の下側位置
	double			m_Top;				// 視界の上側位置
	double			m_Fov;				// 画角
	double			m_ZNear;			// 前方面位置
	double			m_ZFar;				// 後方面位置

	Matrix4x4*		m_pMatrix4x4;		// 4x4変換行列
	double*			m_pMultMatrix;		// 変換行列
	Quaterniond		m_Orientation;		// 姿勢
	Vector3Dd		m_Position;			// 位置


public:
	/** @brief コンストラクタ */
	Camera() : m_pColors(	 new Colors() )
	,			  m_pEye( new Vector3Dd() )
	,		   m_pCenter( new Vector3Dd() )
	,			   m_pUp( new Vector3Dd() )
	,			   m_Fov(			  0.0 )
	,			 m_ZNear(			  0.0 )
	,			  m_ZFar(			  0.0 )
	,		m_pMatrix4x4( new Matrix4x4() )
	{
		// 単位ベクトルで初期化
		m_pMultMatrix = new double[16];
		m_pMatrix4x4->GetIdentityMatrix( m_pMultMatrix );
	}


	/** @brief デストラクタ */
	~Camera()
	{
		delete	 m_pColors;
		delete	 m_pEye;
		delete	 m_pCenter;
		delete	 m_pUp;
		delete	 m_pMatrix4x4;
		delete[] m_pMultMatrix;
	}


public:
	/** @brief 視界範囲の設定 */
	void SetRange( double left, double right, double bottom, double top )
	{
		m_Left	 = left; 
		m_Right	 = right;
		m_Bottom = bottom;
		m_Top	 = top;
	}


	/** @brief 画角の設定
	 *
	 *  @attention		fov の単位は [degree]
	 */
	void SetFov( double fov ) { m_Fov = fov; }


	/** @brief 前方面位置の設定 */
	void SetZNear( double z_near ) { m_ZNear = z_near; }


	/**	@brief 後方面位置の設定 */
	void SetZFar( double z_far ) { m_ZFar = z_far; }


	/**
	 *	@brief 視点位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 */
	void SetEye( double x, double y, double z )
	{ 
		m_pEye->x = x;
		m_pEye->y = y;
		m_pEye->z = z;
	}


	/**
	 *	@brief 注視点位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 */
	void SetCenter( double x, double y, double z )
	{
		m_pCenter->x = x;
		m_pCenter->y = y;
		m_pCenter->z = z;
	}


	/**
	 *	@brief 視界の上方向の位置の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 */
	void SetUp( double x, double y, double z )
	{
		m_pUp->x = x;
		m_pUp->y = y;
		m_pUp->z = z;
	}


	/**
	 * @brief 視界背景色の設定
	 *
	 *  @param[in]			red	   R値		 （   0 ～ 255 ）
	 *  @param[in]			green  G値		 （   0 ～ 255 ）
	 *  @param[in]			blue   B値		 （   0 ～ 255 ）
	 *  @param[in]			alpha  アルファ値（ 0.0 ～ 1.0 ）
	 */
	void SetBackgroungColor( unsigned int red, unsigned int green, unsigned int blue, float alpha )
	{ 
		m_pColors->SetRGB( red, green, blue );
		m_pColors->alpha = alpha;
	}


	/**
	 *	@brief 平行移動の設定
	 *
	 *  @param[in]			x  要素 : x
	 *  @param[in]			y  要素 : y
	 *  @param[in]			z  要素 : z
	 */
	void SetTranslated( double x, double y, double z )
	{ 
		m_pMatrix4x4->MultTransMatrix( true, x, y, z, m_pMultMatrix );
	};


	/**
	 *	@brief 回転の設定
	 *
	 *  @param[in]			angle  回転角度 [degree]
	 *  @param[in]			x	   x軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			y	   y軸まわりの回転率（ 0.0 ～ 1.0 ）
	 *  @param[in]			z	   z軸まわりの回転率（ 0.0 ～ 1.0 ）
	 */
	void SetRotated( bool reset, double angle, double x, double y, double z )
	{ 
		if( reset ) m_pMatrix4x4->GetIdentityMatrix( m_pMultMatrix );
		m_pMatrix4x4->MultRotMatrix( true, angle, x, y, z, m_pMultMatrix );
	};


	/**
	 * @brief 回転の更新
	 *
	 *  @param[in]			dx  x方向の移動量
	 *  @param[in]			dy  y方向の移動量
	 */
	void UpdateRotated( double dx, double dy )
	{
		const double dw( std::sqrt( dx * dx + dy * dy ) );

		if( dw != 0.0 )
		{
			const double angle( dw * PI );						// dw * ( 2.0 * PI ) / 0.5
			const Quaterniond quat( dy * sin( angle ) / dw,		// 要素 : x
									 0.0,						// 要素 : y
									dx * sin( angle ) / dw,		// 要素 : z
									cos( angle ) );				// 要素 : z

			// 回転行列の取得
			m_pMatrix4x4->GetMatrix( true, quat.Cross( m_Orientation ), m_pMultMatrix );
		}
	}


	/** @brief 回転の終了 */
	void EndRotated(){ m_pMatrix4x4->GetQuaternion( m_Orientation ); }


public:
	/** @brief 視界の左側位置の取得 */
	double GetLeft() const { return m_Left; }


	/** @brief 視界の右側位置の取得 */
	double GetRight() const { return m_Right; }


	/** @brief 視界の下側位置の取得 */
	double GetBottom() const { return m_Bottom; }


	/** @brief 視界の上側位置の取得 */
	double GetTop() const { return m_Top; }


	/** @brief 画角の取得 [degree] */
	double GetFov() const { return m_Fov; }


	/** @brief 前方面距離の取得 */
	double GetZNear() const { return m_ZNear; }


	/** @brief 後方面距離の取得 */
	double GetZFar() const { return m_ZFar; }


	/** @brief 視点位置の取得 */
	Vector3Dd& GetEye() const { return *m_pEye; }


	/** brief 注視点位置の取得 */
	Vector3Dd& GetCenter() const { return *m_pCenter; }


	/** @brief 視界の上方向の位置の取得 */
	Vector3Dd& GetUp() const { return *m_pUp; }


	/** @brief 視界背景色の取得 */
	Colors& GetBackgroundColor() const { return *m_pColors; }

	/** @brief 変換行列の設定 */
	double* GetMatrix() const { return m_pMultMatrix; }
};
