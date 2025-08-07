#pragma once

#include "Rendering/Renderer.h"

#include <freeglut.h>
#include <math.h>


#pragma region コンストラクタ、デストラクタ
// コンストラクタ
Renderer::Renderer() : m_dicGLWindowHandle( gcnew Dictionary<PictureBox^,Handle^>() )
,								    m_hWnd(									nullptr )
,								     m_hDC(									nullptr )
,								   m_hGLRC(									nullptr )
{
	// glut の初期化
	int argcp( 0 );
	glutInit( &argcp, nullptr );
}


// デストラクタ
Renderer::~Renderer()
{
	if( m_hWnd && m_hDC )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_hGLRC );

		ReleaseDC( m_hWnd, m_hDC );

		m_hWnd	= nullptr;
		m_hDC	= nullptr;
		m_hGLRC = nullptr;
	}
}
#pragma endregion


#pragma region 設定
// 描画画面の設定
bool
Renderer::SetupGLWindow( PictureBox^ pct )
{
	bool success = false;

	// ウィンドウハンドルの取得
	if( m_hWnd = (HWND)( pct->Handle ).ToPointer() ) success = true;
	// デバイスコンテキストハンドルの取得
	if( success )
	{
		if( !( m_hDC = GetDC( m_hWnd ) ) ) success = false;
	}
	// ピクセルフォーマットの設定
	if( success )
	{
		const int is_setup = SetupPixelFormat( m_hDC );
		if( is_setup == 0 ) success = false;
	}
	// レンダリングコンテキストの取得
	if( success )
	{
		if( !( m_hGLRC = wglCreateContext( m_hDC ) ) ) success = false;
	}

	// 管理情報の更新
	m_dicGLWindowHandle->Add( pct, gcnew Handle( m_hWnd, m_hDC, m_hGLRC ) );

	return success;
}


// ピクセルフォーマットの設定
int
Renderer::SetupPixelFormat( HDC hdc )
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),	// PIXELFORMATDESCRIPTOR のサイズ
		1,									// 必ず１
		PFD_DRAW_TO_WINDOW |				// 使用目的：ウィンドウに対する描画
		PFD_SUPPORT_OPENGL |				// 使用目的：OpenGL サポート
		PFD_DOUBLEBUFFER,					// 使用目的：ダブルバッファリングで描画高速化
		PFD_TYPE_RGBA,						// RGB の使用方法
		32,									// 使用する色のビッド数（8bit：白黒、24bit：RGB、32bit：RGBA）
		0, 0, 0, 0, 0, 0,					// color bits ignored 
		0,									// no alpha buffer 
		0,									// shift bit ignored 
		0,									// no accumulation buffer 
		0, 0, 0, 0,							// accum bits ignored 
		32,									// 32-bit z-buffer
		0,									// no stencil buffer 
		0,									// no auxiliary buffer 
		PFD_MAIN_PLANE,						// main layer 
		0,									// reserved 
		0, 0, 0								// layer masks ignored 
	};

	// 使用可能なピクセルフォーマットの番号を取得する
	const int pixelFormat( ChoosePixelFormat( hdc, &pfd ) );
	if( pixelFormat == 0 ) return 0;

	// 設定したピクセルフォーマットをデバイスコンテキストが使用する
	// ピクセルフォーマットとして設定する
	if( SetPixelFormat( hdc, pixelFormat, &pfd ) == FALSE ) return 0;

	return 1;
}


// 描画画面のハンドル設定
void
Renderer::SetGLWindowHandle( Handle^ handle )
{
	m_hWnd	= handle->GetHWND();
	m_hDC	= handle->GetHDC();
	m_hGLRC = handle->GetHGLRC();
}


//---------------------------------------------------------------------


// ステートの設定
void
Renderer::SetupGLStates( const Camera& camera )
{
	// 背景色
	Colors color( camera.GetBackgroundColor() );
	glClearColor( color.red, color.green, color.blue, color.alpha);

	// カラーバッファ( GL_COLOR_BUFFER_BIT )
	// デプスバッファ( GL_DEPTH_BUFFER_BIT )の消去
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// デプスバッファを 1.0f で初期化
	glClearDepth( 1.0f );

	// カラーデプスバッファマスクを有効( GL_TRUE )に設定
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	// ポリゴンオフセット( GL_POLYGON_OFFSET_FILL )を無効にする
	glDisable( GL_POLYGON_OFFSET_FILL );

	// 隠面消去( GL_DEPTH_TEST )を有効にする
	//glEnable( GL_DEPTH_TEST );
	glDisable( GL_DEPTH_TEST );

	// デプスバッファの評価方法を
	// 近いものほど上に表示する( GL_LEQUAL )ものとする
	glDepthFunc( GL_LEQUAL );

	// 陰影処理の方法を
	// ポリゴンの陰影が滑らかに表現される( スムースシェーディング )描画方法とする
	glShadeModel( GL_SMOOTH );

	// 混合処理( GL_BLEND )を有効にする
	glEnable( GL_BLEND );

	// 透過( GL_ALPHA_TEST )を有効にする
	glEnable( GL_ALPHA_TEST );

	// 加算 + アルファ
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		// アルファブレンド
	//glBlendFunc( GL_ONE, GL_ONE );							// 加算
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );						// スクリーン
}


// ビューポートの設定
void
Renderer::SetupGLViewport( int width, int height, const Camera& camera )
{
	// ビューポートの左下隅の座標を( 0, 0 )に
	// 幅と高さを( width, height )に設定	
	glViewport( 0, 0, width, height );


	////////////////////////////////////////////
	//
	// 視体積の定義
	//
	// マトリックスモードを投影変換( GL_PROJECTION )に設定
	glMatrixMode( GL_PROJECTION );

	// 変換行列の初期化

	glLoadIdentity();

	// 平行投影の設定
	glOrtho( camera.GetLeft(),
			 camera.GetRight(),
			 camera.GetBottom(),
			 camera.GetTop(),
			 camera.GetZNear(),
			 camera.GetZFar() );
	// 透視投影の設定
	//const double aspect = (double)width / (double)height;
	//gluPerspective( camera.GetFov() , aspect, camera.GetZNear(), camera.GetZFar() );
	//
	//
	//
	////////////////////////////////////////////


	////////////////////////////////////////////
	//
	// 視点の定義
	//
	// マトリックスモードをモデルビュー( GL_MODELVIEW )に設定
	glMatrixMode( GL_MODELVIEW );

	// 変換行列の初期化
	glLoadIdentity();

	// カメラの設定
	const Vector3Dd	   eye( camera.GetEye() );
	const Vector3Dd center( camera.GetCenter() );
	const Vector3Dd	    up( camera.GetUp() );
	gluLookAt(	  eye.x,	eye.y,	  eye.z,	// 視点の位置
			   center.x, center.y, center.z,	// 注視点の位置を原点( 0, 0, 0 )設定
				   up.x,	 up.y,	   up.z );	// 視界の上方向を y 軸方向に設定

	glMultMatrixd( camera.GetMatrix() );
	//
	//
	//
	////////////////////////////////////////////
}


// ライトの設定
void
Renderer::SetupGLLighting( const Light& light )
{
	// ライティング処理( GL_LIGHTING )を有効にする
	glEnable( GL_LIGHTING );

	// 0 番目の点光源( GL_LIGHT0 )を点灯する
	glEnable( GL_LIGHT0 );

	// 光源の位置を設定
	const float* position( light.GetPosition().GetPtr() );
	glLightfv( GL_LIGHT0, GL_POSITION, position );

	// 拡散光の設定
	const float* diffuse( light.GetDiffuse().GetPtr() );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );

	// 環境光の設定
	const float* ambient( light.GetAmbient().GetPtr() );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );

	// 鏡面光の設定
	const float* specular( light.GetSpecular().GetPtr() );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
}
#pragma endregion


#pragma region 描画処理
// 描画開始
bool
Renderer::BeginRender( PictureBox^ pct )
{
	bool success = false;

	if( m_dicGLWindowHandle->ContainsKey( pct ) )
	{
		// 描画画面のハンドル設定
		SetGLWindowHandle( m_dicGLWindowHandle[pct] );

		if( m_hWnd && m_hGLRC )
		{
			wglMakeCurrent( m_hDC, m_hGLRC );
			success = true;
		}
	}

	return success;
}


// 描画終了
bool
Renderer::EndRender()
{
	bool success = false;

	if( m_hDC )
	{
		SwapBuffers( m_hDC );
		success = true;
	}

	return success;
}


// 図形の描画
void
Renderer::RenderShape( const std::vector<Shape*>& vec_p_shape )
{
	for each ( const Shape* p_shape in vec_p_shape )
	{
		glPushMatrix();

		// 描画
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, p_shape->GetMainColor().GetPtr() );
		switch( p_shape->GetType() )
		{
			case Shape::Type::SPHERE:
				{
					Sphere* sphere = (Sphere*)p_shape;
					glutSolidSphere( sphere->GetRadius(), sphere->GetSlices(), sphere->GetStacks() );
				}
				break;
			case Shape::Type::CYLINDER:
				{
					Cylinder* cylinder = (Cylinder*)p_shape;
					glMultMatrixd( cylinder->GetMatrix() );
					glutSolidCylinder( cylinder->GetRadius(), cylinder->GetHeight(), cylinder->GetSlices(), cylinder->GetStacks() );
				}
				break;
			case Shape::Type::ARROW:
				{
					// シャフト
					RenderSingleLine( *p_shape );

					// アローヘッド
					Arrow* arrow = (Arrow*)p_shape;
					std::vector<Vector3Dd> vec_pos( arrow->GetHeadPosition() );

					for( unsigned int i = 0; i < vec_pos.size(); i++ )
					{
						Vector3Dd pos( vec_pos[i] );
						glTranslated( pos.x, pos.y, pos.z );
						glutSolidCone( arrow->GetHeadRadius(), arrow->GetHeadHeight(), arrow->GetHeadSlices(), arrow->GetHeadStacks() );
					}
				}
				break;
			case Shape::Type::VERTEX:
				{
					using count_t = Vertex::CountType;
					Vertex* p_vertex = (Vertex*)p_shape;
					count_t count_type = p_vertex->GetCountType();

					switch( count_type )
					{
						case count_t::SINGLE  :RenderSingleLine( *p_shape ); break;
						case count_t::MULTIPLE:RenderMultipleLine( *p_shape ); break;
					}
				}
				break;
		}
		glPopMatrix();
	}
}


// 複数線分の描画
void
Renderer::RenderMultipleLine( const Shape& shape )
{
	using vector_t = Vertex::VectorType;
	using line_t   = Shape::LineType;

	Vertex*   p_vertex( (Vertex*)&shape );
	vector_t  vec_type( p_vertex->GetVectorType() );
	line_t   line_type( p_vertex->GetLineType() );

	glLineWidth( p_vertex->GetWidth() );
	glPointSize( p_vertex->GetWidth() );

	switch( vec_type )
	{
		case vector_t::XY:
		case vector_t::YZ:
		case vector_t::XZ:
			{
				std::vector<std::vector<Vector2Dd>> vec_vec_vertex2d( p_vertex->GetMultipleLine2D() );
				size_t vertex_size( vec_vec_vertex2d.size() );

				double single_value( p_vertex->GetThridDimensionSingleValue() );
				std::vector<double> multiple_value( p_vertex->GetThridDimensionMultipleValue() );
				bool is_multiple( multiple_value.size() == vertex_size );

				size_t max_index( vertex_size - 1 );
				for( size_t i = 0; i < vertex_size; i++ )
				{
					double dimension( ( is_multiple ) ? multiple_value[max_index - i] : single_value );
					std::vector<Vector2Dd> vec_line( vec_vec_vertex2d[max_index - i] );

					switch( vec_type )
					{
						case vector_t::XY:
						case vector_t::YZ:
							{
								glBegin( GL_LINES );
								{
									LineTypeGLBegin( line_type );
									for( unsigned int j = 0; j < vec_line.size(); j++ )
									{
										if( ( 1 < j ) && ( 2 <= std::abs( vec_line[j].x - vec_line[j - 1].x ) ) && ( line_type == line_t::SOLID ) )
										{
											glEnd();
											glBegin( GL_LINES );
											LineTypeGLBegin( line_type );
										}
										if( vec_type == vector_t::XY )glVertex3d( vec_line[j].x, vec_line[j].y, dimension );
										else if( vec_type == vector_t::YZ )glVertex3d( dimension, vec_line[j].x, vec_line[j].y );
									}
								}
								glEnd();
							}
							break;
						case vector_t::XZ:
							{
								glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, p_vertex->GetSubColor().GetPtr() );
								glBegin( GL_TRIANGLE_STRIP );
								{
									for( size_t j = 0; j < vec_line.size(); j++ )
									{
										if( j == 0 ) glVertex3d( 0.0, dimension, 0.0 );
										glVertex3d( vec_line[    j].x, vec_line[j].y + dimension, 0.0 );
										glVertex3d( vec_line[j + 1].x,				   dimension, 0.0 );
									}
								}
								glEnd();

								if( i >= 90 ) continue;

								glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, p_vertex->GetMainColor().GetPtr() );
								glBegin( GL_LINES );
								{
									for( size_t j = 0; j < vec_line.size(); j++ )
									{
										LineTypeGLBegin( line_type );
										glVertex3d( vec_line[j].x, vec_line[j].y + dimension, 0.0 );
									}
								}
								glEnd();

								//glBegin( GL_LINES );
								//{
								//	LineTypeGLBegin( line_type );
								//	unsigned int max_index( vec_line.size() - 1 );
								//	for( unsigned int j = 0; j < max_index + 1; j++ )
								//	{
								//		glVertex3d( vec_line[max_index -j].x, vec_line[max_index-j].y + dimension, 0.0 );
								//	}
								//}
								//glEnd();
							}
							break;
					}
				}
			}
			break;
		case vector_t::XYZ:
			{
				std::vector<std::vector<Vector3Dd>> vec_vec_vertex3d( p_vertex->GetMultipleLine3D() );
				for( unsigned int i = 0; i < vec_vec_vertex3d.size(); i++ )
				{
					std::vector<Vector3Dd> vec_line( vec_vec_vertex3d[i] );

					glBegin( GL_LINES );
					{
						LineTypeGLBegin( line_type );
						for( unsigned int j = 0; j < vec_line.size(); j++ )
							glVertex3d( vec_line[j].x, vec_line[j].y, vec_line[j].z );
					}
					glEnd();
				}
			}
			break;
	}
}


// 単一線分の描画
void
Renderer::RenderSingleLine( const Shape & shape )
{
	using shape_t = Shape::Type;
	using  line_t = Shape::LineType;

	shape_t shape_type( shape.GetType() );
	line_t line_type;

	std::vector<Vector2Dd> vec_line2d;
	std::vector<Vector3Dd> vec_line3d;
	float width( 0.0 );

	switch( shape_type )
	{
		case shape_t::VERTEX:
			{
				using vector_t = Vertex::VectorType;

				Vertex* p_vertex( (Vertex*)&shape );
				line_type  = p_vertex->GetLineType();
				width      = p_vertex->GetWidth();

				vector_t vec_type = p_vertex->GetVectorType();
				switch( vec_type )
				{
					//case vector_t::VEC_2D:vec_line2d = p_vertex->GetSingleLine2D(); break;
					case vector_t::XYZ:vec_line3d = p_vertex->GetSingleLine3D(); break;
				}
			}
			break;
		case shape_t::ARROW:
			{
				Arrow* p_arrow( (Arrow*)&shape );
				line_type  = p_arrow->GetShaftType();
				width	   = p_arrow->GetShaftWidth();
				vec_line3d = p_arrow->GetShaftLine();
			}
			break;
	}

	glLineWidth( width );
	glPointSize( width );

	glBegin( GL_LINES );
	{
		LineTypeGLBegin( line_type );
		for( unsigned int i = 0; i < vec_line3d.size(); i++ )
			glVertex3d( vec_line3d[i].x, vec_line3d[i].y, vec_line3d[i].z );
	}
	glEnd();
}


// 線種別 glBegin
void
Renderer::LineTypeGLBegin( const Shape::LineType type )
{
	using  line_t = Shape::LineType;

	switch( type )
	{
		case line_t::SOLID:
			{
				glBegin( GL_LINE_STRIP );
			}
			break;
		case line_t::DOTS:
			{
				glBegin( GL_LINE_STIPPLE );
				glLineStipple( 1, 0xF0F0 );
			}
			break;
		case line_t::POINT:
			{
				glBegin( GL_POINTS );
			}
			break;
	}
}
#pragma endregion