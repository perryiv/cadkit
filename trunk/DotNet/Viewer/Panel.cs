
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  class Panel : 
    CadKit.OpenGL.Canvas,
    CadKit.Interfaces.IViewerMode
  {
    CadKit.Viewer.Glue.Viewer _viewer = new CadKit.Viewer.Glue.Viewer();


    /// <summary>
    /// Contructor.
    /// </summary>
    public Panel()
    {
      _viewer.setMode(CadKit.Viewer.Glue.Viewer.ViewMode.NAVIGATION);
      this.ContextMenuStrip = null;      
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
    protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
    {
      this._makeCurrent();

      // RESET
      if (e.KeyCode == System.Windows.Forms.Keys.Space || e.KeyCode == System.Windows.Forms.Keys.R)
      {
        // Move the camera.
        _viewer.camera(CadKit.Viewer.Glue.Viewer.CameraOption.RESET);
      }

      // FIT
      else if (e.KeyCode == System.Windows.Forms.Keys.F)
      {
        // Move the camera.
        _viewer.camera(CadKit.Viewer.Glue.Viewer.CameraOption.FIT);
      }

      // Change mode to navigation
      else if (e.KeyCode == System.Windows.Forms.Keys.N)
      {
        this.Mode = CadKit.Interfaces.ViewMode.NAVIGATE;
      }

      // Change mode to pick
      else if (e.KeyCode == System.Windows.Forms.Keys.P)
      {
        this.Mode = CadKit.Interfaces.ViewMode.PICK;
      }

      this._swapBuffers();
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
      float y = this.Size.Height - e.Location.Y;

      bool mouse = left || middle || right;

      CadKit.Viewer.Glue.Viewer.Type type = mouse ? CadKit.Viewer.Glue.Viewer.Type.DRAG : CadKit.Viewer.Glue.Viewer.Type.MOVE;

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
      float y = this.Size.Height - e.Location.Y;

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
      float y = this.Size.Height - e.Location.Y;

      this._makeCurrent();

      _viewer.buttonPress(x, y, left, middle, right);
      _viewer.handleSeek(x, y, left);

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


    /// <summary>
    /// Get/Set the mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get
      {
        CadKit.Viewer.Glue.Viewer.ViewMode mode = this.Viewer.getMode();
        return (CadKit.Interfaces.ViewMode)mode;
      }
      set
      {
        this.Viewer.setMode((CadKit.Viewer.Glue.Viewer.ViewMode)value);
        if(value == CadKit.Interfaces.ViewMode.PICK)
        {
          this.ContextMenuStrip = this.buildContextMenu();
        }
        else
        {
          this.ContextMenuStrip = null;
        }
      }
    }
  }
}
