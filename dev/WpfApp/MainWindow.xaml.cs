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
     /** @brief MainWindow.xamlの相互作用ロジック */
    public partial class MainWindow : Window
    {
        #region DllImport
        [DllImport("CalcLib.dll")]
        private static extern void setInclinationAngle([Out] int degree);

        [DllImport("CalcLib.dll")]
        private static extern int getMagneticLine([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getPolarCapNorthOpened([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getPolarCapNorthClosed([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getPolarCapSouthOpened([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getPolarCapSouthClosed([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getSkyMap([Out] float[]? buffer);

        [DllImport("CalcLib.dll")]
        private static extern int getPulseProfile(bool normalize, [Out] float[]? buffer);
        #endregion


        #region メンバ変数
        #region モデル
        /** @brief 頂点の色
         * 
         *  @details        頂点の座標更新時に変更する
         */
        private Color4 _vertexColor = Color4.White;

        /** @brief InclinationAngle
         * 
         *  @details        パルサーの回転軸と磁化軸のなす角 [度]
         */
        private int _inclinationAngle = 57;

        /** @brief ViewingAngle
         * 
         *  @details        パルサーの回転軸と視線方向のなす角 [度]
         */
        private int _viewingAngle = 90;

        /** @brief 位相
         * 
         *  @details        パルサーの自転角度
         */
        private int _phase = 90;

        /** @brief 描画対象の管理
         * 
         *  @details        GLWpfControlごとに描画対象を切り替えるため
         */
        private Dictionary<GLWpfControl, Object> _objsByControl = new();
        #endregion

        #region GUI
        /** @brief ポーラーキャップ表示ボタンの管理
         * 
         *  @details        ボタンごとに表示を切り替えるため
         */
        private Dictionary<Button, GLWpfControl> _polarCapViewByButton = new();

        /** @brief ポーラーキャップ表示で選択されているボタン */
        private Button? _selectedPolarCapButton;

        /** @brief マウス位置の前回値
         * 
         *  @details        ドラッグ中のマウス位置を保持する
         */
        private Point _lastMousePos;

        /** @brief マウスドラッグフラグ */
        private bool _isDragging = false;
        #endregion

        #region シェーダー
        /** @brief シェーダープログラム */
        private int _shader;

        /** @brief モデル変換行列のの位置 */
        private int _modelLoc;

        /** @brief ビュー変換行列のの位置 */
        private int _viewLoc;

        /** @brief 投影変換行列の位置 */
        private int _projLoc;

        /** @brief オブジェクト色の位置 */
        private int _colorLoc;
        #endregion
        #endregion

        #region 定数
        #region 色
        /** @brief 頂点の灰色
         * 
         *  @details        _vertexColor を変更するため
         */
        private Color4 VERTEX_COLOR = new Color4(0.3f, 0.3f, 0.3f, 1f);

        /** @brief 磁化軸の赤色 */
        private Color4 RED_COLOR = new Color4(1f, 75 / 255f, 50 / 255f , 1f);

        /** @brief 回転軸の緑色 */
        private Color4 GREEN_COLOR = new Color4(0f, 185 / 255f, 75 / 255f, 1f);

        /** @brief ボタンの赤色 */
        private Brush RED_BRUSH = new SolidColorBrush(Color.FromArgb(255, 225, 75, 50));

        /** @brief ガイドの緑色 */
        private Brush GREEN_BRUSH = new SolidColorBrush(Color.FromArgb(255, 0, 185, 75));

        /** @brief ガイドの青色 */
        private Brush BLUE_BRUSH = new SolidColorBrush(Color.FromArgb(255, 60, 90, 255));
        #endregion
        #endregion


        #region 初期化処理
        /** @brief MainWindowのコンストラクタ */
        public MainWindow()
        {
            InitializeComponent();

            // イベント登録
            GLControlMagneticLine.Ready += () => glControlReady(GLControlMagneticLine);
            GLControlPolarCapNorthOpened.Ready += () => glControlReady(GLControlPolarCapNorthOpened);
            GLControlPolarCapNorthClosed.Ready += () => glControlReady(GLControlPolarCapNorthClosed);
            GLControlPolarCapSouthOpened.Ready += () => glControlReady(GLControlPolarCapSouthOpened);
            GLControlPolarCapSouthClosed.Ready += () => glControlReady(GLControlPolarCapSouthClosed);
            GLControlSkyMap.Ready += () => glControlReady(GLControlSkyMap);
            GLControlPulseProfile.Ready += () => glControlReady(GLControlPulseProfile);

            GLControlMagneticLine.Loaded += (s, e) => glControlLoaded(GLControlMagneticLine);
            GLControlPolarCapNorthOpened.Loaded += (s, e) => glControlLoaded(GLControlPolarCapNorthOpened);
            GLControlPolarCapNorthClosed.Loaded += (s, e) => glControlLoaded(GLControlPolarCapNorthClosed);
            GLControlPolarCapSouthOpened.Loaded += (s, e) => glControlLoaded(GLControlPolarCapSouthOpened);
            GLControlPolarCapSouthClosed.Loaded += (s, e) => glControlLoaded(GLControlPolarCapSouthClosed);
            GLControlSkyMap.Loaded += (s, e) => glControlLoaded(GLControlSkyMap);
            GLControlPulseProfile.Loaded += (s, e) => glControlLoaded(GLControlPulseProfile);

            GLControlMagneticLine.Render += delta => glControlRender(GLControlMagneticLine);
            GLControlPolarCapNorthOpened.Render += delta => glControlRender(GLControlPolarCapNorthOpened);
            GLControlPolarCapNorthClosed.Render += delta => glControlRender(GLControlPolarCapNorthClosed);
            GLControlPolarCapSouthOpened.Render += delta => glControlRender(GLControlPolarCapSouthOpened);
            GLControlPolarCapSouthClosed.Render += delta => glControlRender(GLControlPolarCapSouthClosed);
            GLControlSkyMap.Render += delta => glControlRender(GLControlSkyMap);
            GLControlPulseProfile.Render += delta => glControlRender(GLControlPulseProfile);

            // GLWpfControlのコンテキスト設定
            var settings = new GLWpfControlSettings()
            {
                MajorVersion = 3,           // OpenGLのバージョン: OpenGL 3.x系
                MinorVersion = 1,           // OpenGLのマイナーバージョン: OpenGL 3.1
                RenderContinuously = true,  // 毎フレーム自動的に再描画
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

        #region Readyイベント
        /** @brief GLControlのコンテキスト初期化完了イベント
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
        private void glControlReady(GLWpfControl control)
        {
            // OpenGLの設定
            GL.ClearColor(Color4.White);
            GL.Enable(EnableCap.DepthTest);

            // カメラの生成
            setupCamera(control);

            // シェーダーの生成
            setupShader();

            // InclinationAngleの設定（頂点の取得前に行う）
            setInclinationAngle(_inclinationAngle);

            // 頂点の取得
            getVertices(control);

            // VAOの生成
            setupVerticesVAO(control);
            if (control == GLControlMagneticLine)
            {
                setupPrimitiveVAO(control);
            }

            // スカイマップのGUI設定
            if (control == GLControlSkyMap)
            {
                setupSkyMapView();
                ViewingAngleGuide.Stroke = BLUE_BRUSH;
                PhaseGuide.Stroke = GREEN_BRUSH;
            }

            // ポーラーキャップのGUI設定
            setupPolarCapView(control);
        }


        /** @brief カメラの設定
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
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
                    _objsByControl[control].Camera.Yaw = camera.Yaw;
                    _objsByControl[control].Camera.Pitch = camera.Pitch;
                    _objsByControl[control].Camera.Distance = camera.Distance;
                    _objsByControl[control].Camera.Target = camera.Target;
                }
                else
                {
                    _objsByControl[control] = new Object(camera);
                }
            }
        }


        /** @brief シェーダーの設定 */
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

            // シェーダーlocationの格納
            _modelLoc = GL.GetUniformLocation(_shader, "model");
            _viewLoc = GL.GetUniformLocation(_shader, "view");
            _projLoc = GL.GetUniformLocation(_shader, "projection");
            _colorLoc = GL.GetUniformLocation(_shader, "color");
        }


        /** @brief 頂点の取得
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
        private void getVertices(GLWpfControl control)
        {
            float[]? vertices = null;

            if (control == GLControlMagneticLine)
            {
                // 磁力線
                vertices = new float[getMagneticLine(null)];
                getMagneticLine(vertices);
            }
            else if (control == GLControlPolarCapNorthOpened)
            {
                // ポーラーキャップNOP
                vertices = new float[getPolarCapNorthOpened(null)];
                getPolarCapNorthOpened(vertices);
            }
            else if (control == GLControlPolarCapNorthClosed)
            {
                // ポーラーキャップNCL
                vertices = new float[getPolarCapNorthClosed(null)];
                getPolarCapNorthClosed(vertices);
            }
            else if (control == GLControlPolarCapSouthOpened)
            {
                // ポーラーキャップSOP
                vertices = new float[getPolarCapSouthOpened(null)];
                getPolarCapSouthOpened(vertices);
            }
            else if (control == GLControlPolarCapSouthClosed)
            {
                // ポーラーキャップSCL
                vertices = new float[getPolarCapSouthClosed(null)];
                getPolarCapSouthClosed(vertices);
            }
            else if (control == GLControlSkyMap)
            {
                // スカイマップ
                vertices = new float[getSkyMap(null)];
                getSkyMap(vertices);
            }
            else if (control == GLControlPulseProfile)
            {
                // パルスプロファイル
                float[] vertices2d = new float[getPulseProfile(false, null)];
                getPulseProfile(true, vertices2d);

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


        /** @brief 頂点のVAO設定
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
        private void setupVerticesVAO(GLWpfControl control)
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

                // 頂点attribute
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

                // VAOの格納
                obj.PulsarVao = vao;
                obj.PulsarVerticesCount = vertices.Length / size;

                GL.BindVertexArray(0);
            }
        }


        /** @brief プリミティブのVAO設定
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
        private void setupPrimitiveVAO(GLWpfControl control)
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

            // 頂点attribute
            GL.EnableVertexAttribArray(0);
            GL.VertexAttribPointer(
                0,
                3,
                VertexAttribPointerType.Float,
                false,
                3 * sizeof(float),
                0
            );

            // VAOの格納
            obj.PrimitiveVao = vao;
            obj.PrimitiveVerticesCount = vertices.Length / 3;

            GL.BindVertexArray(0);
        }


        /** @brief スカイマップ表示の設定 */
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


        /** @brief ポーラーキャップ表示の設定
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
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
                        _selectedPolarCapButton = PolarCapNorthOpenedButton;
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
        #endregion

        #region Loadedイベント
        /** @brief GLControlの読み込みイベント
         * 
         *  @param[in]      control GLWpfControlコントロール
         */
        private void glControlLoaded(GLWpfControl control)
        {
            // レイアウト処理後に実行（ActualWidth、ActualHeight 取得のため）
            Dispatcher.BeginInvoke(new Action(() =>
            {
                // プロジェクションの設定
                setupProjection(control);

            }), System.Windows.Threading.DispatcherPriority.Loaded);
        }


        /** @brief 投影行列の設定
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
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


        /** @brief スライダーの読み込みイベント
         * 
         *  @param[in]      sender      イベント発生スライダー
         *  @param[in]      e           イベントデータ
         */
        private void Slider_Loaded(object sender, RoutedEventArgs e)
        {
            var slider = (Slider)sender;

            if (slider == MagneticLineInclinationAngleSlider)
            {
                MagneticLineInclinationAngleSlider.Value = _inclinationAngle;
            }
            else if (slider == SkyMapViewingAngleSlider)
            {
                SkyMapViewingAngleSlider.Value = _viewingAngle;
                updateSkyMapGuideLine(slider);
            }
            else if (slider == SkyMapPhaseSlider)
            {
                SkyMapPhaseSlider.Value = _phase;
                updateSkyMapGuideLine(slider);
            }
        }


        /** @brief メイン画面の読み込みイベント
         * 
         *  @param[in]      sender      イベント発生画面
         *  @param[in]      e           イベントデータ
         */
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            double baseWidth = 1080;
            double baseHeight = 1350;

            // 現在の画面サイズ取得
            double screenWidth = SystemParameters.PrimaryScreenWidth;
            double screenHeight = SystemParameters.PrimaryScreenHeight;

            // スケーリング係数の決定
            double scale = Math.Min(screenWidth / 2560, screenHeight / 1440);

            // 新しい画面サイズ
            Width = baseWidth * scale;
            Height = baseHeight * scale;

            // 画面中央に配置
            Left = (screenWidth - Width) * 0.5;
            Top = (screenHeight - Height) * 0.5;
        }
        #endregion
        #endregion


        #region 描画処理
        #region Renderイベント
        /** @brief GLControl のレンダーイベント
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
        private void glControlRender(GLWpfControl control)
        {
            // バッファの消去
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            // 頂点の更新
            if (_objsByControl[control].TryGetValue(_objsByControl[control].PulsarVertices, out var vertices) &&
                vertices != null && vertices.All(x => x == 0))
            {
                // 頂点の取得
                getVertices(control);

                // VAOの更新
                setupVerticesVAO(control);

                // 磁力線のスライダーを有効化
                var thumb = (MagneticLineInclinationAngleSlider.Template.FindName("PART_Thumb", MagneticLineInclinationAngleSlider) as Thumb);
                if (thumb != null)
                {
                    thumb.IsHitTestVisible = true;
                }
            }

            drawObjects(control);
        }


        /** @brief Objectの描画
         * 
         *  @param[in]      control     GLWpfControlコントロール
         */
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


        /** @brief 頂点の描画
         * 
         *  @param[in]      control     GLWpfControlコントロール
         *  @param[in]      vao         対象のVAO
         *  @param[in]      count       対象の頂点数
         */
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
        #endregion
        #endregion


        #region GUI操作
        #region GLControlのマウスイベント
        /** @brief MagneticLine 上でのマウス左ボタン押下イベント
         * 
         *  @param[in]      sender      GLControlMagneticLine
         *  @param[in]      e           マウス状態などを含むイベントデータ
         */
        private void MagneticLine_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if(e.ClickCount == 1)
            {
                _isDragging = true;
                _lastMousePos = e.GetPosition((IInputElement)sender);
            }
            else if (e.ClickCount == 2)
            {
                var control = (GLWpfControl)sender;
                setupCamera(control);

                // ViewingAngleスライダーの同期
                SkyMapViewingAngleSlider.Value = 90;

                // Phaseスライダーの同期
                SkyMapPhaseSlider.Value = 90;
            }
        }


        /** @brief MagneticLine 上でのマウス移動イベント
         * 
         *  @param[in]      sender      GLControlMagneticLine
         *  @param[in]      e           マウス位置などを含むイベントデータ
         */
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

                    // ViewingAngleスライダーの同期
                    _viewingAngle = 90 + (int)camera.Pitch;
                    SkyMapViewingAngleSlider.Value = _viewingAngle;

                    // Phaseスライダーの同期
                    int delta = (90 + (int)camera.Yaw) % 360;
                    _phase = (0 <= delta) ? delta : 360 + delta;
                    SkyMapPhaseSlider.Value = _phase;

                    _lastMousePos = pos;
                }
            }
        }


        /** @brief MagneticLine 上でのマウスボタン離上イベント
         * 
         *  @param[in]      sender      GLControlMagneticLine
         *  @param[in]      e           マウスのイベントデータ
         */
        private void MagneticLine_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _isDragging = false;
        }


        /** @brief MagneticLine 上からのマウス離脱イベント
         * 
         *  @param[in]      sender      GLControlMagneticLine
         *  @param[in]      e           マウスのイベントデータ
         */
        private void MagneticLine_MouseLeave(object sender, MouseEventArgs e)
        {
            _isDragging = false;
        }


        /** @brief GLコントロール上からでのマウスホイールイベント
         * 
         *  @param[in]      sender      GLControl（GLControlPolarCap... / GLControlPulseProfile）
         *  @param[in]      e           ホイールの回転量などを含むイベントデータ
         */
        private void GLControl_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            var control = (GLWpfControl)sender;

            if (control == GLControlPulseProfile)
            {
                if (_objsByControl[control].TryGetValue(_objsByControl[control].Camera, out var camera))
                {
                    zoomCamera(camera, 1.0f, 0.5f, 179.5f, -e.Delta);

                    // ViewingAngleスライダーの同期
                    SkyMapViewingAngleSlider.Value = (int)camera.Distance;
                }
            }
            else if (
                control == GLControlPolarCapNorthOpened ||
                control == GLControlPolarCapNorthClosed ||
                control == GLControlPolarCapSouthOpened ||
                control == GLControlPolarCapSouthClosed)
            {
                var polars = _polarCapViewByButton.Values;

                foreach (var polar in polars)
                {
                    if (_objsByControl[control].TryGetValue(_objsByControl[polar].Camera, out var camera))
                    {
                        if (polar == GLControlPolarCapNorthOpened || polar == GLControlPolarCapNorthClosed)
                        {
                            zoomCamera(camera, -0.0001f, 0.0025f, 0.005f, e.Delta);
                        }
                        else if (polar == GLControlPolarCapSouthOpened || polar == GLControlPolarCapSouthClosed)
                        {
                            zoomCamera(camera, 0.0001f, -0.0025f, -0.005f, e.Delta);
                        }
                    }
                }
            }
        }


        /** @brief カメラのズームイン/アウト
         * 
         *  @param[in]      camera      対象のカメラオブジェクト
         *  @param[in]      zoom        ズーム量
         *  @param[in]      min         カメラと被写体との最小距離
         *  @param[in]      max         カメラと被写体との最大距離
         *  @param[in]      delta       ズーム方向（+/-）
         */
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
        #endregion

        #region スライダーイベント
        /** @brief スライダーの値変更イベント
         * 
         *  @param[in]      sender      スライダー
         *  @param[in]      e           変更前後の値などを含むイベントデータ
         */
        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var slider = (Slider)sender;

            if (slider == MagneticLineInclinationAngleSlider)
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

                    // 磁力線のPhaseを同期
                    Object obj = _objsByControl[GLControlMagneticLine];

                    if (obj.TryGetValue(obj.Camera, out var camera))
                    {
                        camera.Yaw = _phase - 90f;
                    }
                }
                updateSkyMapGuideLine(slider);
            }
        }


        /** @brief スカイマップのガイドライン更新
         * 
         *  @param[in]      sender      スライダー
         */
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


        /** @brief スライダーのドラッグ操作完了イベント
         * 
         *  @param[in]      sender      スライダーつまみ
         *  @param[in]      e           ドラッグ操作結果を含むイベントデータ
         */
        private void Thumb_DragCompleted(object sender, DragCompletedEventArgs e)
        {
            var thumb = sender as Thumb;
            var slider = findParentSlider(thumb);

            if (slider != null && thumb != null)
            {
                int degree = (int)slider.Value;

                if (slider == MagneticLineInclinationAngleSlider)
                {
                    // 磁力線のスライダーを無効化
                    thumb.IsHitTestVisible = false;

                    setInclinationAngle(_inclinationAngle);

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


        /** @brief 子から親要素のスライダーを探索
         * 
         *  @param[in]      child       スライダーの子要素
         *  
         *  @return         親要素のスライダー
         */
        private Slider? findParentSlider(DependencyObject? child)
        {
            while (child != null && !(child is Slider))
            {
                child = VisualTreeHelper.GetParent(child);
            }
            return child as Slider;
        }
        #endregion

        #region ボタンのマウスイベント
        /** @brief PolarCap の矢印ボタンクリックイベント
         * 
         *  @param[in]      sender      PolarCap...Button
         *  @param[in]      e           子から親要素へ伝わるルーティングイベント
         */
        private void PolarCapButton_Click(object sender, RoutedEventArgs e)
        {
            if (_selectedPolarCapButton != null)
            {
                _selectedPolarCapButton.Background = Brushes.Gray;
                _polarCapViewByButton[_selectedPolarCapButton].Visibility = Visibility.Hidden;
            }

            var clicked = (Button)sender;
            clicked.Background = RED_BRUSH;
            _polarCapViewByButton[clicked].Visibility = Visibility.Visible;

            _selectedPolarCapButton = clicked;
        }


        /** @brief PolarCap の矢印ボタン上のマウス侵入イベント
         * 
         *  @param[in]      sender      PolarCap...Button
         *  @param[in]      e           子から親要素へ伝わるルーティングイベント
         */
        private void PolarCapButton_MouseEnter(object sender, RoutedEventArgs e)
        {
            var overed = (Button)sender;

            if (overed.Background == Brushes.Gray)
            {
                overed.Background = Brushes.LightGray;
            }
        }


        /** @brief PolarCap の矢印ボタン上からのマウス離脱イベント
         * 
         *  @param[in]      sender      PolarCap...Button
         *  @param[in]      e           子から親要素へ伝わるルーティングイベント
         */
        private void PolarCapButton_MouseLeave(object sender, RoutedEventArgs e)
        {
            var overed = (Button)sender;

            if (overed.Background == Brushes.LightGray)
            {
                overed.Background = Brushes.Gray;
            }
        }
        #endregion

        #region タイトルバー操作
        /** @brief タイトルバー上でのマウス左ボタン押下イベント
         * 
         *  @param[in]      sender      タイトルバーの Grid
         *  @param[in]      e           マウス状態などを含むイベントデータ
         */
        private void TitleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ButtonState == MouseButtonState.Pressed)
                DragMove();
        }


        /** @brief タイトルバーの閉じるボタンクリックイベント
         * 
         *  @param[in]      sender      Button
         *  @param[in]      e           子から親要素へ伝わるルーティングイベント
         */
        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        #endregion
        #endregion
    }


    #region クラス
    #region Camera
    /** @class Camera
     * 
     *  @brief カメラオブジェクトを管理
     */
    class Camera
    {
        /** @brief 水平方向回転角 */
        public float Yaw { get; set; }

        /** @brief 垂直方向回転角 */
        public float Pitch { get; set; }

        /** @brief 注視点からの距離 */
        public float Distance { get; set; }

        /** @brief 注視点 */
        public Vector3 Target { get; set; }


        /** @brief Cameraのコンストラクタ
         * 
         *  @param[in]      yaw         水平方向回転角
         *  @param[in]      pitch       垂直方向回転角
         *  @param[in]      distance    注視点からの距離
         *  @param[in]      target      注視点
         */
        public Camera(float yaw, float pitch, float distance, Vector3 target)
        {
            Yaw = yaw;
            Pitch = pitch;
            Distance = distance;
            Target = target;
        }

        /** @brief 球面座標からカメラ位置を算出 */
        public Matrix4 GetViewMatrix()
        {
            Vector3 cameraPos = new Vector3(
                Target.X + Distance * (float)(Math.Cos(MathHelper.DegreesToRadians(Pitch)) * Math.Cos(MathHelper.DegreesToRadians(Yaw))),
                Target.Y + Distance * (float)(Math.Sin(MathHelper.DegreesToRadians(Pitch))),
                Target.Z + Distance * (float)(Math.Cos(MathHelper.DegreesToRadians(Pitch)) * Math.Sin(MathHelper.DegreesToRadians(Yaw)))
            );

            return Matrix4.LookAt(cameraPos, Target, Vector3.UnitY);
        }
    }
    #endregion

    #region Object
    /** @class Object
     * 
     *  @brief 描画対象を管理
     */
    class Object
    {
        /** @brief パルサーの頂点 */
        public float[]? PulsarVertices { get; set; }

        /** @brief パルサーの頂点数 */
        public int PulsarVerticesCount { get; set; }

        /** @brief パルサーのVAO */
        public int PulsarVao { get; set; }

        /** @brief プリミティブのVAO */
        public int PrimitiveVao { get; set; }

        /** @brief プリミティブの頂点数 */
        public int PrimitiveVerticesCount { get; set; }

        /** @brief カメラオブジェクト */
        public Camera Camera { get; set; }

        /** @brief 投影行列 */
        public Matrix4 Projection { get; set; }


        /** @brief Objectのコンストラクタ
         * 
         *  @param[in]      camera      カメラオブジェクト
         */
        public Object(Camera camera)
        {
            Camera = camera;
        }

        /** @brief 入力オブジェクトからの値の取得
         * 
         *  @tparam         T           対象のテンプレートパラメータ
         *  @param[in]      obj         入力オブジェクト
         *  @param[out]     value       出力値
         *  
         *  @return         true:   objがnullでなはい
         *                  false:  objがnullである
         */
        public bool TryGetValue<T>(T obj, out T value)
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
    #endregion
    #endregion
}