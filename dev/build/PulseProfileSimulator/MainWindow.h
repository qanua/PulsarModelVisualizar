#pragma once

#include "Rendering/Renderer.h"
#include "Data/Pulsar.h"
#include "PulseCalculator.h"
#include "Scene/Scene.h"
#include "Primitive/Shape.h"
#include "Math/Vector3D.h"
#include "Math/Vector2D.h"

#include <windows.h>


namespace PulseProfileSimulator
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MainWindow の概要
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
#pragma region クラス
	private:
		ref class ScaleNumber
		{
		public:
			String^ number;		// グラフ目盛り数値
			float   x;			// 描画の x 座標
			float   y;			// 描画の y 座標

		public:
			ScaleNumber( String^ str_num, float pos_x, float pos_y ) :number( str_num ), x( pos_x ), y( pos_y ){};
			~ScaleNumber(){};
		};


	private:
		enum class Alignment
		{
			TOP,
			BOTTOM,
			LEFT,
			RIGHT
		};
#pragma endregion


#pragma region メンバ変数
	private:
		Renderer^				m_pRenderer;									// レンダラ
		Pulsar*					m_pPulsar;										// パルサー
		PulseCalculator*		m_pPulseCalculator;								// パルス計算機
		CRITICAL_SECTION*		m_pCriticalSection;								// クリティカルセクション

		Scene*					m_pPulsarModel;									// パルサーモデル
		Scene*					m_pPolarCapNorthBegin;							// ポーラーキャップ北始点
		Scene*					m_pPolarCapNorthEnd;							// ポーラーキャップ北終点
		Scene*					m_pPolarCapSouthBegin;							// ポーラーキャップ南始点
		Scene*					m_pPolarCapSouthEnd;							// ポーラーキャップ南終点
		Scene*					m_pSkyMap;										// スカイマップ
		Scene*					m_pPulseProfile;								// パルス波形

		bool					m_IsDrag;										// マウスドラッグフラグ
		Point					m_MouseDownPoint;								// マウス押下座標

		array<PictureBox^>^		m_pRenderingWindow;								// レンダリング画面
#pragma endregion


#pragma region 定数
	private:
		String^					INCLINATION_ANGLE_LABEL  = "inclination angle";
		String^					VIEWING_ANGLE_LABEL		 = "viewing angle";
		String^					FONT_MEIRYO_UI			 = "Meiryo UI";

		const double			INIT_INCLINATION_ANGLE	 = 57.0;				// 磁化軸の傾き   [degree]
		const double			INIT_VIEWING_ANGLE		 = 57.0;				// 視線方向の傾き [degree]
		const int				INIT_MAGNETIC_LINE_COUNT = 60;					// 磁力線の本数
	private: System::Windows::Forms::SplitContainer^  splitContainer9;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Panel^  panel3;
	private: System::Windows::Forms::Panel^  panel4;


	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::PictureBox^  pictureBox3;
	private: System::Windows::Forms::Panel^  panel5;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::SplitContainer^  splitContainer3;
	private: System::Windows::Forms::SplitContainer^  splitContainer5;
	private: System::Windows::Forms::Panel^  panel6;
	private: System::Windows::Forms::Panel^  panel7;
	private: System::Windows::Forms::SplitContainer^  splitContainer7;
	private: System::Windows::Forms::SplitContainer^  splitContainer8;
	private: System::Windows::Forms::Panel^  panel8;
	private: System::Windows::Forms::Panel^  panel9;
	private: System::Windows::Forms::SplitContainer^  splitContainer10;
	private: System::Windows::Forms::SplitContainer^  splitContainer11;
	private: System::Windows::Forms::PictureBox^  m_pPolarCapAzimuthPictureBox;



	private: System::Windows::Forms::PictureBox^  pictureBox5;
	private: System::Windows::Forms::TextBox^  textBox1;



	private: System::Windows::Forms::PictureBox^  pictureBox6;
	private: System::Windows::Forms::PictureBox^  pictureBox7;
	private: System::Windows::Forms::PictureBox^  pictureBox8;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::PictureBox^  m_pPulseScaleHGridPictureBox;
	private: System::Windows::Forms::PictureBox^  m_pPulseScaleHNumberPictureBox;
	private: System::Windows::Forms::PictureBox^  m_pSkyMapScaleHGridPictureBox;
	private: System::Windows::Forms::PictureBox^  m_pSkyMapScaleHNumberPictureBox;
	private: System::Windows::Forms::PictureBox^  m_pSkyMapScaleVNumberPictureBox;
	private: System::Windows::Forms::PictureBox^  m_pSkyMapScaleVGridPictureBox;
	private: System::Windows::Forms::Label^  m_pPulseScaleHTitleLabel;

	private: System::Windows::Forms::Label^  m_pSkyMapScaleHTitleLabel;
	private: System::Windows::Forms::PictureBox^  m_pSkyMapScaleVTitlePictureBox;













			 const double			PULSE_DISPLAY_DEPTH = 1.0;					// パルス表示間隔
#pragma endregion


#pragma region 初期化 / 終了処理
	public:
		/** @brief コンストラクタ */
		MainWindow( void );


	protected:
		/** @brief デストラクタ */
		~MainWindow();
#pragma endregion


	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::IContainer^		 components;

		System::Windows::Forms::Timer^			 m_pRenderingTimer;

		System::Windows::Forms::PictureBox^		 m_pPulsarModelPictureBox;

		System::Windows::Forms::PictureBox^		 m_pPolarCapNorthBeginPictureBox;
		System::Windows::Forms::PictureBox^		 m_pPolarCapSouthEndPictureBox;
		System::Windows::Forms::PictureBox^		 m_pPolarCapNorthEndPictureBox;
		System::Windows::Forms::PictureBox^		 m_pPolarCapSouthBeginPictureBox;

		System::Windows::Forms::PictureBox^		 m_pSkyMapPictureBox;
		System::Windows::Forms::PictureBox^		 m_pPulseProfilePictureBox;


		System::Windows::Forms::NumericUpDown^	 m_pViewingAngleNumericUpDown;
		System::Windows::Forms::NumericUpDown^	 m_pInclinationAngleNumericUpDown;	
		
		System::Windows::Forms::Panel^			 m_pViewingAngleLine;
	private: System::Windows::Forms::Label^  m_pInclinationAngleLabel;
	private: System::Windows::Forms::Label^  m_pViewingAngleLabel;






		System::Windows::Forms::SplitContainer^  splitContainer1;
		System::Windows::Forms::SplitContainer^  splitContainer2;

		System::Windows::Forms::SplitContainer^  splitContainer4;

		System::Windows::Forms::SplitContainer^  splitContainer6;



#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent( void )
		{
			this->components = ( gcnew System::ComponentModel::Container() );
			System::ComponentModel::ComponentResourceManager^  resources = ( gcnew System::ComponentModel::ComponentResourceManager( MainWindow::typeid ) );
			this->m_pPulsarModelPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pRenderingTimer = ( gcnew System::Windows::Forms::Timer( this->components ) );
			this->splitContainer1 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->splitContainer4 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->panel5 = ( gcnew System::Windows::Forms::Panel() );
			this->splitContainer3 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->splitContainer5 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->panel6 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPolarCapNorthBeginPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->panel7 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPolarCapSouthEndPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->splitContainer7 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->splitContainer10 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->splitContainer11 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->m_pPolarCapAzimuthPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->splitContainer8 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->panel8 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPolarCapNorthEndPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->panel9 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPolarCapSouthBeginPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->pictureBox7 = ( gcnew System::Windows::Forms::PictureBox() );
			this->pictureBox8 = ( gcnew System::Windows::Forms::PictureBox() );
			this->textBox2 = ( gcnew System::Windows::Forms::TextBox() );
			this->pictureBox6 = ( gcnew System::Windows::Forms::PictureBox() );
			this->pictureBox5 = ( gcnew System::Windows::Forms::PictureBox() );
			this->textBox1 = ( gcnew System::Windows::Forms::TextBox() );
			this->pictureBox2 = ( gcnew System::Windows::Forms::PictureBox() );
			this->pictureBox1 = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pInclinationAngleNumericUpDown = ( gcnew System::Windows::Forms::NumericUpDown() );
			this->m_pViewingAngleNumericUpDown = ( gcnew System::Windows::Forms::NumericUpDown() );
			this->m_pInclinationAngleLabel = ( gcnew System::Windows::Forms::Label() );
			this->m_pViewingAngleLabel = ( gcnew System::Windows::Forms::Label() );
			this->splitContainer6 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->panel3 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pSkyMapScaleVNumberPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pSkyMapScaleVGridPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pSkyMapScaleHNumberPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pSkyMapScaleHGridPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->panel4 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pViewingAngleLine = ( gcnew System::Windows::Forms::Panel() );
			this->m_pSkyMapPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->splitContainer2 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->panel1 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPulseScaleHNumberPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pPulseScaleHGridPictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->panel2 = ( gcnew System::Windows::Forms::Panel() );
			this->m_pPulseProfilePictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			this->splitContainer9 = ( gcnew System::Windows::Forms::SplitContainer() );
			this->pictureBox3 = ( gcnew System::Windows::Forms::PictureBox() );
			this->m_pPulseScaleHTitleLabel = ( gcnew System::Windows::Forms::Label() );
			this->m_pSkyMapScaleHTitleLabel = ( gcnew System::Windows::Forms::Label() );
			this->m_pSkyMapScaleVTitlePictureBox = ( gcnew System::Windows::Forms::PictureBox() );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulsarModelPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer1 ) )->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer4 ) )->BeginInit();
			this->splitContainer4->Panel1->SuspendLayout();
			this->splitContainer4->Panel2->SuspendLayout();
			this->splitContainer4->SuspendLayout();
			this->panel5->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer3 ) )->BeginInit();
			this->splitContainer3->Panel1->SuspendLayout();
			this->splitContainer3->Panel2->SuspendLayout();
			this->splitContainer3->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer5 ) )->BeginInit();
			this->splitContainer5->Panel1->SuspendLayout();
			this->splitContainer5->Panel2->SuspendLayout();
			this->splitContainer5->SuspendLayout();
			this->panel6->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapNorthBeginPictureBox ) )->BeginInit();
			this->panel7->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapSouthEndPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer7 ) )->BeginInit();
			this->splitContainer7->Panel1->SuspendLayout();
			this->splitContainer7->Panel2->SuspendLayout();
			this->splitContainer7->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer10 ) )->BeginInit();
			this->splitContainer10->Panel2->SuspendLayout();
			this->splitContainer10->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer11 ) )->BeginInit();
			this->splitContainer11->Panel1->SuspendLayout();
			this->splitContainer11->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapAzimuthPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer8 ) )->BeginInit();
			this->splitContainer8->Panel1->SuspendLayout();
			this->splitContainer8->Panel2->SuspendLayout();
			this->splitContainer8->SuspendLayout();
			this->panel8->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapNorthEndPictureBox ) )->BeginInit();
			this->panel9->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapSouthBeginPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox7 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox8 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox6 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox5 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox2 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox1 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pInclinationAngleNumericUpDown ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pViewingAngleNumericUpDown ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer6 ) )->BeginInit();
			this->splitContainer6->Panel1->SuspendLayout();
			this->splitContainer6->SuspendLayout();
			this->panel3->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVNumberPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVGridPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleHNumberPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleHGridPictureBox ) )->BeginInit();
			this->panel4->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer2 ) )->BeginInit();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->panel1->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseScaleHNumberPictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseScaleHGridPictureBox ) )->BeginInit();
			this->panel2->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseProfilePictureBox ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer9 ) )->BeginInit();
			this->splitContainer9->Panel1->SuspendLayout();
			this->splitContainer9->Panel2->SuspendLayout();
			this->splitContainer9->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox3 ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVTitlePictureBox ) )->BeginInit();
			this->SuspendLayout();
			// 
			// m_pPulsarModelPictureBox
			// 
			this->m_pPulsarModelPictureBox->BackColor = System::Drawing::Color::FromArgb( static_cast<System::Int32>( static_cast<System::Byte>( 0 ) ),
				static_cast<System::Int32>( static_cast<System::Byte>( 1 ) ), static_cast<System::Int32>( static_cast<System::Byte>( 14 ) ) );
			this->m_pPulsarModelPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPulsarModelPictureBox->Location = System::Drawing::Point( 0, 19 );
			this->m_pPulsarModelPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPulsarModelPictureBox->Name = L"m_pPulsarModelPictureBox";
			this->m_pPulsarModelPictureBox->Size = System::Drawing::Size( 492, 410 );
			this->m_pPulsarModelPictureBox->TabIndex = 0;
			this->m_pPulsarModelPictureBox->TabStop = false;
			this->m_pPulsarModelPictureBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseDown );
			this->m_pPulsarModelPictureBox->MouseMove += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseMove );
			this->m_pPulsarModelPictureBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseUp );
			this->m_pPulsarModelPictureBox->Resize += gcnew System::EventHandler( this, &MainWindow::RenderingWindow_Resize );
			// 
			// m_pRenderingTimer
			// 
			this->m_pRenderingTimer->Enabled = true;
			this->m_pRenderingTimer->Tick += gcnew System::EventHandler( this, &MainWindow::m_pRenderingTimer_Tick );
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer1->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add( this->splitContainer4 );
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add( this->splitContainer6 );
			this->splitContainer1->Size = System::Drawing::Size( 989, 869 );
			this->splitContainer1->SplitterDistance = 429;
			this->splitContainer1->SplitterWidth = 1;
			this->splitContainer1->TabIndex = 1;
			// 
			// splitContainer4
			// 
			this->splitContainer4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer4->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer4->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer4->Name = L"splitContainer4";
			// 
			// splitContainer4.Panel1
			// 
			this->splitContainer4->Panel1->Controls->Add( this->panel5 );
			this->splitContainer4->Panel1->Padding = System::Windows::Forms::Padding( 0, 19, 0, 10 );
			// 
			// splitContainer4.Panel2
			// 
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox7 );
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox8 );
			this->splitContainer4->Panel2->Controls->Add( this->textBox2 );
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox6 );
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox5 );
			this->splitContainer4->Panel2->Controls->Add( this->textBox1 );
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox2 );
			this->splitContainer4->Panel2->Controls->Add( this->pictureBox1 );
			this->splitContainer4->Panel2->Controls->Add( this->m_pInclinationAngleNumericUpDown );
			this->splitContainer4->Panel2->Controls->Add( this->m_pViewingAngleNumericUpDown );
			this->splitContainer4->Panel2->Controls->Add( this->m_pInclinationAngleLabel );
			this->splitContainer4->Panel2->Controls->Add( this->m_pViewingAngleLabel );
			this->splitContainer4->Panel2->Controls->Add( this->m_pPulsarModelPictureBox );
			this->splitContainer4->Panel2->Padding = System::Windows::Forms::Padding( 0, 19, 57, 0 );
			this->splitContainer4->Size = System::Drawing::Size( 989, 429 );
			this->splitContainer4->SplitterDistance = 439;
			this->splitContainer4->SplitterWidth = 1;
			this->splitContainer4->TabIndex = 0;
			// 
			// panel5
			// 
			this->panel5->Controls->Add( this->splitContainer3 );
			this->panel5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel5->Location = System::Drawing::Point( 0, 19 );
			this->panel5->Margin = System::Windows::Forms::Padding( 0 );
			this->panel5->Name = L"panel5";
			this->panel5->Padding = System::Windows::Forms::Padding( 39, 20, 20, 30 );
			this->panel5->Size = System::Drawing::Size( 439, 400 );
			this->panel5->TabIndex = 0;
			// 
			// splitContainer3
			// 
			this->splitContainer3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer3->Location = System::Drawing::Point( 39, 20 );
			this->splitContainer3->Margin = System::Windows::Forms::Padding( 1 );
			this->splitContainer3->Name = L"splitContainer3";
			// 
			// splitContainer3.Panel1
			// 
			this->splitContainer3->Panel1->Controls->Add( this->splitContainer5 );
			// 
			// splitContainer3.Panel2
			// 
			this->splitContainer3->Panel2->Controls->Add( this->splitContainer7 );
			this->splitContainer3->Size = System::Drawing::Size( 380, 350 );
			this->splitContainer3->SplitterDistance = 162;
			this->splitContainer3->SplitterWidth = 1;
			this->splitContainer3->TabIndex = 0;
			// 
			// splitContainer5
			// 
			this->splitContainer5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer5->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer5->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer5->Name = L"splitContainer5";
			this->splitContainer5->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer5.Panel1
			// 
			this->splitContainer5->Panel1->Controls->Add( this->panel6 );
			this->splitContainer5->Panel1->Padding = System::Windows::Forms::Padding( 0, 0, 0, 13 );
			// 
			// splitContainer5.Panel2
			// 
			this->splitContainer5->Panel2->Controls->Add( this->panel7 );
			this->splitContainer5->Panel2->Padding = System::Windows::Forms::Padding( 0, 12, 0, 0 );
			this->splitContainer5->Size = System::Drawing::Size( 162, 350 );
			this->splitContainer5->SplitterDistance = 175;
			this->splitContainer5->SplitterWidth = 1;
			this->splitContainer5->TabIndex = 0;
			// 
			// panel6
			// 
			this->panel6->BackColor = System::Drawing::Color::White;
			this->panel6->Controls->Add( this->m_pPolarCapNorthBeginPictureBox );
			this->panel6->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel6->Location = System::Drawing::Point( 0, 0 );
			this->panel6->Margin = System::Windows::Forms::Padding( 0 );
			this->panel6->Name = L"panel6";
			this->panel6->Padding = System::Windows::Forms::Padding( 1 );
			this->panel6->Size = System::Drawing::Size( 162, 162 );
			this->panel6->TabIndex = 0;
			// 
			// m_pPolarCapNorthBeginPictureBox
			// 
			this->m_pPolarCapNorthBeginPictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pPolarCapNorthBeginPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPolarCapNorthBeginPictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pPolarCapNorthBeginPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPolarCapNorthBeginPictureBox->Name = L"m_pPolarCapNorthBeginPictureBox";
			this->m_pPolarCapNorthBeginPictureBox->Size = System::Drawing::Size( 160, 160 );
			this->m_pPolarCapNorthBeginPictureBox->TabIndex = 0;
			this->m_pPolarCapNorthBeginPictureBox->TabStop = false;
			this->m_pPolarCapNorthBeginPictureBox->Resize += gcnew System::EventHandler( this, &MainWindow::RenderingWindow_Resize );
			// 
			// panel7
			// 
			this->panel7->BackColor = System::Drawing::Color::White;
			this->panel7->Controls->Add( this->m_pPolarCapSouthEndPictureBox );
			this->panel7->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel7->Location = System::Drawing::Point( 0, 12 );
			this->panel7->Margin = System::Windows::Forms::Padding( 0 );
			this->panel7->Name = L"panel7";
			this->panel7->Padding = System::Windows::Forms::Padding( 1 );
			this->panel7->Size = System::Drawing::Size( 162, 162 );
			this->panel7->TabIndex = 0;
			// 
			// m_pPolarCapSouthEndPictureBox
			// 
			this->m_pPolarCapSouthEndPictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pPolarCapSouthEndPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPolarCapSouthEndPictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pPolarCapSouthEndPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPolarCapSouthEndPictureBox->Name = L"m_pPolarCapSouthEndPictureBox";
			this->m_pPolarCapSouthEndPictureBox->Size = System::Drawing::Size( 160, 160 );
			this->m_pPolarCapSouthEndPictureBox->TabIndex = 0;
			this->m_pPolarCapSouthEndPictureBox->TabStop = false;
			this->m_pPolarCapSouthEndPictureBox->Resize += gcnew System::EventHandler( this, &MainWindow::RenderingWindow_Resize );
			// 
			// splitContainer7
			// 
			this->splitContainer7->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer7->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer7->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer7->Name = L"splitContainer7";
			// 
			// splitContainer7.Panel1
			// 
			this->splitContainer7->Panel1->Controls->Add( this->splitContainer10 );
			// 
			// splitContainer7.Panel2
			// 
			this->splitContainer7->Panel2->Controls->Add( this->splitContainer8 );
			this->splitContainer7->Size = System::Drawing::Size( 217, 350 );
			this->splitContainer7->SplitterDistance = 54;
			this->splitContainer7->SplitterWidth = 1;
			this->splitContainer7->TabIndex = 0;
			// 
			// splitContainer10
			// 
			this->splitContainer10->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer10->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer10->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer10->Name = L"splitContainer10";
			this->splitContainer10->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer10.Panel2
			// 
			this->splitContainer10->Panel2->Controls->Add( this->splitContainer11 );
			this->splitContainer10->Size = System::Drawing::Size( 54, 350 );
			this->splitContainer10->SplitterDistance = 144;
			this->splitContainer10->SplitterWidth = 1;
			this->splitContainer10->TabIndex = 0;
			// 
			// splitContainer11
			// 
			this->splitContainer11->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer11->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer11->Name = L"splitContainer11";
			this->splitContainer11->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer11.Panel1
			// 
			this->splitContainer11->Panel1->Controls->Add( this->m_pPolarCapAzimuthPictureBox );
			this->splitContainer11->Size = System::Drawing::Size( 54, 205 );
			this->splitContainer11->SplitterDistance = 60;
			this->splitContainer11->SplitterWidth = 1;
			this->splitContainer11->TabIndex = 0;
			// 
			// m_pPolarCapAzimuthPictureBox
			// 
			this->m_pPolarCapAzimuthPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPolarCapAzimuthPictureBox->Location = System::Drawing::Point( 0, 0 );
			this->m_pPolarCapAzimuthPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPolarCapAzimuthPictureBox->Name = L"m_pPolarCapAzimuthPictureBox";
			this->m_pPolarCapAzimuthPictureBox->Size = System::Drawing::Size( 54, 60 );
			this->m_pPolarCapAzimuthPictureBox->TabIndex = 0;
			this->m_pPolarCapAzimuthPictureBox->TabStop = false;
			// 
			// splitContainer8
			// 
			this->splitContainer8->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer8->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer8->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer8->Name = L"splitContainer8";
			this->splitContainer8->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer8.Panel1
			// 
			this->splitContainer8->Panel1->Controls->Add( this->panel8 );
			this->splitContainer8->Panel1->Padding = System::Windows::Forms::Padding( 0, 0, 0, 13 );
			// 
			// splitContainer8.Panel2
			// 
			this->splitContainer8->Panel2->Controls->Add( this->panel9 );
			this->splitContainer8->Panel2->Padding = System::Windows::Forms::Padding( 0, 12, 0, 0 );
			this->splitContainer8->Size = System::Drawing::Size( 162, 350 );
			this->splitContainer8->SplitterDistance = 175;
			this->splitContainer8->SplitterWidth = 1;
			this->splitContainer8->TabIndex = 0;
			// 
			// panel8
			// 
			this->panel8->BackColor = System::Drawing::Color::White;
			this->panel8->Controls->Add( this->m_pPolarCapNorthEndPictureBox );
			this->panel8->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel8->Location = System::Drawing::Point( 0, 0 );
			this->panel8->Margin = System::Windows::Forms::Padding( 0 );
			this->panel8->Name = L"panel8";
			this->panel8->Padding = System::Windows::Forms::Padding( 1 );
			this->panel8->Size = System::Drawing::Size( 162, 162 );
			this->panel8->TabIndex = 0;
			// 
			// m_pPolarCapNorthEndPictureBox
			// 
			this->m_pPolarCapNorthEndPictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pPolarCapNorthEndPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPolarCapNorthEndPictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pPolarCapNorthEndPictureBox->Name = L"m_pPolarCapNorthEndPictureBox";
			this->m_pPolarCapNorthEndPictureBox->Size = System::Drawing::Size( 160, 160 );
			this->m_pPolarCapNorthEndPictureBox->TabIndex = 0;
			this->m_pPolarCapNorthEndPictureBox->TabStop = false;
			this->m_pPolarCapNorthEndPictureBox->Resize += gcnew System::EventHandler( this, &MainWindow::RenderingWindow_Resize );
			// 
			// panel9
			// 
			this->panel9->BackColor = System::Drawing::Color::White;
			this->panel9->Controls->Add( this->m_pPolarCapSouthBeginPictureBox );
			this->panel9->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel9->Location = System::Drawing::Point( 0, 12 );
			this->panel9->Margin = System::Windows::Forms::Padding( 0 );
			this->panel9->Name = L"panel9";
			this->panel9->Padding = System::Windows::Forms::Padding( 1 );
			this->panel9->Size = System::Drawing::Size( 162, 162 );
			this->panel9->TabIndex = 0;
			// 
			// m_pPolarCapSouthBeginPictureBox
			// 
			this->m_pPolarCapSouthBeginPictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pPolarCapSouthBeginPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPolarCapSouthBeginPictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pPolarCapSouthBeginPictureBox->Name = L"m_pPolarCapSouthBeginPictureBox";
			this->m_pPolarCapSouthBeginPictureBox->Size = System::Drawing::Size( 160, 160 );
			this->m_pPolarCapSouthBeginPictureBox->TabIndex = 0;
			this->m_pPolarCapSouthBeginPictureBox->TabStop = false;
			this->m_pPolarCapSouthBeginPictureBox->Resize += gcnew System::EventHandler( this, &MainWindow::RenderingWindow_Resize );
			// 
			// pictureBox7
			// 
			this->pictureBox7->BackgroundImage = ( cli::safe_cast<System::Drawing::Image^>( resources->GetObject( L"pictureBox7.BackgroundImage" ) ) );
			this->pictureBox7->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pictureBox7->Location = System::Drawing::Point( 477, 358 );
			this->pictureBox7->Name = L"pictureBox7";
			this->pictureBox7->Size = System::Drawing::Size( 20, 15 );
			this->pictureBox7->TabIndex = 9;
			this->pictureBox7->TabStop = false;
			// 
			// pictureBox8
			// 
			this->pictureBox8->BackgroundImage = ( cli::safe_cast<System::Drawing::Image^>( resources->GetObject( L"pictureBox8.BackgroundImage" ) ) );
			this->pictureBox8->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pictureBox8->Location = System::Drawing::Point( 477, 338 );
			this->pictureBox8->Name = L"pictureBox8";
			this->pictureBox8->Size = System::Drawing::Size( 20, 15 );
			this->pictureBox8->TabIndex = 8;
			this->pictureBox8->TabStop = false;
			// 
			// textBox2
			// 
			this->textBox2->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->textBox2->Location = System::Drawing::Point( 410, 338 );
			this->textBox2->Margin = System::Windows::Forms::Padding( 0 );
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size( 56, 35 );
			this->textBox2->TabIndex = 7;
			this->textBox2->Text = L"999";
			this->textBox2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// pictureBox6
			// 
			this->pictureBox6->BackgroundImage = ( cli::safe_cast<System::Drawing::Image^>( resources->GetObject( L"pictureBox6.BackgroundImage" ) ) );
			this->pictureBox6->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pictureBox6->Location = System::Drawing::Point( 477, 262 );
			this->pictureBox6->Name = L"pictureBox6";
			this->pictureBox6->Size = System::Drawing::Size( 20, 15 );
			this->pictureBox6->TabIndex = 6;
			this->pictureBox6->TabStop = false;
			// 
			// pictureBox5
			// 
			this->pictureBox5->BackgroundImage = ( cli::safe_cast<System::Drawing::Image^>( resources->GetObject( L"pictureBox5.BackgroundImage" ) ) );
			this->pictureBox5->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pictureBox5->Location = System::Drawing::Point( 477, 242 );
			this->pictureBox5->Name = L"pictureBox5";
			this->pictureBox5->Size = System::Drawing::Size( 20, 15 );
			this->pictureBox5->TabIndex = 5;
			this->pictureBox5->TabStop = false;
			// 
			// textBox1
			// 
			this->textBox1->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->textBox1->Location = System::Drawing::Point( 410, 242 );
			this->textBox1->Margin = System::Windows::Forms::Padding( 0 );
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size( 56, 35 );
			this->textBox1->TabIndex = 4;
			this->textBox1->Text = L"999";
			this->textBox1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// pictureBox2
			// 
			this->pictureBox2->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBox2->Location = System::Drawing::Point( 417, 99 );
			this->pictureBox2->Margin = System::Windows::Forms::Padding( 0 );
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size( 40, 40 );
			this->pictureBox2->TabIndex = 1;
			this->pictureBox2->TabStop = false;
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Fuchsia;
			this->pictureBox1->Location = System::Drawing::Point( 387, 69 );
			this->pictureBox1->Margin = System::Windows::Forms::Padding( 0 );
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size( 100, 100 );
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// m_pInclinationAngleNumericUpDown
			// 
			this->m_pInclinationAngleNumericUpDown->Location = System::Drawing::Point( 343, 258 );
			this->m_pInclinationAngleNumericUpDown->Name = L"m_pInclinationAngleNumericUpDown";
			this->m_pInclinationAngleNumericUpDown->Size = System::Drawing::Size( 55, 19 );
			this->m_pInclinationAngleNumericUpDown->TabIndex = 1;
			this->m_pInclinationAngleNumericUpDown->ValueChanged += gcnew System::EventHandler( this, &MainWindow::NumericUpDown_ValueChanged );
			// 
			// m_pViewingAngleNumericUpDown
			// 
			this->m_pViewingAngleNumericUpDown->Location = System::Drawing::Point( 343, 351 );
			this->m_pViewingAngleNumericUpDown->Name = L"m_pViewingAngleNumericUpDown";
			this->m_pViewingAngleNumericUpDown->Size = System::Drawing::Size( 55, 19 );
			this->m_pViewingAngleNumericUpDown->TabIndex = 3;
			this->m_pViewingAngleNumericUpDown->ValueChanged += gcnew System::EventHandler( this, &MainWindow::NumericUpDown_ValueChanged );
			// 
			// m_pInclinationAngleLabel
			// 
			this->m_pInclinationAngleLabel->AutoSize = true;
			this->m_pInclinationAngleLabel->BackColor = System::Drawing::Color::Transparent;
			this->m_pInclinationAngleLabel->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->m_pInclinationAngleLabel->ForeColor = System::Drawing::Color::White;
			this->m_pInclinationAngleLabel->Location = System::Drawing::Point( 358, 214 );
			this->m_pInclinationAngleLabel->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pInclinationAngleLabel->Name = L"m_pInclinationAngleLabel";
			this->m_pInclinationAngleLabel->Size = System::Drawing::Size( 139, 20 );
			this->m_pInclinationAngleLabel->TabIndex = 0;
			this->m_pInclinationAngleLabel->Text = L"Inclination Angle";
			// 
			// m_pViewingAngleLabel
			// 
			this->m_pViewingAngleLabel->AutoSize = true;
			this->m_pViewingAngleLabel->BackColor = System::Drawing::Color::Transparent;
			this->m_pViewingAngleLabel->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->m_pViewingAngleLabel->ForeColor = System::Drawing::Color::White;
			this->m_pViewingAngleLabel->Location = System::Drawing::Point( 358, 309 );
			this->m_pViewingAngleLabel->Name = L"m_pViewingAngleLabel";
			this->m_pViewingAngleLabel->Size = System::Drawing::Size( 119, 20 );
			this->m_pViewingAngleLabel->TabIndex = 2;
			this->m_pViewingAngleLabel->Text = L"Viewing Angle";
			// 
			// splitContainer6
			// 
			this->splitContainer6->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer6->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer6->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer6->Name = L"splitContainer6";
			// 
			// splitContainer6.Panel1
			// 
			this->splitContainer6->Panel1->BackColor = System::Drawing::Color::FromArgb( static_cast<System::Int32>( static_cast<System::Byte>( 0 ) ),
				static_cast<System::Int32>( static_cast<System::Byte>( 1 ) ), static_cast<System::Int32>( static_cast<System::Byte>( 14 ) ) );
			this->splitContainer6->Panel1->Controls->Add( this->panel3 );
			this->splitContainer6->Panel1->Padding = System::Windows::Forms::Padding( 0, 9, 0, 20 );
			this->splitContainer6->Size = System::Drawing::Size( 989, 439 );
			this->splitContainer6->SplitterDistance = 779;
			this->splitContainer6->SplitterWidth = 1;
			this->splitContainer6->TabIndex = 1;
			// 
			// panel3
			// 
			this->panel3->Controls->Add( this->m_pSkyMapScaleVTitlePictureBox );
			this->panel3->Controls->Add( this->m_pSkyMapScaleHTitleLabel );
			this->panel3->Controls->Add( this->m_pSkyMapScaleVNumberPictureBox );
			this->panel3->Controls->Add( this->m_pSkyMapScaleVGridPictureBox );
			this->panel3->Controls->Add( this->m_pSkyMapScaleHNumberPictureBox );
			this->panel3->Controls->Add( this->m_pSkyMapScaleHGridPictureBox );
			this->panel3->Controls->Add( this->panel4 );
			this->panel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel3->Location = System::Drawing::Point( 0, 9 );
			this->panel3->Margin = System::Windows::Forms::Padding( 0 );
			this->panel3->Name = L"panel3";
			this->panel3->Padding = System::Windows::Forms::Padding( 39, 19, 18, 29 );
			this->panel3->Size = System::Drawing::Size( 779, 410 );
			this->panel3->TabIndex = 2;
			// 
			// m_pSkyMapScaleVNumberPictureBox
			// 
			this->m_pSkyMapScaleVNumberPictureBox->Location = System::Drawing::Point( 0, 0 );
			this->m_pSkyMapScaleVNumberPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleVNumberPictureBox->Name = L"m_pSkyMapScaleVNumberPictureBox";
			this->m_pSkyMapScaleVNumberPictureBox->Size = System::Drawing::Size( 29, 392 );
			this->m_pSkyMapScaleVNumberPictureBox->TabIndex = 6;
			this->m_pSkyMapScaleVNumberPictureBox->TabStop = false;
			// 
			// m_pSkyMapScaleVGridPictureBox
			// 
			this->m_pSkyMapScaleVGridPictureBox->Location = System::Drawing::Point( 29, 19 );
			this->m_pSkyMapScaleVGridPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleVGridPictureBox->Name = L"m_pSkyMapScaleVGridPictureBox";
			this->m_pSkyMapScaleVGridPictureBox->Size = System::Drawing::Size( 10, 362 );
			this->m_pSkyMapScaleVGridPictureBox->TabIndex = 5;
			this->m_pSkyMapScaleVGridPictureBox->TabStop = false;
			// 
			// m_pSkyMapScaleHNumberPictureBox
			// 
			this->m_pSkyMapScaleHNumberPictureBox->Location = System::Drawing::Point( 0, 391 );
			this->m_pSkyMapScaleHNumberPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleHNumberPictureBox->Name = L"m_pSkyMapScaleHNumberPictureBox";
			this->m_pSkyMapScaleHNumberPictureBox->Size = System::Drawing::Size( 779, 18 );
			this->m_pSkyMapScaleHNumberPictureBox->TabIndex = 4;
			this->m_pSkyMapScaleHNumberPictureBox->TabStop = false;
			// 
			// m_pSkyMapScaleHGridPictureBox
			// 
			this->m_pSkyMapScaleHGridPictureBox->Location = System::Drawing::Point( 39, 381 );
			this->m_pSkyMapScaleHGridPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleHGridPictureBox->Name = L"m_pSkyMapScaleHGridPictureBox";
			this->m_pSkyMapScaleHGridPictureBox->Size = System::Drawing::Size( 722, 10 );
			this->m_pSkyMapScaleHGridPictureBox->TabIndex = 3;
			this->m_pSkyMapScaleHGridPictureBox->TabStop = false;
			// 
			// panel4
			// 
			this->panel4->BackColor = System::Drawing::Color::White;
			this->panel4->Controls->Add( this->m_pViewingAngleLine );
			this->panel4->Controls->Add( this->m_pSkyMapPictureBox );
			this->panel4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel4->Location = System::Drawing::Point( 39, 19 );
			this->panel4->Margin = System::Windows::Forms::Padding( 0 );
			this->panel4->Name = L"panel4";
			this->panel4->Padding = System::Windows::Forms::Padding( 1 );
			this->panel4->Size = System::Drawing::Size( 722, 362 );
			this->panel4->TabIndex = 2;
			// 
			// m_pViewingAngleLine
			// 
			this->m_pViewingAngleLine->BackColor = System::Drawing::Color::Magenta;
			this->m_pViewingAngleLine->Cursor = System::Windows::Forms::Cursors::HSplit;
			this->m_pViewingAngleLine->Location = System::Drawing::Point( 1, 180 );
			this->m_pViewingAngleLine->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pViewingAngleLine->Name = L"m_pViewingAngleLine";
			this->m_pViewingAngleLine->Size = System::Drawing::Size( 720, 2 );
			this->m_pViewingAngleLine->TabIndex = 1;
			this->m_pViewingAngleLine->MouseDown += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::m_pViewingAngleLine_MouseDown );
			this->m_pViewingAngleLine->MouseMove += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::m_pViewingAngleLine_MouseMove );
			this->m_pViewingAngleLine->MouseUp += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::m_pViewingAngleLine_MouseUp );
			// 
			// m_pSkyMapPictureBox
			// 
			this->m_pSkyMapPictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pSkyMapPictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pSkyMapPictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pSkyMapPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapPictureBox->Name = L"m_pSkyMapPictureBox";
			this->m_pSkyMapPictureBox->Size = System::Drawing::Size( 720, 360 );
			this->m_pSkyMapPictureBox->TabIndex = 0;
			this->m_pSkyMapPictureBox->TabStop = false;
			this->m_pSkyMapPictureBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseDown );
			this->m_pSkyMapPictureBox->MouseMove += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseMove );
			this->m_pSkyMapPictureBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler( this, &MainWindow::RenderingWindow_MouseUp );
			// 
			// splitContainer2
			// 
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer2->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer2->Name = L"splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->AutoScroll = true;
			this->splitContainer2->Panel1->BackColor = System::Drawing::Color::FromArgb( static_cast<System::Int32>( static_cast<System::Byte>( 0 ) ),
				static_cast<System::Int32>( static_cast<System::Byte>( 1 ) ), static_cast<System::Int32>( static_cast<System::Byte>( 14 ) ) );
			this->splitContainer2->Panel1->Controls->Add( this->panel1 );
			this->splitContainer2->Panel1->Padding = System::Windows::Forms::Padding( 30, 19, 0, 20 );
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add( this->splitContainer1 );
			this->splitContainer2->Size = System::Drawing::Size( 1600, 869 );
			this->splitContainer2->SplitterDistance = 610;
			this->splitContainer2->SplitterWidth = 1;
			this->splitContainer2->TabIndex = 2;
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::FromArgb( static_cast<System::Int32>( static_cast<System::Byte>( 0 ) ), static_cast<System::Int32>( static_cast<System::Byte>( 1 ) ),
				static_cast<System::Int32>( static_cast<System::Byte>( 14 ) ) );
			this->panel1->Controls->Add( this->m_pPulseScaleHTitleLabel );
			this->panel1->Controls->Add( this->m_pPulseScaleHNumberPictureBox );
			this->panel1->Controls->Add( this->m_pPulseScaleHGridPictureBox );
			this->panel1->Controls->Add( this->panel2 );
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point( 30, 19 );
			this->panel1->Margin = System::Windows::Forms::Padding( 0 );
			this->panel1->Name = L"panel1";
			this->panel1->Padding = System::Windows::Forms::Padding( 19, 19, 19, 29 );
			this->panel1->Size = System::Drawing::Size( 580, 830 );
			this->panel1->TabIndex = 1;
			// 
			// m_pPulseScaleHNumberPictureBox
			// 
			this->m_pPulseScaleHNumberPictureBox->Location = System::Drawing::Point( 0, 811 );
			this->m_pPulseScaleHNumberPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPulseScaleHNumberPictureBox->Name = L"m_pPulseScaleHNumberPictureBox";
			this->m_pPulseScaleHNumberPictureBox->Size = System::Drawing::Size( 580, 18 );
			this->m_pPulseScaleHNumberPictureBox->TabIndex = 3;
			this->m_pPulseScaleHNumberPictureBox->TabStop = false;
			// 
			// m_pPulseScaleHGridPictureBox
			// 
			this->m_pPulseScaleHGridPictureBox->Location = System::Drawing::Point( 19, 801 );
			this->m_pPulseScaleHGridPictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPulseScaleHGridPictureBox->Name = L"m_pPulseScaleHGridPictureBox";
			this->m_pPulseScaleHGridPictureBox->Size = System::Drawing::Size( 542, 10 );
			this->m_pPulseScaleHGridPictureBox->TabIndex = 2;
			this->m_pPulseScaleHGridPictureBox->TabStop = false;
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::White;
			this->panel2->Controls->Add( this->m_pPulseProfilePictureBox );
			this->panel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel2->Location = System::Drawing::Point( 19, 19 );
			this->panel2->Margin = System::Windows::Forms::Padding( 0 );
			this->panel2->Name = L"panel2";
			this->panel2->Padding = System::Windows::Forms::Padding( 1 );
			this->panel2->Size = System::Drawing::Size( 542, 782 );
			this->panel2->TabIndex = 1;
			// 
			// m_pPulseProfilePictureBox
			// 
			this->m_pPulseProfilePictureBox->BackColor = System::Drawing::Color::Black;
			this->m_pPulseProfilePictureBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_pPulseProfilePictureBox->Location = System::Drawing::Point( 1, 1 );
			this->m_pPulseProfilePictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPulseProfilePictureBox->Name = L"m_pPulseProfilePictureBox";
			this->m_pPulseProfilePictureBox->Size = System::Drawing::Size( 540, 780 );
			this->m_pPulseProfilePictureBox->TabIndex = 0;
			this->m_pPulseProfilePictureBox->TabStop = false;
			// 
			// splitContainer9
			// 
			this->splitContainer9->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer9->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
			this->splitContainer9->IsSplitterFixed = true;
			this->splitContainer9->Location = System::Drawing::Point( 0, 0 );
			this->splitContainer9->Margin = System::Windows::Forms::Padding( 0 );
			this->splitContainer9->Name = L"splitContainer9";
			this->splitContainer9->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer9.Panel1
			// 
			this->splitContainer9->Panel1->BackColor = System::Drawing::Color::White;
			this->splitContainer9->Panel1->Controls->Add( this->pictureBox3 );
			// 
			// splitContainer9.Panel2
			// 
			this->splitContainer9->Panel2->Controls->Add( this->splitContainer2 );
			this->splitContainer9->Size = System::Drawing::Size( 1600, 900 );
			this->splitContainer9->SplitterDistance = 30;
			this->splitContainer9->SplitterWidth = 1;
			this->splitContainer9->TabIndex = 1;
			// 
			// pictureBox3
			// 
			this->pictureBox3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox3->Location = System::Drawing::Point( 0, 0 );
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size( 1600, 30 );
			this->pictureBox3->TabIndex = 0;
			this->pictureBox3->TabStop = false;
			// 
			// m_pPulseScaleHTitleLabel
			// 
			this->m_pPulseScaleHTitleLabel->AutoSize = true;
			this->m_pPulseScaleHTitleLabel->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->m_pPulseScaleHTitleLabel->ForeColor = System::Drawing::Color::White;
			this->m_pPulseScaleHTitleLabel->Location = System::Drawing::Point( 268, 0 );
			this->m_pPulseScaleHTitleLabel->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pPulseScaleHTitleLabel->Name = L"m_pPulseScaleHTitleLabel";
			this->m_pPulseScaleHTitleLabel->Size = System::Drawing::Size( 46, 17 );
			this->m_pPulseScaleHTitleLabel->TabIndex = 4;
			this->m_pPulseScaleHTitleLabel->Text = L"phase";
			// 
			// m_pSkyMapScaleHTitleLabel
			// 
			this->m_pSkyMapScaleHTitleLabel->AutoSize = true;
			this->m_pSkyMapScaleHTitleLabel->Font = ( gcnew System::Drawing::Font( L"Meiryo UI", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 128 ) ) );
			this->m_pSkyMapScaleHTitleLabel->ForeColor = System::Drawing::Color::White;
			this->m_pSkyMapScaleHTitleLabel->Location = System::Drawing::Point( 378, 2 );
			this->m_pSkyMapScaleHTitleLabel->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleHTitleLabel->Name = L"m_pSkyMapScaleHTitleLabel";
			this->m_pSkyMapScaleHTitleLabel->Size = System::Drawing::Size( 46, 17 );
			this->m_pSkyMapScaleHTitleLabel->TabIndex = 7;
			this->m_pSkyMapScaleHTitleLabel->Text = L"phase";
			// 
			// m_pSkyMapScaleVTitlePictureBox
			// 
			this->m_pSkyMapScaleVTitlePictureBox->Location = System::Drawing::Point( 761, 19 );
			this->m_pSkyMapScaleVTitlePictureBox->Margin = System::Windows::Forms::Padding( 0 );
			this->m_pSkyMapScaleVTitlePictureBox->Name = L"m_pSkyMapScaleVTitlePictureBox";
			this->m_pSkyMapScaleVTitlePictureBox->Size = System::Drawing::Size( 18, 362 );
			this->m_pSkyMapScaleVTitlePictureBox->TabIndex = 8;
			this->m_pSkyMapScaleVTitlePictureBox->TabStop = false;
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 12 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb( static_cast<System::Int32>( static_cast<System::Byte>( 0 ) ), static_cast<System::Int32>( static_cast<System::Byte>( 1 ) ),
				static_cast<System::Int32>( static_cast<System::Byte>( 14 ) ) );
			this->ClientSize = System::Drawing::Size( 1600, 900 );
			this->Controls->Add( this->splitContainer9 );
			this->DoubleBuffered = true;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MainWindow";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"MainWindow";
			this->Shown += gcnew System::EventHandler( this, &MainWindow::MainWindow_Shown );
			this->Resize += gcnew System::EventHandler( this, &MainWindow::MainWindow_Resize );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulsarModelPictureBox ) )->EndInit();
			this->splitContainer1->Panel1->ResumeLayout( false );
			this->splitContainer1->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer1 ) )->EndInit();
			this->splitContainer1->ResumeLayout( false );
			this->splitContainer4->Panel1->ResumeLayout( false );
			this->splitContainer4->Panel2->ResumeLayout( false );
			this->splitContainer4->Panel2->PerformLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer4 ) )->EndInit();
			this->splitContainer4->ResumeLayout( false );
			this->panel5->ResumeLayout( false );
			this->splitContainer3->Panel1->ResumeLayout( false );
			this->splitContainer3->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer3 ) )->EndInit();
			this->splitContainer3->ResumeLayout( false );
			this->splitContainer5->Panel1->ResumeLayout( false );
			this->splitContainer5->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer5 ) )->EndInit();
			this->splitContainer5->ResumeLayout( false );
			this->panel6->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapNorthBeginPictureBox ) )->EndInit();
			this->panel7->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapSouthEndPictureBox ) )->EndInit();
			this->splitContainer7->Panel1->ResumeLayout( false );
			this->splitContainer7->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer7 ) )->EndInit();
			this->splitContainer7->ResumeLayout( false );
			this->splitContainer10->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer10 ) )->EndInit();
			this->splitContainer10->ResumeLayout( false );
			this->splitContainer11->Panel1->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer11 ) )->EndInit();
			this->splitContainer11->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapAzimuthPictureBox ) )->EndInit();
			this->splitContainer8->Panel1->ResumeLayout( false );
			this->splitContainer8->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer8 ) )->EndInit();
			this->splitContainer8->ResumeLayout( false );
			this->panel8->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapNorthEndPictureBox ) )->EndInit();
			this->panel9->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPolarCapSouthBeginPictureBox ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox7 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox8 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox6 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox5 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox2 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox1 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pInclinationAngleNumericUpDown ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pViewingAngleNumericUpDown ) )->EndInit();
			this->splitContainer6->Panel1->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer6 ) )->EndInit();
			this->splitContainer6->ResumeLayout( false );
			this->panel3->ResumeLayout( false );
			this->panel3->PerformLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVNumberPictureBox ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVGridPictureBox ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleHNumberPictureBox ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleHGridPictureBox ) )->EndInit();
			this->panel4->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapPictureBox ) )->EndInit();
			this->splitContainer2->Panel1->ResumeLayout( false );
			this->splitContainer2->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer2 ) )->EndInit();
			this->splitContainer2->ResumeLayout( false );
			this->panel1->ResumeLayout( false );
			this->panel1->PerformLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseScaleHNumberPictureBox ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseScaleHGridPictureBox ) )->EndInit();
			this->panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pPulseProfilePictureBox ) )->EndInit();
			this->splitContainer9->Panel1->ResumeLayout( false );
			this->splitContainer9->Panel2->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->splitContainer9 ) )->EndInit();
			this->splitContainer9->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->pictureBox3 ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->m_pSkyMapScaleVTitlePictureBox ) )->EndInit();
			this->ResumeLayout( false );

		}
#pragma endregion


#pragma region UI 設定
	private:
		void SetupUI();
		void DrawScaleGrid( Alignment alignment, int interval, int count, Pen^ p_pen, Graphics^& p_graphics );
		void DrawScaleNumber( array<ScaleNumber^>^ arr_p_number, System::Drawing::Font^ p_font, Brush^ p_brush, Graphics^& p_graphics );
#pragma endregion


#pragma region 描画画面マウス操作
	private:
		/**
		 *	@brief シーン種類の取得
		 *
		 *  @param[in]			p_window  描画するピクチャボックス
		 *  @param[out]			type      シーン種類
		 */
		void GetSceneType( const PictureBox^ p_window, Scene::Type& type );


		/** @brief 描画画面のマウス操作イベント */
		System::Void	RenderingWindow_MouseDown			( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
		System::Void	RenderingWindow_MouseMove			( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
		System::Void	RenderingWindow_MouseUp				( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );


		/** @brief スカイマップ視線方向線のマウス操作イベント */
		System::Void	m_pViewingAngleLine_MouseDown		( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
		System::Void	m_pViewingAngleLine_MouseMove		( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
		System::Void	m_pViewingAngleLine_MouseUp			( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
#pragma endregion


#pragma region 描画更新
	private:
		/**
		 *	@brief シーンの取得
		 *
		 *  @return				シーンのポインタ
		 */
		Scene* GetScene( PictureBox^ p_window );


		/**
		 *	@brief レンダリング更新
		 *
		 *  @param[in]			p_window  描画するピクチャボックス
		 */
		void UpdateRender( PictureBox^ p_window );


		/**
		 *	@brief 画面サイズの設定
		 *
		 *  @param[in]			p_window  描画するピクチャボックス
		 */
		void SetWindowSize( PictureBox^ p_window );

		/** @brief メイン画面の表示更新イベント */
		System::Void	MainWindow_Shown					( System::Object^  sender, System::EventArgs^  e );
		System::Void	MainWindow_Resize					( System::Object^  sender, System::EventArgs^  e );


		/** @brief タイマーの打刻イベント */
		System::Void	m_pRenderingTimer_Tick				( System::Object^  sender, System::EventArgs^  e );


		/** @brief 描画画面の表示更新イベント */
		System::Void	RenderingWindow_Resize				( System::Object^  sender, System::EventArgs^  e );

#pragma endregion

	private:
		// 設定・更新
		void			SetupPulsar();
		void			UpdateInclinationAngle();

		System::Void	NumericUpDown_ValueChanged			( System::Object^  sender, System::EventArgs^  e );

	private:
		// シーン生成
		inline Scene*	GetScene							( int type );
		inline double	GetPolarAngle						( int type );

		void			CreateScene();
		void			CreateCamera();
		void			CreateLight();
		void			CreatePulsarModel();
		void			CreatePolarCap();
		void			CreateSkyMap();
		void			CreatePulse();
};
}
