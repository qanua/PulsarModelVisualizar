using OpenTK;
using OpenTK.GLControl;
using OpenTK.Graphics.OpenGL;
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Threading;

namespace WpfApp
{
    public partial class MainWindow : Window
    {
        [DllImport("CalcLib.dll")]
        private static extern int GetVertices([Out] float[] buffer, int maxCount);

        private GLControl glControl;
        private float[] vertices = new float[9];


        public MainWindow()
        {
            InitializeComponent();

            glControl = new GLControl();
            glControl.Dock = System.Windows.Forms.DockStyle.Fill;
            panel.Controls.Add(glControl);

            glControl.Load += GlControl_Load;
            glControl.Paint += GlControl_Paint;
            glControl.Resize += GlControl_Resize;

            // DLL から三角形の頂点を取得
            GetVertices(vertices, vertices.Length);

            glControl.Invalidate(); // 最初の描画を要求
        }


        private void GlControl_Load(object? sender, EventArgs e)
        {
            if (glControl == null) return;
            glControl.MakeCurrent();

            GL.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            GL.Enable(EnableCap.DepthTest);
        }


        private void GlControl_Paint(object? sender, System.Windows.Forms.PaintEventArgs e)
        {
            if (glControl == null) return;
            glControl.MakeCurrent();

            GL.ClearColor(Color.Black);
            GL.Clear(ClearBufferMask.ColorBufferBit);

            GL.Begin(PrimitiveType.Triangles);
            for (int i = 0; i < vertices.Length; i += 3)
            {
                GL.Vertex3(vertices[i]*100, vertices[i + 1]*100, vertices[i + 2]*100);
            }
            GL.End();

            glControl.SwapBuffers();
        }


        private void GlControl_Resize(object? sender, EventArgs e)
        {
            if (glControl == null) return;
            glControl.MakeCurrent();

            GL.Viewport(0, 0, glControl.Width, glControl.Height);

            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadIdentity();
            GL.Ortho(-1, 1, -1, 1, -1, 1); // 2D描画用
        }
    }
}