#pragma once

#include "MainWindow.h"

#include <freeglut.h>

// DEBUG
//#include <fstream>
//std::ofstream ofs( "C:/Users/kana/Desktop/test/test.txt" );


namespace PulseProfileSimulator
{
	using mouse_btn_t  = System::Windows::Forms::MouseButtons;
	using scene_t	   = Scene::Type;
	using shape_t      = Shape::Type;
	using line_t	   = Shape::LineType;
	using line_count_t = Vertex::CountType;
	using line_vec_t   = Vertex::VectorType;
	using font_t	   = System::Drawing::Font;


#pragma region 初期化 / 終了処理
	// コンストラクタ
	MainWindow::MainWindow( void ) : m_pRenderer(		gcnew Renderer() )
	,								   m_pPulsar(			new Pulsar() )
	,						  m_pPulseCalculator(  new PulseCalculator() )
	,						  m_pCriticalSection( new CRITICAL_SECTION() )
	,							  m_pPulsarModel(			 new Scene() )
	,					   m_pPolarCapNorthBegin(		     new Scene() )
	,					     m_pPolarCapNorthEnd(		     new Scene() )
	,					   m_pPolarCapSouthBegin(		     new Scene() )
	,						 m_pPolarCapSouthEnd(		     new Scene() )
	,								   m_pSkyMap(			 new Scene() )
	,							 m_pPulseProfile(			 new Scene() )
	,									m_IsDrag(				   false )
	{
		InitializeComponent();

		// クリティカルセクションの初期化
		::InitializeCriticalSection( m_pCriticalSection );

		// UI の初期設定
		SetupUI();

		// レンダリング画面の構築
		m_pRenderingWindow = gcnew array<PictureBox^>( (int)scene_t::COUNT );

		// 初期設定
		SetupPulsar();

		// シーン生成
		CreateScene();
	}


	// デストラクタ
	MainWindow::~MainWindow()
	{
		// レンダリングの停止
		m_pRenderingTimer->Stop();

		// リソースの開放
		if ( components ) delete components;

		// リソースの開放
		delete m_pRenderer;
		delete m_pPulsar;
		delete m_pPulseCalculator;
		delete m_pPulsarModel;
		delete m_pPolarCapNorthBegin;
		delete m_pPolarCapNorthEnd;
		delete m_pPolarCapSouthBegin;
		delete m_pPolarCapSouthEnd;
		delete m_pSkyMap;
		delete m_pPulseProfile;

		// クリティカルセクションの破棄
		::DeleteCriticalSection( m_pCriticalSection );
		delete m_pCriticalSection;
	}
#pragma endregion


#pragma region UI 設定
	// UI の初期設定
	void
	MainWindow::SetupUI()
	{
		array<PictureBox^>^ arr_p_pct = { m_pPulseScaleHGridPictureBox,
										  m_pPulseScaleHNumberPictureBox,
										  m_pSkyMapScaleHGridPictureBox,
										  m_pSkyMapScaleHNumberPictureBox,
										  m_pSkyMapScaleVGridPictureBox,
										  m_pSkyMapScaleVNumberPictureBox };
		Bitmap^ p_bmp;
		Graphics^ p_graphics;

		for each ( PictureBox^ pct in arr_p_pct )
		{
			p_bmp = gcnew Bitmap( pct->Width, pct->Height );
			p_graphics = Graphics::FromImage( p_bmp );
			
			// 目盛り線
			if( ( pct == m_pPulseScaleHGridPictureBox  ) ||
				( pct == m_pSkyMapScaleHGridPictureBox ) ||
				( pct == m_pSkyMapScaleVGridPictureBox ) )
			{
				int interval( 0 );
				int count( 0 );
				Alignment alignment;

				if( pct == m_pPulseScaleHGridPictureBox )
				{
					interval = 15;
					count = 36;
					alignment = Alignment::BOTTOM;
				}
				else if( pct == m_pSkyMapScaleHGridPictureBox )
				{
					interval = 20;
					count = 36;
					alignment = Alignment::BOTTOM;
				}
				else if( pct == m_pSkyMapScaleVGridPictureBox )
				{
					interval = 20;
					count = 18;
					alignment = Alignment::LEFT;
				}
				DrawScaleGrid( alignment, interval, count, Pens::White, p_graphics );
			}
			// 目盛り数値
			else if( ( pct == m_pPulseScaleHNumberPictureBox  ) ||
					 ( pct == m_pSkyMapScaleHNumberPictureBox ) ||
					 ( pct == m_pSkyMapScaleVNumberPictureBox ) )
			{
				array<ScaleNumber^>^ arr_p_number;

				if( pct == m_pPulseScaleHNumberPictureBox )
				{
					arr_p_number = gcnew array<ScaleNumber^>{ gcnew ScaleNumber(   "0",   9, 4 ),
															  gcnew ScaleNumber( "180", 276, 4 ),
															  gcnew ScaleNumber( "360", 547, 4 )};
				}
				else if( pct == m_pSkyMapScaleHNumberPictureBox )
				{
					arr_p_number = gcnew array<ScaleNumber^>{ gcnew ScaleNumber(   "0",  30, 4 ),
															  gcnew ScaleNumber( "180", 386, 4 ),
															  gcnew ScaleNumber( "360", 747, 4 )};
				}
				else if( pct == m_pSkyMapScaleVNumberPictureBox )
				{
					arr_p_number = gcnew array<ScaleNumber^>{ gcnew ScaleNumber( "180", 0,  10 ),
															  gcnew ScaleNumber(  "90", 0, 190 )};
				}
				DrawScaleNumber( arr_p_number, gcnew font_t( FONT_MEIRYO_UI, 10 ), Brushes::White, p_graphics );
			}
			pct->Image = p_bmp;
		}

		font_t^ p_font = m_pSkyMapScaleHTitleLabel->Font;
		Brush^ p_brush = Brushes::White;
		Pen^ p_pen = Pens::White;

		// グラフ数値タイトル（縦書き：viewing angle）
		p_bmp = gcnew Bitmap( m_pSkyMapScaleVTitlePictureBox->Width, m_pSkyMapScaleVTitlePictureBox->Height );
		p_graphics = Graphics::FromImage( p_bmp );
		p_graphics->DrawString( "viewing angle", p_font, p_brush, 0, 136, gcnew StringFormat( StringFormatFlags::DirectionVertical ) );
		m_pSkyMapScaleVTitlePictureBox->Image = p_bmp;

		// 方位
		p_bmp = gcnew Bitmap( m_pPolarCapAzimuthPictureBox->Width, m_pPolarCapAzimuthPictureBox->Height );
		p_graphics = Graphics::FromImage( p_bmp );
		p_graphics->DrawString( "N", p_font, p_brush, 20,  0 );
		p_graphics->DrawString( "S", p_font, p_brush, 20, 45 );
		p_graphics->DrawLine( p_pen, 15, 18, 39, 42 );
		p_graphics->DrawLine( p_pen, 39, 18, 15, 42 );
		m_pPolarCapAzimuthPictureBox->Image = p_bmp;		
	}


	// グラフの目盛り線描画
	void
	MainWindow::DrawScaleGrid( Alignment alignment, int interval, int count, Pen^ p_pen, Graphics^& p_graphics )
	{		
		for( int i = 0; i <= count; i++ )
		{
			const int pos( ( i == 0  ) ? 0 : ( interval * i + 1 ) );
			const int length( ( i % ( count / 2 ) == 0 ) ? 8 : 4 );

			     if( alignment == Alignment::BOTTOM ) p_graphics->DrawLine( p_pen, pos,   0, pos        , length );
			else if( alignment == Alignment::LEFT   ) p_graphics->DrawLine( p_pen, 10 , pos, 10 - length, pos    );
		}
	}


	// グラフの目盛り数値描画
	void
	MainWindow::DrawScaleNumber( array<ScaleNumber^>^ arr_p_number, System::Drawing::Font^ p_font, Brush^ p_brush, Graphics^& p_graphics )
	{
		for each ( ScaleNumber^ p_number in arr_p_number )
			p_graphics->DrawString( p_number->number, p_font, p_brush, p_number->x, p_number->y );
	}
#pragma endregion


#pragma region 描画画面マウス操作
	// シーン種類の取得
	void
	MainWindow::GetSceneType( const PictureBox ^ p_window, Scene::Type& type )
	{
		type = ( p_window == m_pPulsarModelPictureBox		 ) ? scene_t::PULSAR_MODEL			:
			   ( p_window == m_pPolarCapNorthBeginPictureBox ) ? scene_t::POLAR_CAP_NORTH_BEGIN :
			   ( p_window == m_pPolarCapNorthEndPictureBox   ) ? scene_t::POLAR_CAP_NORTH_END   :
			   ( p_window == m_pPolarCapSouthBeginPictureBox ) ? scene_t::POLAR_CAP_SOUTH_BEGIN	:
			   ( p_window == m_pPolarCapSouthEndPictureBox   ) ? scene_t::POLAR_CAP_SOUTH_END	:
			   ( p_window == m_pSkyMapPictureBox			 ) ? scene_t::SKY_MAP				:
			   ( p_window == m_pPulseProfilePictureBox		 ) ? scene_t::PULSE_PROFILE			:
																 scene_t::NONE					;
	}


	// 描画画面のマウス押下イベント
	System::Void
	MainWindow::RenderingWindow_MouseDown( System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
	{
		PictureBox^ window( (PictureBox^)sender );
		window->Focus();

		// シーン種類の取得、確認
		scene_t scene( scene_t::NONE );
		GetSceneType( window, scene );

		// シーン別処理
		switch( scene )
		{
			case( scene_t::NONE	   ):return; break;
			case( scene_t::SKY_MAP ):
				{
					// マウスカーソルを視線方向線にスナップする
					if( !m_IsDrag && m_pSkyMapPictureBox->Cursor == Cursors::HSplit )
					{
						Point ScreenPoint = m_pSkyMapPictureBox->PointToScreen( Point( e->X, m_pViewingAngleLine->Location.Y ) );
						SetCursorPos( ScreenPoint.X, ScreenPoint.Y );

						// 視線方向線の移動開始
						m_pViewingAngleLine_MouseDown( m_pSkyMapPictureBox, e );
					}
				}
				break;
			case( scene_t::PULSAR_MODEL ):
				{
					// マウス位置の設定
					if( e->Button == mouse_btn_t::Left )
						m_pPulsarModel->SetMouseDownPosition( e->X, e->Y );
				}
				break;

		}
	}


	// 描画画面のマウス移動イベント
	System::Void
	MainWindow::RenderingWindow_MouseMove( System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
	{
		PictureBox^ window( (PictureBox^)sender );

		// シーン種類の取得、確認
		scene_t scene( scene_t::NONE );
		GetSceneType( window, scene );

		// シーン別処理
		switch( scene )
		{
			case( scene_t::NONE	   ):return; break;
			case( scene_t::SKY_MAP ):
				{
					// 視線方向線の移動更新
					if( e->Button == mouse_btn_t::Left )
					{
						if( m_IsDrag )
							m_pViewingAngleLine_MouseMove( nullptr, e );
					}
					// カーソル変更
					else
					{
						bool can_snap( ( m_pViewingAngleLine->Location.Y - 15 <= e->Y ) &&
									   ( m_pViewingAngleLine->Location.Y + 15 >= e->Y ) );

						m_pSkyMapPictureBox->Cursor = ( can_snap ) ? Cursors::HSplit : Cursors::Default;
					}
				}
				break;
			case( scene_t::PULSAR_MODEL ):
				{
					// マウス位置の設定
					if( e->Button == mouse_btn_t::Left )
						m_pPulsarModel->SetMouseMovePosition( e->X, e->Y );
				}
				break;
		}
	}


	// 描画画面のマウス離上イベント
	System::Void
	MainWindow::RenderingWindow_MouseUp( System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
	{
		PictureBox^ window( (PictureBox^)sender );

		// シーン種類の取得、確認
		scene_t scene( scene_t::NONE );
		GetSceneType( window, scene );

		// シーン別処理
		switch( scene )
		{
			case( scene_t::NONE	   ):return; break;
			case( scene_t::SKY_MAP ):
				{
					// 視線方向線の移動終了
					if( m_IsDrag )
						m_pViewingAngleLine_MouseUp( nullptr, e );
				}
				break;
			case( scene_t::PULSAR_MODEL ):
				{
					// マウス位置の設定
					if( e->Button == mouse_btn_t::Left )
						m_pPulsarModel->SetMouseUpPosition( e->X, e->Y );
				}
				break;
		}
	}


	// スカイマップ視線方向線のマウス押下イベント
	System::Void
	MainWindow::m_pViewingAngleLine_MouseDown( System::Object ^ , System::Windows::Forms::MouseEventArgs ^ e )
	{
		if( m_IsDrag ) return;

		// フラグ更新
		m_IsDrag = true;

		// マウス押下位置の取得
		m_MouseDownPoint = e->Location;

		// GUI 変更
		m_pViewingAngleLine->BackColor = Color::DarkMagenta;
	}


	// スカイマップ視線方向線のマウス移動イベント
	System::Void
	MainWindow::m_pViewingAngleLine_MouseMove( System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
	{
		if( !m_IsDrag ) return;

		// 視線方向線の移動位置取得
		int y( ( dynamic_cast<Panel^>( sender ) ) ? ( m_pViewingAngleLine->Location.Y + e->Y - m_MouseDownPoint.Y ) : e->Y );
		if( y < 0 ) y = 0;

		// 視線方向線の移動、描画更新
		m_pViewingAngleLine->Location = System::Drawing::Point( 0, y );
		UpdateRender( m_pSkyMapPictureBox );
	}


	// スカイマップ視線方向線のマウス離上イベント
	System::Void
	MainWindow::m_pViewingAngleLine_MouseUp( System::Object ^ , System::Windows::Forms::MouseEventArgs ^ e )
	{
		if( !m_IsDrag ) return;

		// フラグ更新
		m_IsDrag = false;

		// GUI 変更
		m_pViewingAngleLine->BackColor = Color::Magenta;
	}
#pragma endregion


#pragma region 描画更新
	// シーンの取得
	Scene*
	MainWindow::GetScene( PictureBox^ p_window )
	{
		return( ( p_window->Name == "m_pPulsarModelPictureBox"		  ) ? m_pPulsarModel        :
				( p_window->Name == "m_pPolarCapNorthBeginPictureBox" ) ? m_pPolarCapNorthBegin : 
				( p_window->Name == "m_pPolarCapNorthEndPictureBox"   ) ? m_pPolarCapNorthEnd   : 
				( p_window->Name == "m_pPolarCapSouthBeginPictureBox" ) ? m_pPolarCapSouthBegin :
				( p_window->Name == "m_pPolarCapSouthEndPictureBox"   ) ? m_pPolarCapSouthEnd   :
				( p_window->Name == "m_pSkyMapPictureBox"			  ) ? m_pSkyMap	            : 
				( p_window->Name == "m_pPulseProfilePictureBox"		  ) ? m_pPulseProfile       : nullptr );
	}


	// レンダリング更新
	void
	MainWindow::UpdateRender( PictureBox^ p_window )
	{
		if( !p_window ) return;

		m_pRenderer->BeginRender( p_window );
		{
			// シーン取得
			Scene* p_scene( GetScene( p_window ) );

			// レンダリング
			if( p_scene )
			{
				EnterCriticalSection( m_pCriticalSection );
				{
					m_pRenderer->SetupGLStates( p_scene->GetCamera() );
					m_pRenderer->SetupGLViewport( p_window->Width, p_window->Height, p_scene->GetCamera() );
					m_pRenderer->SetupGLLighting( p_scene->GetLight() );		
					m_pRenderer->RenderShape( p_scene->GetElement() );
				}
				LeaveCriticalSection( m_pCriticalSection );

			}
		}
		m_pRenderer->EndRender();
	}


	// 画面サイズの設定
	void 
	MainWindow::SetWindowSize( PictureBox^ p_window )
	{
		// シーン取得
		Scene* p_scene( GetScene( p_window ) );

		if( p_scene )
			p_scene->SetWindowSize( p_window->Width, p_window->Height ); 
	}


	// メイン画面の表示イベント
	System::Void
	MainWindow::MainWindow_Shown( System::Object^ , System::EventArgs^  e )
	{
		// レンダリング画面の設定
		for( int i = 0; i < (int)scene_t::COUNT; i++ )
		{
			PictureBox^ window;
			switch( i )
			{
				case (int)scene_t::PULSAR_MODEL         : window = m_pPulsarModelPictureBox       ; break;
				case (int)scene_t::POLAR_CAP_NORTH_BEGIN: window = m_pPolarCapNorthBeginPictureBox; break;
				case (int)scene_t::POLAR_CAP_NORTH_END  : window = m_pPolarCapNorthEndPictureBox  ; break;
				case (int)scene_t::POLAR_CAP_SOUTH_BEGIN: window = m_pPolarCapSouthBeginPictureBox; break;
				case (int)scene_t::POLAR_CAP_SOUTH_END  : window = m_pPolarCapSouthEndPictureBox  ; break;
				case (int)scene_t::SKY_MAP		        : window = m_pSkyMapPictureBox	          ; break;
				case (int)scene_t::PULSE_PROFILE        : window = m_pPulseProfilePictureBox      ; break;
			}

			// 描画準備
			SetWindowSize( window );
			m_pRenderingWindow[i] = window;
			m_pRenderer->SetupGLWindow( window );
		}
	}


	// メイン画面のリサイズイベント
	System::Void
	MainWindow::MainWindow_Resize( System::Object^  sender, System::EventArgs^  e )
	{
		for each ( PictureBox^ window in m_pRenderingWindow )
			UpdateRender( window );
	}


	// タイマーの打刻イベント
	System::Void
	MainWindow::m_pRenderingTimer_Tick( System::Object^  sender, System::EventArgs^  e )
	{
		for each ( PictureBox^ window in m_pRenderingWindow )
			UpdateRender( window );
	}


	// 描画画面のリサイズイベント
	System::Void
	MainWindow::RenderingWindow_Resize( System::Object ^ sender, System::EventArgs ^ e )
	{
		//PictureBox^ window( (PictureBox^)sender );
		//UpdateRender( window );
	}
#pragma endregion


#pragma region 設定
	// パルサーデータ初期設定
	void
	MainWindow::SetupPulsar()
	{
		m_pPulsar->m_InclinationAngle	 = INIT_INCLINATION_ANGLE;
		m_pPulsar->m_ViewingAngle		 = INIT_VIEWING_ANGLE;
		m_pPulsar->m_MagneticLineCount	 = INIT_MAGNETIC_LINE_COUNT;

		m_pInclinationAngleNumericUpDown->DecimalPlaces  = 1;
		m_pInclinationAngleNumericUpDown->Increment		 = (Decimal)1.0;
		m_pInclinationAngleNumericUpDown->Maximum		 = (Decimal)360.0;
		m_pInclinationAngleNumericUpDown->Minimum		 = (Decimal)0.0;
		m_pInclinationAngleNumericUpDown->Value			 = (Decimal)INIT_INCLINATION_ANGLE;

		m_pViewingAngleNumericUpDown->DecimalPlaces		 = 1;
		m_pViewingAngleNumericUpDown->Increment			 = (Decimal)1.0;
		m_pViewingAngleNumericUpDown->Maximum			 = (Decimal)360.0;
		m_pViewingAngleNumericUpDown->Minimum			 = (Decimal)0.0;
		m_pViewingAngleNumericUpDown->Value				 = (Decimal)INIT_VIEWING_ANGLE;
	}


	// 磁化軸の傾きを更新
	void
	MainWindow::UpdateInclinationAngle()
	{
		// パルス計算
		m_pPulseCalculator->GetPulsarInfo( *m_pPulsar );

		for( int i = 0; i < (int)scene_t::COUNT; i++ )
		{
			Scene* p_scene( GetScene( i ) );
			std::vector<Shape*> vec_p_element( p_scene->GetElement() );

			switch( i )
			{
				case(int)scene_t::PULSAR_MODEL:
					{
						int index( (int)Scene::PulsarModel::COUNT );
						if( vec_p_element.size() == index )
						{
							// 磁化軸の更新
							index = (int)Scene::PulsarModel::MAGNETIC_AXIS;
							Vertex* p_vertex( (Vertex*)vec_p_element[index] );
							p_vertex->SetAngle3D( GetPolarAngle( i ) );

							// 磁力線の更新
							index = (int)Scene::PulsarModel::MAGNETIC_LINE;
							p_vertex = (Vertex*)vec_p_element[index];

							std::vector<std::vector<Vector3Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine3D() );
							m_pPulsar->GetMagneticLineVertex( *p_vec_vec_line );
							p_vertex->SetMultipleLine3D( *p_vec_vec_line );
						}
					}
					break;
				case(int)scene_t::POLAR_CAP_NORTH_BEGIN:
				case(int)scene_t::POLAR_CAP_NORTH_END:
				case(int)scene_t::POLAR_CAP_SOUTH_BEGIN:
				case(int)scene_t::POLAR_CAP_SOUTH_END:
					{
						// カメラの更新
						Camera* p_camera( &( p_scene->GetCamera() ) );
						p_camera->SetRotated( true, GetPolarAngle( i ), 0.0, -1.0, 0.0 );
						p_scene->SetCamera( p_camera );

						// ポーラーキャップの更新
						int index( (int)Scene::PolarCap::COUNT );
						if( vec_p_element.size() == index )
						{
							index = (int)Scene::PolarCap::MAGNETIC_LINE;
							Vertex* p_vertex( (Vertex*)vec_p_element[index] );

							std::vector<Vector3Dd>* vec_p_polar( ( i == (int)scene_t::POLAR_CAP_NORTH_BEGIN ) ? &( m_pPulsar->GetPolarCapNorthBeginVertex() ):
																 ( i == (int)scene_t::POLAR_CAP_NORTH_END   ) ? &( m_pPulsar->GetPolarCapNorthEndVertex()   ):
																 ( i == (int)scene_t::POLAR_CAP_SOUTH_BEGIN ) ? &( m_pPulsar->GetPolarCapSouthBeginVertex() ):
																												&( m_pPulsar->GetPolarCapSouthEndVertex()   ) );

							p_vertex->SetSingleLine3D( *vec_p_polar );
						}
					}
					break;
				case (int)scene_t::SKY_MAP:
					{
						// スカイマップの更新
						int index( (int)Scene::SkyMap::COUNT );
						if( vec_p_element.size() == index )
						{
							index = (int)Scene::SkyMap::MAGNETIC_LINE;
							Vertex* p_vertex( (Vertex*)vec_p_element[index] );

							std::vector<std::vector<Vector2Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine2D() );
							m_pPulsar->GetSkyMapVertex( *p_vec_vec_line );
							p_vertex->SetMultipleLine2D( *p_vec_vec_line );
						}
					}
					break;
				case (int)scene_t::PULSE_PROFILE:
					{
						// パルス波形の更新
						int index( (int)Scene::Pulse::COUNT );
						if( vec_p_element.size() == index )
						{
							index = (int)Scene::Pulse::PULSE;
							Vertex* p_vertex( (Vertex*)vec_p_element[index] );

							m_pPulsar->NormalizePulse();

							std::vector<std::vector<Vector2Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine2D() );
							m_pPulsar->GetPulseVertex( *p_vec_vec_line );
							p_vertex->SetMultipleLine2D( *p_vec_vec_line );
						}
					}
					break;
			}
		}
	}


	// 設定値変更イベント
	System::Void
	MainWindow::NumericUpDown_ValueChanged( System::Object ^ sender, System::EventArgs ^ e )
	{
		NumericUpDown^ numeric( (NumericUpDown^)sender );

		if( numeric == m_pInclinationAngleNumericUpDown )
		{
			EnterCriticalSection( m_pCriticalSection );
			{
				m_pPulsar->m_InclinationAngle = (double)m_pInclinationAngleNumericUpDown->Value;
				UpdateInclinationAngle();
			}
			LeaveCriticalSection( m_pCriticalSection );

		}
		else if( numeric == m_pViewingAngleNumericUpDown )
		{
			 
		}
	}
#pragma endregion


#pragma region シーン生成
	// シーンの取得
	Scene*
	MainWindow::GetScene( int type )
	{
		return ( ( type == (int)scene_t::PULSAR_MODEL	       ) ? m_pPulsarModel	     :
				 ( type == (int)scene_t::POLAR_CAP_NORTH_BEGIN ) ? m_pPolarCapNorthBegin :
				 ( type == (int)scene_t::POLAR_CAP_NORTH_END   ) ? m_pPolarCapNorthEnd   :
				 ( type == (int)scene_t::POLAR_CAP_SOUTH_BEGIN ) ? m_pPolarCapSouthBegin :
				 ( type == (int)scene_t::POLAR_CAP_SOUTH_END   ) ? m_pPolarCapSouthEnd   :
				 ( type == (int)scene_t::SKY_MAP		       ) ? m_pSkyMap		     :
															       m_pPulseProfile      );
	}


	// 極軸アングル取得
	double
	MainWindow::GetPolarAngle( int type )
	{
		const double angle( (double)m_pInclinationAngleNumericUpDown->Value );

		return ( ( ( type == (int)scene_t::POLAR_CAP_SOUTH_BEGIN ) ||
			       ( type == (int)scene_t::POLAR_CAP_SOUTH_END   ) ) ? angle + 180.0 : angle );
	}


	// シーン生成
	void
	MainWindow::CreateScene()
	{
		// カメラの設定
		CreateCamera();

		// ライトの設定
		CreateLight();

		// パルス計算
		m_pPulseCalculator->GetPulsarInfo( *m_pPulsar );

		// 構成要素の生成
		CreatePulsarModel();	// パルサーモデル
		CreatePolarCap();		// ポーラーキャップ
		CreateSkyMap();			// スカイマップ
		CreatePulse();			// パルス
	}


	// カメラ生成
	void
	MainWindow::CreateCamera()
	{
		double angle( (double)m_pInclinationAngleNumericUpDown->Value );
		for( int i = 0; i < (int)scene_t::COUNT; i++ )
		{
			Camera* p_camera( new Camera() );

			//p_camera->SetFov					( 45.0 );
			p_camera->SetZNear					( 0.0 );
			p_camera->SetZFar					( 1000 );
			p_camera->SetCenter					( 0.0, 0.0, 0.0 );
			p_camera->SetBackgroungColor		( 0, 0, 0, 1.0 );

			switch( i )
			{
				case(int)scene_t::PULSAR_MODEL:
					{
						p_camera->SetEye		( 0.0, -5.0, 0.0 );
						p_camera->SetUp			( 0.0, 0.0, 1.0 );
						p_camera->SetBackgroungColor( 0, 1, 14, 1.0 );

						const double rng( 2.0 );
						p_camera->SetRange( -1.9, 2.9, -rng, rng );
					}
					break;
				case(int)scene_t::POLAR_CAP_NORTH_BEGIN:
				case(int)scene_t::POLAR_CAP_NORTH_END:
				case(int)scene_t::POLAR_CAP_SOUTH_BEGIN:
				case(int)scene_t::POLAR_CAP_SOUTH_END:
					{
						const double distance( ( ( i == (int)scene_t::POLAR_CAP_NORTH_BEGIN ) ||
							                     ( i == (int)scene_t::POLAR_CAP_NORTH_END   ) ) ? 0.003 : -0.003 );

						p_camera->SetEye		( 0.0, 0.0, distance );
						p_camera->SetUp			( 0.0, 1.0, 0.0 );
						p_camera->SetRotated	( false, GetPolarAngle( i ), 0.0, -1.0, 0.0 );

						const double rng( 0.001 );
						p_camera->SetRange( -rng, rng, -rng, rng );
					}
					break;
				case(int)scene_t::SKY_MAP:
					{
						p_camera->SetEye		( 0.0, 0.0, 360.0 );
						p_camera->SetUp			( 0.0, 1.0, 0.0 );
						p_camera->SetRange		( 0.0, 360.0, 0.0, 180.0 );
					}
					break;
				case(int)scene_t::PULSE_PROFILE:
					{
						p_camera->SetEye( 0.0, 0.0, 500.0 );
						p_camera->SetUp( 0.0, 1.0, 0.0 );
						p_camera->SetRange( 0.0, 360.0, 0.0, 180.0 );
					}
					break;
			}
			GetScene( i )->SetCamera( p_camera );
		}
	}


	// ライト生成
	void
	MainWindow::CreateLight()
	{
		for( int i = 0; i < (int)scene_t::COUNT; i++ )
		{
			Light* p_light( new Light() );

			p_light->SetPosition	( 0.0, 0.0, 3.0, 360.0 );
			p_light->SetDiffuse		( 255, 255, 255 );
			p_light->SetAmbient		( 65, 65, 65 );
			p_light->SetSpecular	( 255, 255, 255 );

			GetScene( i )->SetLight( p_light );
		}
	}


	// パルサーモデルの生成
	void
	MainWindow::CreatePulsarModel()
	{
		using model_t = Scene::PulsarModel;

		for( int i = 0; i < (int)model_t::COUNT; i++ )
		{
			Shape* p_shape;
			switch( i )
			{
				case (int)model_t::STAR:
					{
						Sphere* p_sphere( new Sphere() );

						p_sphere->SetMainColor( 255, 255, 255, 1.0 );
						p_sphere->SetRadius( 0.05 );
						p_sphere->SetSlices( 36 );
						p_sphere->SetStacks( 36 );

						p_shape = p_sphere;
					}
					break;
				case (int)model_t::ROTATION_AXIS:
					{
						Arrow* p_arrow( new Arrow( line_t::SOLID, Arrow::HeadType::CONE ) );

						double length = 3.0;
						double head_height = 0.2;

						p_arrow->SetMainColor( 255, 0, 0, 1.0 );
						p_arrow->SetShaftWidth( 0.1f );

						std::vector<Vector3Dd> vec_line;
						vec_line.emplace_back( Vector3Dd( 0.0, 0.0, -length * 0.5 ) );
						vec_line.emplace_back( Vector3Dd( 0.0, 0.0, ( length * 0.5 - head_height ) ) );
						p_arrow->SetShaftVertex( vec_line );

						p_arrow->SetHeadRadius( 0.05 );
						p_arrow->SetHeadHeight( head_height );

						p_shape = p_arrow;
					}
					break;
				case (int)model_t::MAGNETIC_AXIS:
					{
						Vertex* p_vertex( new Vertex( line_count_t::SINGLE, line_vec_t::XYZ, line_t::SOLID ) );

						p_vertex->SetMainColor( 255, 255, 255, 1.0 );
						p_vertex->SetWidth( 0.1f );
						p_vertex->SetLength( 2.0f );
						p_vertex->SetAngle3D( (double)m_pInclinationAngleNumericUpDown->Value );

						p_shape = p_vertex;
					}
					break;
				case (int)model_t::LIGTH_CYLINDER:
					{
						Cylinder* p_cylinder( new Cylinder() );

						double height = 3.2;

						p_cylinder->SetMainColor( 0, 0, 255, 0.25f );
						p_cylinder->SetRadius( 1.0 );
						p_cylinder->SetSlices( 36 );
						p_cylinder->SetStacks( 1 );
						p_cylinder->SetHeight( height );
						p_cylinder->SetTranslated( 0.0, 0.0, -height * 0.5 );

						p_shape = p_cylinder;
					}
					break;
				case(int)model_t::MAGNETIC_LINE:
					{
						// 磁力線の生成
						Vertex* p_vertex( new Vertex( line_count_t::MULTIPLE, line_vec_t::XYZ, line_t::SOLID ) );

						p_vertex->SetMainColor( 0, 255, 0, 1.0 );
						p_vertex->SetWidth( 0.5 );

						std::vector<std::vector<Vector3Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine3D() );
						m_pPulsar->GetMagneticLineVertex( *p_vec_vec_line );
						p_vertex->SetMultipleLine3D( *p_vec_vec_line );

						p_shape = p_vertex;
					}
					break;
			}
			m_pPulsarModel->SetElement( p_shape );
		}
	}


	// ポーラーキャップの生成
	void
	MainWindow::CreatePolarCap()
	{
		using polar_t = Scene::PolarCap;

		for( int i = 0; i < 4; i++ )
		{
			std::vector<Vector3Dd>* vec_polar;
			Scene* p_scene;
			switch( i )
			{
				case 0: vec_polar = &( m_pPulsar->GetPolarCapNorthBeginVertex() ); p_scene = m_pPolarCapNorthBegin; break;
				case 1: vec_polar = &( m_pPulsar->GetPolarCapNorthEndVertex()   ); p_scene = m_pPolarCapNorthEnd  ; break;
				case 2: vec_polar = &( m_pPulsar->GetPolarCapSouthBeginVertex() ); p_scene = m_pPolarCapSouthBegin; break;
				case 3: vec_polar = &( m_pPulsar->GetPolarCapSouthEndVertex()   ); p_scene = m_pPolarCapSouthEnd  ; break;
			}

			Shape* p_shape;
			for( int j = 0; j < (int)polar_t::COUNT; j++ )
			{
				switch( j )
				{
					case(int)polar_t::MAGNETIC_LINE:
						{
							Vertex* p_vertex( new Vertex( line_count_t::SINGLE, line_vec_t::XYZ, line_t::POINT ) );

							p_vertex->SetMainColor	 ( 0, 255, 0, 1.0 );
							p_vertex->SetWidth		 ( 2.0 );
							p_vertex->SetSingleLine3D( *vec_polar );

							p_shape = p_vertex;
						}
						break;
				}
			}
			p_scene->SetElement( p_shape );
		}
	}


	// スカイマップの生成
	void
	MainWindow::CreateSkyMap()
	{
		using sky_t = Scene::SkyMap;

		Shape* p_shape;
		for( int j = 0; j < (int)sky_t::COUNT; j++ )
		{
			switch( j )
			{
				case(int)sky_t::MAGNETIC_LINE:
					{
						Vertex* p_vertex( new Vertex( line_count_t::MULTIPLE, line_vec_t::XY, line_t::SOLID ) );

						p_vertex->SetMainColor     ( 0, 255, 0, 1.0 );
						p_vertex->SetWidth		   ( 0.5 );

						std::vector<std::vector<Vector2Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine2D() );
						m_pPulsar->GetSkyMapVertex( *p_vec_vec_line );
						p_vertex->SetMultipleLine2D( *p_vec_vec_line );

						p_shape = p_vertex;
					}
					break;
			}
		}
		m_pSkyMap->SetElement( p_shape );
	}


	// パルスの生成
	void
	MainWindow::CreatePulse()
	{
		using pulse_t = Scene::Pulse;

		Shape* p_shape;
		for( int j = 0; j < (int)pulse_t::COUNT; j++ )
		{
			switch( j )
			{
				case(int)pulse_t::PULSE:
					{
						Vertex* p_vertex( new Vertex( line_count_t::MULTIPLE, line_vec_t::XZ, line_t::SOLID ) );

						p_vertex->SetMainColor( 0, 255, 0, 1.0 );
						p_vertex->SetSubColor( 0, 0, 0, 1.0 );
						p_vertex->SetWidth( 0.5 );

						m_pPulsar->NormalizePulse();

						std::vector<std::vector<Vector2Dd>>* p_vec_vec_line( &p_vertex->GetMultipleLine2D() );
						m_pPulsar->GetPulseVertex( *p_vec_vec_line );

						p_vertex->SetMultipleLine2D( *p_vec_vec_line );

						std::vector<double> y_dimension;
						size_t size( p_vec_vec_line->size() );
						y_dimension.reserve( size );
						for( size_t i = 0; i < size; i++ )
							y_dimension.emplace_back( PULSE_DISPLAY_DEPTH * i );

						p_vertex->SetThridDimensionMultipleValue( y_dimension );

						p_shape = p_vertex;
					}
					break;
			}
		}
		m_pPulseProfile->SetElement( p_shape );
	}
#pragma endregion
}