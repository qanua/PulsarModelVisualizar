using System.Windows;

namespace WpfApp
{
    /** @brief SplashWindow.xamlの相互作用ロジック */
    public partial class SplashWindow : Window
    {
        /** @brief SplashWindowのコンストラクタ */
        public SplashWindow()
        {
            InitializeComponent();
        }


        /** @brief スプラッシュ画面の読み込みイベント
         * 
         *  @param[in]      sender      イベント発生画面
         *  @param[in]      e           イベントデータ
         */
        private void SplashWindow_Loaded(object sender, RoutedEventArgs e)
        {
            double baseWidth = 480;
            double baseHeight = 135;

            // 現在の画面サイズ取得
            double screenWidth = SystemParameters.PrimaryScreenWidth;
            double screenHeight = SystemParameters.PrimaryScreenHeight;

            // スケーリング係数の決定
            double scale = Math.Min(screenWidth / 2560, screenHeight / 1440);

            // 新しい画面サイズ
            Width = baseWidth * scale;
            Height = baseHeight * scale;
        }
    }
}
