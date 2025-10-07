using OpenTK.Graphics.OpenGL;
using OpenTK.Mathematics;
using OpenTK.Wpf;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;


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


        // Model
        private Color4 _vertexColor = Color4.White;
        private int _inclinationAngle = 0;
        private int _viewingAngle = 90;
        private int _phase = 90;
        private Dictionary<GLWpfControl, Object> _objsByControl = new();

        // GUI
        private Button? _selectedButton;
        private Dictionary<Button, GLWpfControl> _polarCapViewByButton = new();
        private Point _lastMousePos;
        private bool _isDragging = false;

        // Shader
        private int _shader, _modelLoc, _viewLoc, _projLoc, _colorLoc;

        // Color
        private Color4 VERTEX_COLOR = new Color4(0.3f, 0.3f, 0.3f, 1f);
        private Color4 RED_COLOR = new Color4(1f, 75 / 255f, 50 / 255f , 1f);
        private Color4 GREEN_COLOR = new Color4(0f, 185 / 255f, 75 / 255f, 1f);

        private Brush RED_BRUSH = new SolidColorBrush(Color.FromArgb(255, 225, 75, 50));
        private Brush BLUE_BRUSH = new SolidColorBrush(Color.FromArgb(255, 60, 90, 255));
        private Brush GREEN_BRUSH = new SolidColorBrush(Color.FromArgb(255, 0, 185, 75));


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

            GLControlMagneticLine.Loaded += (s, e) => layoutLoaded(GLControlMagneticLine);
            GLControlPolarCapNorthOpened.Loaded += (s, e) => layoutLoaded(GLControlPolarCapNorthOpened);
            GLControlPolarCapNorthClosed.Loaded += (s, e) => layoutLoaded(GLControlPolarCapNorthClosed);
            GLControlPolarCapSouthOpened.Loaded += (s, e) => layoutLoaded(GLControlPolarCapSouthOpened);
            GLControlPolarCapSouthClosed.Loaded += (s, e) => layoutLoaded(GLControlPolarCapSouthClosed);
            GLControlSkyMap.Loaded += (s, e) => layoutLoaded(GLControlSkyMap);
            GLControlPulseProfile.Loaded += (s, e) => layoutLoaded(GLControlPulseProfile);

            GLControlMagneticLine.Render += delta => renderView(GLControlMagneticLine, delta);
            GLControlPolarCapNorthOpened.Render += delta => renderView(GLControlPolarCapNorthOpened, delta);
            GLControlPolarCapNorthClosed.Render += delta => renderView(GLControlPolarCapNorthClosed, delta);
            GLControlPolarCapSouthOpened.Render += delta => renderView(GLControlPolarCapSouthOpened, delta);
            GLControlPolarCapSouthClosed.Render += delta => renderView(GLControlPolarCapSouthClosed, delta);
            GLControlSkyMap.Render += delta => renderView(GLControlSkyMap, delta);
            GLControlPulseProfile.Render += delta => renderView(GLControlPulseProfile, delta);

            // GL コントロールの設定
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

            if (slider == MagneticLineSlider)
            {
                _inclinationAngle = (int)MagneticLineSlider.Value;
                SetInclinationAngle(_inclinationAngle);
            }
            else if (slider == SkyMapViewingAngleSlider)
            {
                _viewingAngle = (int)SkyMapViewingAngleSlider.Value;
                updateSkyMapGuideLine(slider);
            }
            else if (slider == SkyMapPhaseSlider)
            {
                _phase = (int)SkyMapPhaseSlider.Value;
                updateSkyMapGuideLine(slider);
            }
        }


        private void contextReady(GLWpfControl control, TimeSpan delta)
        {
            // OpenGL の設定
            GL.ClearColor(Color4.White);
            GL.Enable(EnableCap.DepthTest);

            // カメラの生成
            setupCamera(control);

            // シェーダーの生成
            setupShader();

            // 頂点の取得
            getVertices(control);

            // VAO の生成
            setupVAO(control);
            if (control == GLControlMagneticLine)
            {
                setupAxisVAO(control);
            }

            // スカイマップの GUI 設定
            if (control == GLControlSkyMap)
            {
                setupSkyMapView();
                ViewingAngleGuide.Stroke = BLUE_BRUSH;
                PhaseGuide.Stroke = GREEN_BRUSH;
            }

            // ポーラーキャップの GUI 設定
            setupPolarCapView(control);
        }


        private void setupCamera(GLWpfControl control)
        {
            Camera? camera = null;
            if (control == GLControlMagneticLine)
            {
                // 磁力線
                camera = new Camera(0f, 0f, -5f, Vector3.Zero);
            }
            else if (control == GLControlPolarCapNorthOpened || control == GLControlPolarCapNorthClosed)
            {
                // ポーラーキャップ北
                camera = new Camera(33f, 0f, 0.0027f, Vector3.Zero);
            }
            else if (control == GLControlPolarCapSouthOpened || control == GLControlPolarCapSouthClosed)
            {
                // ポーラーキャップ南
                camera = new Camera(33f, 0f, -0.0027f, Vector3.Zero);
            }
            else if (control == GLControlSkyMap)
            {
                // スカイマップ
                camera = new Camera(90f, 0f, 10f, Vector3.Zero);
            }
            else if (control == GLControlPulseProfile)
            {
                // パルスプロファイル
                camera = new Camera(90f, 0, _viewingAngle + 0.5f, new Vector3(180, 0, 0));
            }

            // カメラの格納
            if (camera != null)
            {
                if (_objsByControl.ContainsKey(control))
                {
                    _objsByControl[control].Camera = camera;
                }
                else
                {
                    _objsByControl[control] = new Object(camera);
                }
            }
        }


        private void setupShader()
        {
            // 頂点シェーダー
            string vertexShaderSource = @"
            #version 330 core
            layout(location = 0) in vec3 aPosition;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            void main()
            {
                gl_Position = projection * view * model * vec4(aPosition, 1.0);
            }
            ";

            // フラグメントシェーダー
            string fragmentShaderSource = @"
            #version 330 core
            out vec4 FragColor;
            uniform vec4 color;

            void main()
            {
                FragColor = color;
            }
            ";

            // シェーダーのコンパイル
            int vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vertexShader, vertexShaderSource);
            GL.CompileShader(vertexShader);

            int fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fragmentShader, fragmentShaderSource);
            GL.CompileShader(fragmentShader);

            // シェーダーをリンク
            _shader = GL.CreateProgram();
            GL.AttachShader(_shader, vertexShader);
            GL.AttachShader(_shader, fragmentShader);
            GL.LinkProgram(_shader);

            // ソースの削除
            GL.DeleteShader(vertexShader);
            GL.DeleteShader(fragmentShader);

            // シェーダー location の格納
            _modelLoc = GL.GetUniformLocation(_shader, "model");
            _viewLoc = GL.GetUniformLocation(_shader, "view");
            _projLoc = GL.GetUniformLocation(_shader, "projection");
            _colorLoc = GL.GetUniformLocation(_shader, "color");
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

                // 二次元から三次元に展開する
                for (int i = 0; i < count; i++)
                {
                    int index3d = i * 3;
                    int index2d = i * 2;

                    vertices[index3d] = vertices2d[index2d];
                    vertices[index3d + 1] = vertices2d[index2d + 1];
                    vertices[index3d + 2] = i / 360;
                }
            }

            // 磁力線の頂点を格納
            if (vertices != null)
            {
                _objsByControl[control].PulsarVertices = vertices;
            }
        }


        private void setupVAO(GLWpfControl control)
        {
            Object obj = _objsByControl[control];

            if (obj.TryGetValue(obj.PulsarVertices, out var vertices) && vertices != null)
            {
                int vao = GL.GenVertexArray();
                int vbo = GL.GenBuffer();

                GL.BindVertexArray(vao);

                // 頂点バッファ
                GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
                GL.BufferData(
                    BufferTarget.ArrayBuffer,                   // バッファの種類: VBO
                    (IntPtr)(vertices.Length * sizeof(float)),  // バッファのサイズ
                    vertices,                                   // 転送データ
                    BufferUsageHint.StaticDraw                  // データの使い方: 毎フレーム更新
                );

                // 頂点 attribute
                int size = (control == GLControlSkyMap) ? 2 : 3;
                GL.EnableVertexAttribArray(0);
                GL.VertexAttribPointer(
                    0,                              // シェーダーの入力変数位置: location = 0
                    size,                           // 1頂点あたりの要素数
                    VertexAttribPointerType.Float,  // データ型: float
                    false,                          // 正規化: しない
                    size * sizeof(float),           // 1頂点あたりのバイト数
                    0                               // データ開始位置のオフセット: 0
                );

                // VAO の格納
                obj.PulsarVao = vao;
                obj.PulsarVerticesCount = vertices.Length / size;

                GL.BindVertexArray(0);
            }
        }


        private void setupAxisVAO(GLWpfControl control)
        {
            Object obj = _objsByControl[control];

            // 軸の頂点
            float[] vertices =
            {
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f,  0.5f
            };

            int vao = GL.GenVertexArray();
            int vbo = GL.GenBuffer();

            GL.BindVertexArray(vao);

            // 頂点バッファ
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            GL.BufferData(
                BufferTarget.ArrayBuffer,
                vertices.Length * sizeof(float),
                vertices, BufferUsageHint.StaticDraw
            );

            // 頂点 attribute
            GL.EnableVertexAttribArray(0);
            GL.VertexAttribPointer(
                0,
                3,
                VertexAttribPointerType.Float,
                false,
                3 * sizeof(float),
                0
            );

            // VAO の格納
            obj.PrimitiveVao = vao;
            obj.PrimitiveVerticesCount = vertices.Length / 3;

            GL.BindVertexArray(0);
        }


        private void setupSkyMapView()
        {
            var groupStroke = new GeometryGroup();
            var groupDash = new GeometryGroup();

            for (int i = 0; i <= 36; i++)
            {
                int x = 270 + 15 * i;
                var geo = new LineGeometry(
                    new Point(x, 690),
                    new Point(x, 1350));

                if (i == 0 || i == 36)
                {
                    // 外枠（実線）
                    groupStroke.Children.Add(geo);
                }
                else
                {
                    // 罫線（破線）
                    groupDash.Children.Add(geo);
                }
            }

            PathStrokePulseProfile.Data = groupStroke;
            PathDashPulseProfile.Data = groupDash;
        }


        private void setupPolarCapView(GLWpfControl control)
        {
            // ボタンの色を変更（初回のみ）
            if (_polarCapViewByButton.Count == 0)
            {
                _polarCapViewByButton[PolarCapSouthClosedButton] = GLControlPolarCapSouthClosed;
                _polarCapViewByButton[PolarCapSouthOpenedButton] = GLControlPolarCapSouthOpened;
                _polarCapViewByButton[PolarCapNorthClosedButton] = GLControlPolarCapNorthClosed;
                _polarCapViewByButton[PolarCapNorthOpenedButton] = GLControlPolarCapNorthOpened;

                var buttons = _polarCapViewByButton.Keys.ToList();

                foreach (var button in buttons)
                {
                    if (button == PolarCapNorthOpenedButton)
                    {
                        button.Background = RED_BRUSH;
                        _selectedButton = PolarCapNorthOpenedButton;
                    }
                    else
                    {
                        button.Background = Brushes.Gray;
                    }
                }
            }

            // コントロールの可視性を変更
            if (control == GLControlPolarCapNorthOpened)
            {
                control.Visibility = Visibility.Visible;
            }
            else if (
                control == GLControlPolarCapNorthClosed ||
                control == GLControlPolarCapSouthOpened ||
                control == GLControlPolarCapSouthClosed)
            {
                control.Visibility = Visibility.Hidden;
            }
        }


        private void layoutLoaded(GLWpfControl control)
        {
            // レイアウト処理後に実行（ActualWidth、ActualHeight 取得のため）
            Dispatcher.BeginInvoke(new Action(() =>
            {
                // プロジェクションの設定
                setupProjection(control);

            }), System.Windows.Threading.DispatcherPriority.Loaded);
        }


        private void setupProjection(GLWpfControl control)
        {
            Matrix4 proj = Matrix4.Identity;

            if (control == GLControlSkyMap)
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
                    -23f, 17f,
                    0.001f, 1f
                );
            }
            else
            {
                float fovy = MathHelper.DegreesToRadians(60f);
                float aspect = (float)control.ActualWidth / (float)control.ActualHeight;

                if (control == GLControlMagneticLine)
                {
                    proj = Matrix4.CreatePerspectiveFieldOfView(
                        fovy,   // 視野角
                        aspect, // アスペクト比
                        0.001f, // near
                        100f    // far
                    );
                }
                else if (
                    control == GLControlPolarCapNorthOpened ||
                    control == GLControlPolarCapNorthClosed ||
                    control == GLControlPolarCapSouthOpened ||
                    control == GLControlPolarCapSouthClosed)
                {
                    proj = Matrix4.CreatePerspectiveFieldOfView(
                        fovy,
                        aspect,
                        0.0001f,
                        100f
                    );
                }
            }

            // プロジェクションの格納
            _objsByControl[control].Projection = proj;
        }


        private void renderView(GLWpfControl control, TimeSpan delta)
        {
            // バッファの消去
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            // 頂点の更新
            if (_objsByControl[control].TryGetValue(_objsByControl[control].PulsarVertices, out var vertices) &&
                vertices != null && vertices.All(x => x == 0))
            {
                // 頂点の取得
                getVertices(control);

                // VAO の更新
                setupVAO(control);

                // 磁力線のスライダーを有効化
                var thumb = (MagneticLineSlider.Template.FindName("PART_Thumb", MagneticLineSlider) as Thumb);
                if (thumb != null)
                {
                    thumb.IsHitTestVisible = true;
                }
            }

            drawObjects(control);
        }


        private void drawObjects(GLWpfControl control)
        {
            Object obj = _objsByControl[control];

            if (obj.TryGetValue(obj.Camera, out var camera) &&
                obj.TryGetValue(obj.Projection, out var proj))
            {
                // シェーダーのバインド
                GL.UseProgram(_shader);

                Matrix4 view = camera.GetViewMatrix();

                GL.UniformMatrix4(_viewLoc, false, ref view);
                GL.UniformMatrix4(_projLoc, false, ref proj);

                // 頂点の描画
                if (obj.TryGetValue(obj.PulsarVao, out var pulvao) &&
                    obj.TryGetValue(obj.PulsarVerticesCount, out var pulvcount))
                {
                    Matrix4 model = Matrix4.Identity;

                    GL.UniformMatrix4(_modelLoc, false, ref model);
                    GL.Uniform4(_colorLoc, _vertexColor);

                    drawVertices(control, pulvao, pulvcount);
                }

                // プリミティブの描画
                if (obj.TryGetValue(obj.PrimitiveVao, out var privao) &&
                    obj.TryGetValue(obj.PrimitiveVerticesCount, out var privcount))
                {
                    // 回転軸
                    Matrix4 model = Matrix4.CreateScale(0.02f, 4.0f, 0.02f);
                    Color4 color = GREEN_COLOR;

                    GL.UniformMatrix4(_modelLoc, false, ref model);
                    GL.Uniform4(_colorLoc, color);

                    GL.LineWidth(3.0f);
                    GL.BindVertexArray(privao);
                    GL.DrawArrays(PrimitiveType.Lines, 0, privcount);

                    // 磁化軸
                    model *= Matrix4.CreateRotationX(MathHelper.DegreesToRadians(_inclinationAngle));
                    color = RED_COLOR;

                    GL.UniformMatrix4(_modelLoc, false, ref model);
                    GL.Uniform4(_colorLoc, color);

                    GL.DrawArrays(PrimitiveType.Lines, 0, privcount);
                }
            }
        }


        private void drawVertices(GLWpfControl control, int vao, int count)
        {
            GL.BindVertexArray(vao);

            if (control == GLControlPulseProfile)
            {
                // 線の描画
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
                // 点の描画
                if (control == GLControlPolarCapNorthOpened ||
                    control == GLControlPolarCapNorthClosed ||
                    control == GLControlPolarCapSouthOpened ||
                    control == GLControlPolarCapSouthClosed)
                {
                    GL.PointSize(2f);
                }
                else
                {
                    GL.PointSize(1f);
                }

                GL.DrawArrays(PrimitiveType.Points, 0, count);
            }
            GL.BindVertexArray(0);
        }


        private void MagneticLine_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2)
            {
                var control = (GLWpfControl)sender;
                setupCamera(control);

                // ViewingAngle スライダーの同期
                SkyMapViewingAngleSlider.Value = 90;

                // Phase スライダーの同期
                SkyMapPhaseSlider.Value = 90;
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
            if (_isDragging)
            {
                var control = (GLWpfControl)sender;

                if (_objsByControl[control].TryGetValue(_objsByControl[control].Camera, out var camera))
                {
                    // マウス移動量を計算
                    Point pos = e.GetPosition(control);
                    float dx = (float)(pos.X - _lastMousePos.X);
                    float dy = (float)(pos.Y - _lastMousePos.Y);

                    // 回転角度に反映（感度: 0.5f）
                    camera.Yaw += dx * 0.5f;    // マウス横移動で水平回転
                    camera.Pitch += dy * 0.5f;  // マウス縦移動で垂直回転

                    // ピッチ制限（上下90°未満）
                    camera.Pitch = Math.Clamp(camera.Pitch, -89.9f, 89.9f);

                    // ViewingAngle スライダーの同期
                    _viewingAngle = 90 + (int)camera.Pitch;
                    SkyMapViewingAngleSlider.Value = _viewingAngle;

                    // Phase スライダーの同期
                    int delta = (90 + (int)camera.Yaw) % 360;
                    _phase = (0 <= delta) ? delta : 360 + delta;
                    SkyMapPhaseSlider.Value = _phase;

                    _lastMousePos = pos;
                }
            }
        }


        private void MagneticLine_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _isDragging = false;
        }


        private void MagneticLine_MouseLeave(object sender, MouseEventArgs e)
        {
            _isDragging = false;
        }


        private void Control_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            var control = (GLWpfControl)sender;

            if (control == GLControlPulseProfile)
            {
                if (_objsByControl[control].TryGetValue(_objsByControl[control].Camera, out var camera))
                {
                    zoomCamera(camera, 1.0f, 0.5f, 179.5f, -e.Delta);

                    // ViewingAngle スライダーの同期
                    SkyMapViewingAngleSlider.Value = (int)camera.Distance;
                }
            }
            else if (
                control == GLControlPolarCapNorthOpened ||
                control == GLControlPolarCapNorthClosed ||
                control == GLControlPolarCapSouthOpened ||
                control == GLControlPolarCapSouthClosed)
            {
                var views = _polarCapViewByButton.Values;

                foreach (var view in views)
                {
                    if (_objsByControl[control].TryGetValue(_objsByControl[view].Camera, out var camera))
                    {
                        if (view == GLControlPolarCapNorthOpened || view == GLControlPolarCapNorthClosed)
                        {
                            zoomCamera(camera, -0.0001f, 0.0025f, 0.005f, e.Delta);
                        }
                        else if (view == GLControlPolarCapSouthOpened || view == GLControlPolarCapSouthClosed)
                        {
                            zoomCamera(camera, 0.0001f, -0.0025f, -0.005f, e.Delta);
                        }
                    }
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

            // 最小・最大距離の制限（フレームアウト防止）
            if (camera.Distance * min < 0)
            {
                camera.Distance = min;
            }
            else if (Math.Abs(camera.Distance) < Math.Abs(min))
            {
                camera.Distance = min;
            }
            else if (Math.Abs(max) < Math.Abs(camera.Distance))
            {
                camera.Distance = max;
            }
        }


        private void ArrowSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var slider = (Slider)sender;

            if (slider == MagneticLineSlider)
            {
                slider.Value = _inclinationAngle = (int)e.NewValue;

                // スライダー操作中の色変更
                _vertexColor = (_vertexColor == Color4.White) ? VERTEX_COLOR : Color4.LightGray;
            }
            else if (slider == SkyMapViewingAngleSlider)
            {
                if (_viewingAngle != e.NewValue)
                {
                    int value = (int)e.NewValue;
                    slider.Value = _viewingAngle = (value == 180) ? 179 : value;

                    // パルスプロファイルの表示を同期
                    Object obj = _objsByControl[GLControlPulseProfile];

                    if (obj.TryGetValue(obj.Camera, out var pulcam))
                    {
                        pulcam.Distance = _viewingAngle + 0.5f;
                    }

                    // 磁力線の表示を同期
                    obj = _objsByControl[GLControlMagneticLine];

                    if (obj.TryGetValue(obj.Camera, out var magcam))
                    {
                        magcam.Pitch = Math.Clamp(_viewingAngle - 90f, -89.9f, 89.9f);
                    }
                }
                updateSkyMapGuideLine(slider);
            }
            else if (slider == SkyMapPhaseSlider)
            {
                if (_phase != e.NewValue)
                {
                    slider.Value = _phase = (int)e.NewValue;

                    // 磁力線の Phase を同期
                    Object obj = _objsByControl[GLControlMagneticLine];

                    if (obj.TryGetValue(obj.Camera, out var camera))
                    {
                        camera.Yaw = _phase - 90f;
                    }
                }
                updateSkyMapGuideLine(slider);
            }
        }


        private void updateSkyMapGuideLine(Slider slider)
        {
            double min = slider.Minimum;
            double max = slider.Maximum;

            if (slider == SkyMapViewingAngleSlider)
            {
                double height = SkyMapViewingAngleCanvas.ActualHeight;

                // 値を座標に変換（上0、下180）
                double y = (_viewingAngle - min) / (max - min) * height;

                ViewingAngleGuide.Y1 = y;
                ViewingAngleGuide.Y2 = y;
            }
            else if(slider == SkyMapPhaseSlider)
            {
                double width = SkyMapPhaseCanvas.ActualWidth;

                // 値を座標に変換（左0、右360）
                double x = (_phase - min) / (max - min) * width;

                PhaseGuide.X1 = x;
                PhaseGuide.X2 = x;
            }
        }


        private void Thumb_DragCompleted(object sender, DragCompletedEventArgs e)
        {
            var thumb = sender as Thumb;
            var slider = findParentSlider(thumb);

            if (slider != null && thumb != null)
            {
                int degree = (int)slider.Value;

                if (slider == MagneticLineSlider)
                {
                    // 磁力線のスライダーを無効化
                    thumb.IsHitTestVisible = false;

                    SetInclinationAngle(_inclinationAngle);

                    // ポーラーキャップのカメラ位置を変更
                    foreach (var control in _objsByControl.Keys)
                    {
                        if (control == GLControlPolarCapNorthOpened ||
                            control == GLControlPolarCapNorthClosed ||
                            control == GLControlPolarCapSouthOpened ||
                            control == GLControlPolarCapSouthClosed)
                        {
                            _objsByControl[control].Camera.Yaw = 90 - _inclinationAngle;
                        }
                    }

                    // 頂点を全削除（更新のため）
                    foreach (var control in _objsByControl.Keys)
                    {
                        var vertices = _objsByControl[control].PulsarVertices;
                        if (vertices != null)
                        {
                            Array.Clear(vertices, 0, vertices.Length);
                        }
                    }
                    _vertexColor = VERTEX_COLOR;
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


        private void PolarCapArrowButton_Click(object sender, RoutedEventArgs e)
        {
            if (_selectedButton != null)
            {
                _selectedButton.Background = Brushes.Gray;
                _polarCapViewByButton[_selectedButton].Visibility = Visibility.Hidden;
            }

            var clicked = (Button)sender;
            clicked.Background = RED_BRUSH;
            _polarCapViewByButton[clicked].Visibility = Visibility.Visible;

            _selectedButton = clicked;
        }


        private void PolarCapArrowButton_MouseEnter(object sender, RoutedEventArgs e)
        {
            var overed = (Button)sender;

            if (overed.Background == Brushes.Gray)
            {
                overed.Background = Brushes.LightGray;
            }
        }


        private void PolarCapArrowButton_MouseLeave(object sender, RoutedEventArgs e)
        {
            var overed = (Button)sender;

            if (overed.Background == Brushes.LightGray)
            {
                overed.Background = Brushes.Gray;
            }
        }
    }


    class Camera
    {
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


    class Object
    {
        public float[]? PulsarVertices { get; set; }    // パルサーの頂点
        public int PulsarVerticesCount { get; set; }    // パルサーの頂点数
        public int PulsarVao { get; set; }              // パルサーの VAO
        public int PrimitiveVao { get; set; }           // プリミティブの VAO
        public int PrimitiveVerticesCount { get; set; } // プリミティブの頂点数
        public Camera Camera { get; set; }              // カメラ
        public Matrix4 Projection { get; set; }         // 投影行列

        public Object(Camera camera)
        {
            Camera = camera;
        }

        public bool TryGetValue<TObject>(
            TObject obj,
            out TObject value)
        {
            if (obj != null)
            {
                value = obj;
                return true;
            }
            else
            {
                value = default!;
                return false;
            }
        }
    }
}
