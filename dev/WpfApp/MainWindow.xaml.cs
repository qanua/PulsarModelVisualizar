using OpenTK.Graphics.OpenGL;
using OpenTK.Mathematics;
//using OpenTK.Windowing.Common;
using OpenTK.Wpf;
using System;
using System.Diagnostics;
using System.Drawing;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Media3D;


namespace WpfApp
{
    public partial class MainWindow : Window
    {
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


        private System.Windows.Point lastMousePos;
        private bool isDragging = false;

        private Dictionary<GLWpfControl, float[]> dicVertices = new();
        private Dictionary<GLWpfControl, int> dicVbos = new();
        private Dictionary<GLWpfControl, int> dicVertexCount = new();
        private Dictionary<GLWpfControl, Camera> dicCameras = new();


        public MainWindow()
        {
            InitializeComponent();

            // イベント登録
            glControlMagneticLine.Ready += () => ContextReady(glControlMagneticLine, TimeSpan.Zero);
            glControlPolarCapNorthOpened.Ready += () => ContextReady(glControlPolarCapNorthOpened, TimeSpan.Zero);
            glControlPolarCapNorthClosed.Ready += () => ContextReady(glControlPolarCapNorthClosed, TimeSpan.Zero);
            glControlPolarCapSouthOpened.Ready += () => ContextReady(glControlPolarCapSouthOpened, TimeSpan.Zero);
            glControlPolarCapSouthClosed.Ready += () => ContextReady(glControlPolarCapSouthClosed, TimeSpan.Zero);
            //glControlSkyMap.Ready += () => ContextReady(glControlSkyMap, TimeSpan.Zero);
            //glControlPulseProfile.Ready += () => ContextReady(glControlPulseProfile, TimeSpan.Zero);

            glControlMagneticLine.Render += delta => RenderView(glControlMagneticLine, delta);
            glControlPolarCapNorthOpened.Render += delta => RenderView(glControlPolarCapNorthOpened, delta);
            glControlPolarCapNorthClosed.Render += delta => RenderView(glControlPolarCapNorthClosed, delta);
            glControlPolarCapSouthOpened.Render += delta => RenderView(glControlPolarCapSouthOpened, delta);
            glControlPolarCapSouthClosed.Render += delta => RenderView(glControlPolarCapSouthClosed, delta);
            //glControlSkyMap.Render += delta => DrawLineSegmentglControlSkyMap, delta);
            //glControlPulseProfile.Render += delta => DrawLineSegment(glControlPulseProfile, delta);

            // GLコントロールの設定
            var settings = new GLWpfControlSettings()
            {
                MajorVersion = 3,
                MinorVersion = 1,
                RenderContinuously = true,
            };

            // 描画開始
            glControlMagneticLine.Start(settings);
            glControlPolarCapNorthOpened.Start(settings);
            glControlPolarCapNorthClosed.Start(settings);
            glControlPolarCapSouthOpened.Start(settings);
            glControlPolarCapSouthClosed.Start(settings);
            //glControlSkyMap.Start(settings);
            //glControlPulseProfile.Start(settings);
        }


        private void ContextReady(GLWpfControl control, TimeSpan delta)
        {
            // OpenGLの初期化
            GL.ClearColor(Color4.Black);

            // カメラの生成
            InitCamera(control);

            // 頂点の取得
            StoreVertices(control);

            // VBOの初期化
            InitVBO(control);
        }


        private void InitCamera(GLWpfControl control)
        {
            if (control == glControlMagneticLine)
            {
                // 磁力線
                dicCameras[control] = new Camera(0f, 20f, 5f, Vector3.Zero);
            }
            else if (
                control == glControlPolarCapNorthOpened || control == glControlPolarCapNorthClosed)
            {
                // ポーラーキャップ
                dicCameras[control] = new Camera(33f, 0f, 0.01f, Vector3.Zero);
            }
            else if(
                control == glControlPolarCapSouthOpened || control == glControlPolarCapSouthClosed)
            {
                // ポーラーキャップ
                dicCameras[control] = new Camera(33f, 0f, -0.01f, Vector3.Zero);
            }
            //else if(control == glControlSkyMap)
            //{
            //}
            //else if(control == glControlPulseProfile)
            //{
            //}
        }


        private void StoreVertices(GLWpfControl control)
        {
            float[]? vertices = null;

            if (control == glControlMagneticLine)
            {
                vertices = new float[GetMagneticLine(null)];
                GetMagneticLine(vertices);
            }
            else if (control == glControlPolarCapNorthOpened)
            {
                // ポーラーキャップNOP
                vertices = new float[GetPolarCapNorthOpened(null)];
                GetPolarCapNorthOpened(vertices);
            }
            else if (control == glControlPolarCapNorthClosed)
            {
                // ポーラーキャップNCL
                vertices = new float[GetPolarCapNorthClosed(null)];
                GetPolarCapNorthClosed(vertices);
            }
            else if (control == glControlPolarCapSouthOpened)
            {
                // ポーラーキャップSOP
                vertices = new float[GetPolarCapSouthOpened(null)];
                GetPolarCapSouthOpened(vertices);
            }
            else if (control == glControlPolarCapSouthClosed)
            {
                // ポーラーキャップSCL
                vertices = new float[GetPolarCapSouthClosed(null)];
                GetPolarCapSouthClosed(vertices);
            }
            //else if(control == glControlSkyMap)
            //{
            //}
            //else if(control == glControlPulseProfile)
            //{
            //}

            if (vertices != null)
            {
                dicVertices[control] = vertices;
            }
        }


        private void InitVBO(GLWpfControl control)
        {
            if(dicVertices.TryGetValue(control, out var vertices))
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

                dicVbos[control] = vbo;
                dicVertexCount[control] = vertices.Length / 3;

                // DEBUG
                //int arrayBuffer;
                //GL.GetInteger(GetPName.ArrayBufferBinding, out arrayBuffer);
                //Debug.WriteLine($"[{control.Name}] ArrayBuffer bound = {arrayBuffer}");

                GL.BindBuffer(BufferTarget.ArrayBuffer, 0);

                // DEBUG
                //Debug.WriteLine($"InitVBO called for {control.Name}, vertices.Length={vertices.Length}");
            }
        }


        private void RenderView(GLWpfControl control, TimeSpan delta)
        {
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            if (dicVertexCount.TryGetValue(control, out var count) && dicVbos.TryGetValue(control, out int vbo))
            {
                // カメラの更新
                UpdateCamera(control);

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
                GL.VertexPointer(3, VertexPointerType.Float, 0, IntPtr.Zero);

                // 頂点の描画
                GL.Color4(Color4.White);
                GL.PointSize(1f);
                GL.DrawArrays(PrimitiveType.Points, 0, count);   // PrimitiveType.LineStrip

                GL.DisableClientState(ArrayCap.VertexArray);
                GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
            }
        }


        private void UpdateCamera(GLWpfControl control)
        {
            if (dicCameras.ContainsKey(control))
            {
                Matrix4 proj;

                if (control == glControlMagneticLine)
                {
                    // 投影行列
                    proj = Matrix4.CreatePerspectiveFieldOfView(
                        MathHelper.DegreesToRadians(60f),
                        (float)control.ActualWidth / (float)control.ActualHeight,
                        0.001f, 100f
                    );
                }
                else
                {
                    float aspect = (float)control.ActualWidth / (float)control.ActualHeight;
                    float size = 0.001f; // 基準スケール

                    float left, right, bottom, top;
                    if (aspect >= 1.0f)
                    {
                        // 横長画面 → 横に合わせる
                        left = -size * aspect;
                        right = size * aspect;
                        bottom = -size;
                        top = size;
                    }
                    else
                    {
                        // 縦長画面 → 縦に合わせる
                        left = -size;
                        right = size;
                        bottom = -size / aspect;
                        top = size / aspect;
                    }

                    proj = Matrix4.CreateOrthographicOffCenter(
                        left, right,
                        bottom, top,
                        0.001f, 100f
                    );
                }

                GL.MatrixMode(MatrixMode.Projection);
                GL.LoadMatrix(ref proj);

                // ビュー行列
                Matrix4 view = dicCameras[control].GetViewMatrix();
                GL.MatrixMode(MatrixMode.Modelview);
                GL.LoadMatrix(ref view);
            }
        }


        private void GLControl_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                isDragging = true;
                lastMousePos = e.GetPosition((IInputElement)sender);
            }
        }


        private void GLControl_MouseMove(object sender, MouseEventArgs e)
        {
            if (!isDragging) return;
            var control = (GLWpfControl)sender;

            if (dicCameras.ContainsKey(control))
            {
                Camera camera = dicCameras[control];

                // マウス移動量を計算
                System.Windows.Point pos = e.GetPosition(control);
                float dx = (float)(pos.X - lastMousePos.X);
                float dy = (float)(pos.Y - lastMousePos.Y);

                // 回転角度に反映（0.5f: 感度調整）
                camera.Yaw += dx * 0.5f;  // マウス横移動で水平回転
                camera.Pitch += dy * 0.5f;  // マウス縦移動で垂直回転

                // ピッチ制限（上下90°超え防止）
                camera.Pitch = Math.Clamp(camera.Pitch, -89f, 89f);

                lastMousePos = pos;
            }
        }


        private void GLControl_MouseUp(object sender, MouseButtonEventArgs e)
        {
            isDragging = false;
        }


        private void GLControl_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            var control = (GLWpfControl)sender;
            if (dicCameras.ContainsKey(control))
            {
                Camera camera = dicCameras[control];

                // スクロール方向によって距離を増減
                if (e.Delta > 0)
                    camera.Distance -= 0.5f; // ズームイン
                else
                    camera.Distance += 0.5f; // ズームアウト

                // 最小距離の制限（注視点にめり込まないように）
                //if (camera.Distance < 1.0f) camera.Distance = 1.0f;
            }
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
