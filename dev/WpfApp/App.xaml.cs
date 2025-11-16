using System.Windows;

namespace WpfApp
{
    /** @brief App.xamlの相互作用ロジック */
    public partial class App : Application
    {
        /** @brief アプリ起動イベント
         * 
         *  @param[in]      sender      イベント発生オブジェクト
         *  @param[in]      e           イベントデータ
         */
        private void Application_Startup(object sender, StartupEventArgs e)
        {
            // スプラッシュ画面を開く
            var splash = new SplashWindow();
            splash.Show();

            // メイン画面を開く
            var main = new MainWindow();
            main.Show();

            // スプラッシュ画面を閉じる
            splash.Close();
        }
    }
}
