
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  class Panel : CadKit.OpenGL.Canvas
  {
    CadKit.Viewer.Glue.Viewer _viewer = new CadKit.Viewer.Glue.Viewer();


    /// <summary>
    /// Contructor.
    /// </summary>
    public Panel()
    {
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~Panel()
    {
      _viewer = null;
    }


    /// <summary>
    /// Clear.
    /// </summary>
    public void clear()
    {
      _viewer.clear();
      _viewer = null;
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      this.initRenderingContext();

      this._makeCurrent();

      _viewer.create();

      this.BackColorChanged += new System.EventHandler(OnBackColorChanged);
    }


    /// <summary>
    /// The background color has changed.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public void OnBackColorChanged(object sender, System.EventArgs e)
    {
      System.Drawing.Color color = this.BackColor;
      byte red = color.R;
      byte green = color.G;
      byte blue = color.B;

      float r = (float)red / 255;
      float g = (float)green / 255;
      float b = (float)blue / 255;

      _viewer.backgroundColor(r, g, b);
    }

    /// <summary>
    /// Get the viewer.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer Viewer
    {
      get
      {
        return _viewer;
      }
    }


    /// <summary>
    /// A key was pressed.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnKeyPress(System.Windows.Forms.KeyPressEventArgs e)
    {
      base.OnKeyPress(e);
    }


    /// <summary>
    /// The mosue has moved.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
    {
      bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
      bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
      bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

      float x = e.Location.X;
      float y = e.Location.Y;

      bool mouse = left || middle || right;

      int type = CadKit.Viewer.Glue.Type.type(mouse);

      this._makeCurrent();

      _viewer.handleNavigation(x, y, left, middle, right, type);

      this._swapBuffers();
    }


    /// <summary>
    /// The mouse wheel moved.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseWheel(System.Windows.Forms.MouseEventArgs e)
    {
      base.OnMouseWheel(e);
    }


    /// <summary>
    /// A mouse button is released.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
    {
      bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
      bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
      bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

      float x = e.Location.X;
      float y = e.Location.Y;

      this._makeCurrent();

      _viewer.buttonRelease(x, y, left, middle, right);

      this._swapBuffers();
    }


    /// <summary>
    /// A mouse button is pressed.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
    {
      bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
      bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
      bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

      float x = e.Location.X;
      float y = e.Location.Y;

      this._makeCurrent();

      _viewer.buttonPress(x, y, left, middle, right);

      this._swapBuffers();
    }


    /// <summary>
    /// Render.
    /// </summary>
    protected override void _paintOpenGL()
    {
      _viewer.render();
    }


    /// <summary>
    /// Resize the viewport.
    /// </summary>
    protected override void _resizeOpenGL()
    {
      _viewer.resize(this.Size.Width, this.Size.Height);
    }
  }
}
