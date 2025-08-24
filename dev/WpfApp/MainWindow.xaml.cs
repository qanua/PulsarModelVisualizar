using OpenTK.Graphics.OpenGL;
using OpenTK.Mathematics;
using OpenTK.Wpf;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;


namespace WpfApp
{
    public partial class MainWindow : Window
    {
        [DllImport("CalcLib.dll")]
        private static extern int GetVertices([Out] float[] buffer, int maxCount);

        private float[] vertices = new float[9];

        // カメラの球面座標 (角度と距離)
        private float yaw = 0f;         // 水平方向回転角
        private float pitch = 20f;      // 垂直方向回転角
        private float distance = 5f;    // 注視点からの距離
        private Point lastMousePos;


        public MainWindow()
        {
            InitializeComponent();

            // GLコントロールの設定
            var settings = new GLWpfControlSettings()
            {
                MajorVersion = 3,
                MinorVersion = 1,
                RenderContinuously = true,
            };

            // DLL から三角形の頂点を取得
            GetVertices(vertices, vertices.Length);

            // 描画開始
            glControl.Start(settings);
        }


        private void GLControl_Loaded(object sender, RoutedEventArgs e)
        {
            // OpenGLの初期化
            GL.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            GL.Enable(EnableCap.DepthTest);
        }


        private void GLControl_Ready()
        {
            ////カリングの設定
            //GL.Enable(EnableCap.CullFace); //カリングの有効化
            //GL.FrontFace(FrontFaceDirection.Ccw); //反時計回りで描かれる面を表面に指定
            ////ライトの設定
            //GL.Enable(EnableCap.Lighting); //ライティングの有効化
            //GL.Enable(EnableCap.Light0); //ライトを1つ（０番）を有効化
            //GL.Light(LightName.Light0, LightParameter.Position, Vector4.UnitZ); //UnitZ(0,0,1)にライトを配置
            ////法線の正規化
            //GL.Enable(EnableCap.Normalize);
            ////物体の質感の設定
            //GL.Enable(EnableCap.ColorMaterial); //質感の有効化

        }


        private void GLControl_Render(TimeSpan obj)
        {
            // バッファの初期化
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            // 投影行列
            Matrix4 proj = Matrix4.CreatePerspectiveFieldOfView(
                MathHelper.DegreesToRadians(60f),                               // 視野角（FOV: Field of View）
                (float)glControl.ActualWidth / (float)glControl.ActualHeight,   // アスペクト比
                0.1f,   // 近クリップ面
                100f    // 遠クリップ面
            );

            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadMatrix(ref proj);

            // カメラの位置を球面座標から計算
            Vector3 target = Vector3.Zero; // 注視点（三角形の中心）
            Vector3 cameraPos = new Vector3(
                target.X + distance * (float)(System.Math.Cos(MathHelper.DegreesToRadians(pitch)) * System.Math.Cos(MathHelper.DegreesToRadians(yaw))),
                target.Y + distance * (float)(System.Math.Sin(MathHelper.DegreesToRadians(pitch))),
                target.Z + distance * (float)(System.Math.Cos(MathHelper.DegreesToRadians(pitch)) * System.Math.Sin(MathHelper.DegreesToRadians(yaw)))
            );

            Matrix4 view = Matrix4.LookAt(cameraPos, target, Vector3.UnitY);
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadMatrix(ref view);

            // 三角形を描画
            DrawLineSegment();
        }


        private void DrawLineSegment()
        {
            // 三角形を描画
            GL.Begin(PrimitiveType.Triangles);
            GL.Color4(Color4.Red);
            for (int i = 0; i < vertices.Length; i += 3)
            {
                GL.Vertex3(vertices[i], vertices[i + 1], vertices[i + 2]);
            }
            GL.End();
        }


        private void GLControl_MouseDown(object sender, MouseButtonEventArgs e)
        {
            lastMousePos = e.GetPosition(glControl);
        }


        private void GLControl_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                // マウス移動量を計算
                Point currentPos = e.GetPosition(glControl);
                float dx = (float)(currentPos.X - lastMousePos.X);
                float dy = (float)(currentPos.Y - lastMousePos.Y);

                // 回転角度に反映（0.5f: 感度調整）
                yaw += dx * 0.5f;
                pitch -= dy * 0.5f;

                // ピッチを制限（上下90度）
                pitch = MathHelper.Clamp(pitch, -89f, 89f);

                lastMousePos = currentPos;
            }
        }
    }
}
