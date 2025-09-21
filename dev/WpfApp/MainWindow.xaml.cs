using OpenTK.Graphics.OpenGL;
using OpenTK.Mathematics;
using OpenTK.Wpf;
using System;
using System.Diagnostics;
using System.Drawing;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Policy;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;
using static System.Formats.Asn1.AsnWriter;


namespace WpfApp
{
    public partial class MainWindow : Window
    {
        [DllImport("CalcLib.dll")]
        private static extern void SetInclinationAngle([Out] int degree);

        [DllImport("CalcLib.dll")]
        private static extern int GetMagneticLine([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetPolarCapNorthOpened([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetPolarCapNorthClosed([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetPolarCapSouthOpened([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetPolarCapSouthClosed([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetSkyMap([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int GetPulseProfile([Out] float[]? buffer, bool normalize);


        private System.Windows.Point _lastMousePos;
        private bool _isDragging = false;

        private int _inclinationAngle = 0;
        private int _viewingAngle = 0;

        Color4 _vertexColor = Color4.Black;

        private Dictionary<GLWpfControl, float[]> _verticesByControl = new();
        private Dictionary<GLWpfControl, int> _vbosByControl = new();
        private Dictionary<GLWpfControl, int> _vertexCountByControl = new();
        private Dictionary<GLWpfControl, Camera> _cameraByControl = new();


        public MainWindow()
        {
            InitializeComponent();

            // イベント登録
            GLControlMagneticLine.Ready += () => contextReady(GLControlMagneticLine, TimeSpan.Zero);
            GLControlPolarCapNorthOpened.Ready += () => contextReady(GLControlPolarCapNorthOpened, TimeSpan.Zero);
            GLControlPolarCapNorthClosed.Ready += () => contextReady(GLControlPolarCapNorthClosed, TimeSpan.Zero);
            GLControlPolarCapSouthOpened.Ready += () => contextReady(GLControlPolarCapSouthOpened, TimeSpan.Zero);
            GLControlPolarCapSouthClosed.Ready += () => contextReady(GLControlPolarCapSouthClosed, TimeSpan.Zero);
            GLControlSkyMap.Ready += () => contextReady(GLControlSkyMap, TimeSpan.Zero);
            GLControlPulseProfile.Ready += () => contextReady(GLControlPulseProfile, TimeSpan.Zero);

            GLControlMagneticLine.Render += delta => renderView(GLControlMagneticLine, delta);
            GLControlPolarCapNorthOpened.Render += delta => renderView(GLControlPolarCapNorthOpened, delta);
            GLControlPolarCapNorthClosed.Render += delta => renderView(GLControlPolarCapNorthClosed, delta);
            GLControlPolarCapSouthOpened.Render += delta => renderView(GLControlPolarCapSouthOpened, delta);
            GLControlPolarCapSouthClosed.Render += delta => renderView(GLControlPolarCapSouthClosed, delta);
            GLControlSkyMap.Render += delta => renderView(GLControlSkyMap, delta);
            GLControlPulseProfile.Render += delta => renderView(GLControlPulseProfile, delta);

            // GLコントロールの設定
            var settings = new GLWpfControlSettings()
            {
                MajorVersion = 3,
                MinorVersion = 1,
                RenderContinuously = true,
            };

            // 描画開始
            GLControlMagneticLine.Start(settings);
            GLControlPolarCapNorthOpened.Start(settings);
            GLControlPolarCapNorthClosed.Start(settings);
            GLControlPolarCapSouthOpened.Start(settings);
            GLControlPolarCapSouthClosed.Start(settings);
            GLControlSkyMap.Start(settings);
            GLControlPulseProfile.Start(settings);
        }

        private void Slider_Loaded(object sender, RoutedEventArgs e)
        {
            var slider = (Slider)sender;

            if (slider == slider_MagneticLine)
            {
                _inclinationAngle = (int)slider_MagneticLine.Value;
                SetInclinationAngle(_inclinationAngle);
            }
            else if (slider == slider_SkyMap)
            {
                _viewingAngle = (int)slider_SkyMap.Value;
                updateSkyMapGuideLine(slider);
            }
        }


        private void contextReady(GLWpfControl control, TimeSpan delta)
        {
            // OpenGLの初期化
            GL.ClearColor(Color4.Black);

            // カメラの生成
            initCamera(control);

            // 頂点の取得
            getVertices(control);

            // VBOの初期化
            initVBO(control);

            // グラフの目盛り生成
            initGraphScale();
        }


        private void initGraphScale()
        {
            var groupStroke = new GeometryGroup();
            var groupDash = new GeometryGroup();

            for (int i = 0; i <= 36; i++)
            {
                int x = 270 + 15 * i;
                var geo = new LineGeometry(
                        new System.Windows.Point(x, 690),
                        new System.Windows.Point(x, 1350));

                if (i == 0 || i == 36)
                {
                    groupStroke.Children.Add(geo);
                }
                else
                {
                    groupDash.Children.Add(geo);
                }
            }
            PulseProfile_ScaleStroke.Data = groupStroke;
            PulseProfile_ScaleDash.Data = groupDash;
        }


        private void initCamera(GLWpfControl control)
        {
            if (control == GLControlMagneticLine)
            {
                // 磁力線
                _cameraByControl[control] = new Camera(0f, 0f, -5f, Vector3.Zero);
            }
            else if (control == GLControlPolarCapNorthOpened || control == GLControlPolarCapNorthClosed)
            {
                // ポーラーキャップ北
                _cameraByControl[control] = new Camera(33f, 0f, 0.005f, Vector3.Zero);
            }
            else if (control == GLControlPolarCapSouthOpened || control == GLControlPolarCapSouthClosed)
            {
                // ポーラーキャップ南
                _cameraByControl[control] = new Camera(33f, 0f, -0.005f, Vector3.Zero);
            }
            else if (control == GLControlSkyMap)
            {
                // スカイマップ
                _cameraByControl[control] = new Camera(90f, 0f, 10f, Vector3.Zero);
            }
            else if (control == GLControlPulseProfile)
            {
                // パルスプロファイル
                _cameraByControl[control] = new Camera(90f, 0, _viewingAngle + 0.5f, new Vector3(180, 0, 0));
            }
        }


        private void getVertices(GLWpfControl control)
        {
            float[]? vertices = null;

            if (control == GLControlMagneticLine)
            {
                // 磁力線
                vertices = new float[GetMagneticLine(null)];
                GetMagneticLine(vertices);
            }
            else if (control == GLControlPolarCapNorthOpened)
            {
                // ポーラーキャップNOP
                vertices = new float[GetPolarCapNorthOpened(null)];
                GetPolarCapNorthOpened(vertices);
            }
            else if (control == GLControlPolarCapNorthClosed)
            {
                // ポーラーキャップNCL
                vertices = new float[GetPolarCapNorthClosed(null)];
                GetPolarCapNorthClosed(vertices);
            }
            else if (control == GLControlPolarCapSouthOpened)
            {
                // ポーラーキャップSOP
                vertices = new float[GetPolarCapSouthOpened(null)];
                GetPolarCapSouthOpened(vertices);
            }
            else if (control == GLControlPolarCapSouthClosed)
            {
                // ポーラーキャップSCL
                vertices = new float[GetPolarCapSouthClosed(null)];
                GetPolarCapSouthClosed(vertices);
            }
            else if (control == GLControlSkyMap)
            {
                // スカイマップ
                vertices = new float[GetSkyMap(null)];
                GetSkyMap(vertices);
            }
            else if (control == GLControlPulseProfile)
            {
                // パルスプロファイル
                float[] vertices2d = new float[GetPulseProfile(null, false)];
                GetPulseProfile(vertices2d, true);

                int count = (int)(vertices2d.Length * 0.5);
                vertices = new float[count * 3];

                for (int i = 0; i < count; i++)
                {
                    int index3d = i * 3;
                    int index2d = i * 2;

                    vertices[index3d] = vertices2d[index2d];
                    vertices[index3d + 1] = vertices2d[index2d + 1];
                    vertices[index3d + 2] = i / 360;
                }
            }

            if (vertices != null)
            {
                _verticesByControl[control] = vertices;
            }
        }


        private void initVBO(GLWpfControl control)
        {
            if (control != null && _verticesByControl.TryGetValue(control, out var vertices))
            {
                int vbo = GL.GenBuffer();

                // DEBUG
                //Debug.WriteLine($"[{control.Name}] Generated VBO = {vbo}");

                GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);

                // 頂点データをGPUに転送
                GL.BufferData(BufferTarget.ArrayBuffer,
                    (IntPtr)(vertices.Length * sizeof(float)),
                    vertices,
                    BufferUsageHint.StaticDraw);

                _vbosByControl[control] = vbo;
                int size = (control == GLControlSkyMap) ? 2 : 3;
                _vertexCountByControl[control] = vertices.Length / size;

                // DEBUG
                //int arrayBuffer;
                //GL.GetInteger(GetPName.ArrayBufferBinding, out arrayBuffer);
                //Debug.WriteLine($"[{control.Name}] ArrayBuffer bound = {arrayBuffer}");

                GL.BindBuffer(BufferTarget.ArrayBuffer, 0);

                // DEBUG
                //Debug.WriteLine($"initVBO called for {control.Name}, vertices.Length={vertices.Length}");
            }
        }


        private void renderView(GLWpfControl control, TimeSpan delta)
        {
            if (control != null && _verticesByControl.TryGetValue(control, out var vertices))
            {
                if (vertices.All(x => x == 0))
                {
                    // OpenGLの初期化
                    GL.ClearColor(Color4.Black);

                    // 頂点の取得
                    getVertices(control);

                    // VBOの初期化
                    initVBO(control);
                }

                if (_vertexCountByControl.TryGetValue(control, out var count) &&
                    _vbosByControl.TryGetValue(control, out int vbo))
                {
                    GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

                    // カメラの更新
                    updateCamera(control);

                    // DEBUG
                    //int arrayBuffer;
                    //GL.GetInteger(GetPName.ArrayBufferBinding, out arrayBuffer);
                    //Debug.WriteLine($"[{control.Name}] ArrayBuffer bound = {arrayBuffer}");
                    //Debug.WriteLine($"[{control.Name}] vbo = {vbo}");
                    //if (arrayBuffer == 0)
                    //    return;
                    //if (vbo == 0)
                    //    return;

                    GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);

                    // 固定機能パイプライン
                    GL.EnableClientState(ArrayCap.VertexArray);
                    int size = (control == GLControlSkyMap) ? 2 : 3;
                    GL.VertexPointer(size, VertexPointerType.Float, 0, IntPtr.Zero);

                    GL.Color4(_vertexColor);

                    if (control == GLControlPulseProfile)
                    {
                        GL.LineWidth(0.5f);
                        int vertexCount = 360;
                        int lineCount = count / vertexCount;
                        for (int i = 0; i < lineCount; i++)
                        {
                            GL.DrawArrays(PrimitiveType.LineStrip, i * vertexCount, vertexCount);
                        }
                    }
                    else
                    {
                        GL.PointSize(1f);
                        GL.DrawArrays(PrimitiveType.Points, 0, count);
                    }

                    GL.DisableClientState(ArrayCap.VertexArray);
                    GL.BindBuffer(BufferTarget.ArrayBuffer, 0);

                }
            }
        }


        private void updateCamera(GLWpfControl control)
        {
            if (control != null && _cameraByControl.TryGetValue(control, out var camera))
            {
                Matrix4 proj = Matrix4.Identity; // ここで初期化

                if (control == GLControlMagneticLine)
                {
                    proj = Matrix4.CreatePerspectiveFieldOfView(
                        MathHelper.DegreesToRadians(60f),
                        (float)control.ActualWidth / (float)control.ActualHeight,
                        0.001f, 100f
                    );
                }
                else if (
                    control == GLControlPolarCapNorthOpened ||
                    control == GLControlPolarCapNorthClosed ||
                    control == GLControlPolarCapSouthOpened ||
                    control == GLControlPolarCapSouthClosed)
                {
                    proj = Matrix4.CreatePerspectiveFieldOfView(
                        MathHelper.DegreesToRadians(60f),
                        (float)control.ActualWidth / (float)control.ActualHeight,
                        0.0001f, 100f
                    );
                    //proj = Matrix4.CreateOrthographicOffCenter(
                    //    -0.001f, 0.001f,
                    //    -0.001f, 0.001f,
                    //    0.0001f, 100f
                    //);
                }
                else if (control == GLControlSkyMap)
                {
                    proj = Matrix4.CreateOrthographicOffCenter(
                        0f, 360f,
                        0f, 180f,
                        0.001f, 100f
                    );
                }
                else if (control == GLControlPulseProfile)
                {
                    proj = Matrix4.CreateOrthographicOffCenter(
                        -180f, 180f,
                        -20f, 20f,
                        0.001f, 1f
                    );
                }

                GL.MatrixMode(MatrixMode.Projection);
                GL.LoadMatrix(ref proj);

                // ビュー行列
                Matrix4 view = _cameraByControl[control].GetViewMatrix();
                GL.MatrixMode(MatrixMode.Modelview);
                GL.LoadMatrix(ref view);

                if (control == GLControlMagneticLine)
                {
                    float radius = 2;
                    float y = radius * (float)Math.Cos(_inclinationAngle * (Math.PI / 180));
                    float z = radius * (float)Math.Sin(_inclinationAngle * (Math.PI / 180));

                    // 軸の描画
                    GL.Color4(Color4.Red);
                    GL.Begin(PrimitiveType.Lines);
                    GL.Vertex3(0, y, z);
                    GL.Vertex3(0, -y, -z);
                    GL.End();
                }
            }
        }


        private void MagneticLine_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2)
            {
                var control = (GLWpfControl)sender;
                initCamera(control);
            }
        }


        private void MagneticLine_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                _isDragging = true;
                _lastMousePos = e.GetPosition((IInputElement)sender);
            }
        }


        private void MagneticLine_MouseMove(object sender, MouseEventArgs e)
        {
            if (!_isDragging) return;
            var control = (GLWpfControl)sender;

            if (control != null && _cameraByControl.TryGetValue(control, out var camera))
            {
                // マウス移動量を計算
                System.Windows.Point pos = e.GetPosition(control);
                float dx = (float)(pos.X - _lastMousePos.X);
                float dy = (float)(pos.Y - _lastMousePos.Y);

                // 回転角度に反映（0.5f: 感度調整）
                camera.Yaw += dx * 0.5f;    // マウス横移動で水平回転
                camera.Pitch += dy * 0.5f;  // マウス縦移動で垂直回転

                // ピッチ制限（上下90°超え防止）
                camera.Pitch = Math.Clamp(camera.Pitch, -89f, 89f);

                _lastMousePos = pos;
            }
        }


        private void MagneticLine_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _isDragging = false;
        }


        private void GLWpfControl_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            var control = (GLWpfControl)sender;

            if (control != null && _cameraByControl.TryGetValue(control, out var camera))
            {
                //float zoom, min, max = 0;

                if (control == GLControlPulseProfile)
                {
                    zoomCamera(camera, 1.0f, 0.5f, 179.5f, e.Delta);

                    // SkyMap の ViewingAngle を同期させる
                    slider_SkyMap.Value = (int)Math.Round(camera.Distance);
                }
                else if (
                    control == GLControlPolarCapNorthOpened ||
                    control == GLControlPolarCapNorthClosed ||
                    control == GLControlPolarCapSouthOpened ||
                    control == GLControlPolarCapSouthClosed)
                {
                    zoomCamera(camera, -0.0001f, 0.0025f, 0.005f, e.Delta);
                }
            }
        }


        private void zoomCamera(Camera camera, float zoom, float min, float max, float delta)
        {
            // スクロール方向によって距離を増減
            if (0 < delta)
            {
                camera.Distance += zoom; // ズームアウト
            }
            else
            {
                camera.Distance -= zoom; // ズームイン
            }

            // 最小距離の制限（注視点にめり込まないように）
            if (camera.Distance < min)
            {
                camera.Distance = min;
            }
            else if (max < camera.Distance)
            {
                camera.Distance = max;
            }
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var slider = (Slider)sender;

            if (slider == slider_MagneticLine)
            {
                slider.Value = _inclinationAngle = (int)e.NewValue;

                // 操作中は線色を変更する
                _vertexColor = (_vertexColor == Color4.Black) ? Color4.White : Color4.DimGray;
            }
            else if (slider == slider_SkyMap)
            {
                if (slider.Value == 180)
                {
                    slider.Value = 179;
                }
                slider.Value = _viewingAngle = (int)slider.Value;

                if ((_viewingAngle - (int)e.OldValue) != 0)
                {
                    updateSkyMapGuideLine(slider);

                    // PulseProfile の ViewingAngle を同期させる
                    if (GLControlPulseProfile != null && _cameraByControl.TryGetValue(GLControlPulseProfile, out var camera))
                    {
                        camera.Distance = _viewingAngle + 0.5f;
                    }
                }
            }
        }


        private void updateSkyMapGuideLine(Slider slider)
        {
            if (slider == slider_SkyMap)
            {
                double min = slider.Minimum;
                double max = slider.Maximum;
                double height = GuideCanvas.ActualHeight;

                // 値を座標に変換（下が0、上が180）
                double y = height - (_viewingAngle - min) / (max - min) * height;

                GuideLine.Y1 = y;
                GuideLine.Y2 = y;
            }
        }


        private void Thumb_DragCompleted(object sender, DragCompletedEventArgs e)
        {
            var thumb = sender as Thumb;
            var slider = findParentSlider(thumb);

            if (slider != null)
            {
                int degree = (int)slider.Value;

                if (slider == slider_MagneticLine)
                {
                    SetInclinationAngle(_inclinationAngle);
                    updatePolarCapCameraAngle();

                    foreach (var key in _verticesByControl.Keys)
                    {
                        Array.Clear(_verticesByControl[key], 0, _verticesByControl[key].Length);
                    }
                    _vertexColor = Color4.White;
                }
            }
        }


        private void updatePolarCapCameraAngle()
        {
            foreach (var control in _cameraByControl.Keys)
            {
                if (control == GLControlPolarCapNorthOpened ||
                    control == GLControlPolarCapNorthClosed ||
                    control == GLControlPolarCapSouthOpened ||
                    control == GLControlPolarCapSouthClosed)
                {
                    _cameraByControl[control].Yaw = 90 - _inclinationAngle;
                }
            }
        }


        private Slider? findParentSlider(DependencyObject? child)
        {
            while (child != null && !(child is Slider))
            {
                child = VisualTreeHelper.GetParent(child);
            }
            return child as Slider;
        }
    }


    class Camera
    {
        // カメラの球面座標 (角度と距離)
        public float Yaw { get; set; }      // 水平方向回転角
        public float Pitch { get; set; }    // 垂直方向回転角
        public float Distance { get; set; } // 注視点からの距離
        public Vector3 Target { get; set; } // 注視点

        public Camera(float yaw, float pitch, float distance, Vector3 target)
        {
            Yaw = yaw;
            Pitch = pitch;
            Distance = distance;
            Target = target;
        }

        public Matrix4 GetViewMatrix()
        {
            // 球面座標からカメラ位置を算出
            Vector3 cameraPos = new Vector3(
                Target.X + Distance * (float)(Math.Cos(MathHelper.DegreesToRadians(Pitch)) * Math.Cos(MathHelper.DegreesToRadians(Yaw))),
                Target.Y + Distance * (float)(Math.Sin(MathHelper.DegreesToRadians(Pitch))),
                Target.Z + Distance * (float)(Math.Cos(MathHelper.DegreesToRadians(Pitch)) * Math.Sin(MathHelper.DegreesToRadians(Yaw)))
            );

            return Matrix4.LookAt(cameraPos, Target, Vector3.UnitY);
        }
    }
}
