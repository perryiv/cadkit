
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
    CadKit.Interfaces.IViewerMode,
    CadKit.Interfaces.ICamera,
    CadKit.Interfaces.IExportImage,
    CadKit.Interfaces.IExportScene,
    CadKit.Interfaces.IFrameDump
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
      lock (this.Mutex)
      {
        _viewer.clear();
        _viewer = null;
      }
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      lock (this.Mutex)
      {
        this.initRenderingContext();

        // Set the render context.
        _viewer.RenderContext = this.RenderContext;

        // Create the viewer.
        _viewer.create();

        // Set initial background color.
        this._updateViewerBackground();
      }
    }


    /// <summary>
    /// Background color
    /// </summary>
    public override System.Drawing.Color BackColor
    {
      get { lock (this.Mutex) { return base.BackColor; } }
      set
      {
        lock (this.Mutex)
        {
          base.BackColor = value;
          this._updateViewerBackground();
        }
      }
    }


    /// <summary>
    /// Updte the viewer's background color.
    /// </summary>
    private void _updateViewerBackground()
    {
      lock (this.Mutex)
      {
        System.Drawing.Color color = this.BackColor;
        if (null == color || null == _viewer)
          return;

        byte red = color.R;
        byte green = color.G;
        byte blue = color.B;

        float r = (float)red / 255;
        float g = (float)green / 255;
        float b = (float)blue / 255;

        _viewer.backgroundColor(r, g, b);
      }
    }


    /// <summary>
    /// Get the viewer.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer Viewer
    {
      get { lock (this.Mutex) { return _viewer; } }
    }


    /// <summary>
    /// A key was pressed.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
    {
      try
      {
        lock (this.Mutex)
        {
          // RESET
          if (e.KeyCode == System.Windows.Forms.Keys.Space || e.KeyCode == System.Windows.Forms.Keys.R)
          {
            // Move the camera.
            _viewer.camera(CadKit.Interfaces.CameraOption.RESET);
          }

          // FIT
          else if (e.KeyCode == System.Windows.Forms.Keys.F)
          {
            // Move the camera.
            _viewer.camera(CadKit.Interfaces.CameraOption.FIT);
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
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 5235829930: {0}", ex.Message);
      }
    }


    /// <summary>
    /// The mosue has moved.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
    {
      try
      {
        lock (this.Mutex)
        {
          bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          bool mouse = left || middle || right;
          if (false == mouse)
            return;

          CadKit.Viewer.Glue.Viewer.Type type = mouse ? CadKit.Viewer.Glue.Viewer.Type.DRAG : CadKit.Viewer.Glue.Viewer.Type.MOVE;

          _viewer.handleNavigation(x, y, left, middle, right, type);
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 3325853270: {0}", ex.Message);
      }
    }


    /// <summary>
    /// The mouse wheel moved.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseWheel(System.Windows.Forms.MouseEventArgs e)
    {
      try
      {
        lock (this.Mutex)
        {
          base.OnMouseWheel(e);
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 2951730899: {0}", ex.Message);
      }
    }


    /// <summary>
    /// A mouse button is released.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
    {
      try
      {
        lock (this.Mutex)
        {
          bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          _viewer.buttonRelease(x, y, left, middle, right);
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 2213011651: {0}", ex.Message);
      }
    }


    /// <summary>
    /// A mouse button is pressed.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
    {
      try
      {
        lock (this.Mutex)
        {
          bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          _viewer.buttonPress(x, y, left, middle, right);
          _viewer.handleSeek(x, y, left);
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 3544395587: {0}", ex.Message);
      }
    }


    /// <summary>
    /// Render.
    /// </summary>
    protected override void _paintOpenGL()
    {
      lock (this.Mutex)
      {
        if (null != _viewer)
          _viewer.render();
      }
    }


    /// <summary>
    /// Resize the viewport.
    /// </summary>
    protected override void _resizeOpenGL()
    {
      lock (this.Mutex)
      {
        if (null != _viewer)
          _viewer.resize(this.Size.Width, this.Size.Height);
      }
    }


    /// <summary>
    /// Get/Set the mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Viewer.Glue.Viewer.ViewMode mode = this.Viewer.getMode();
          return (CadKit.Interfaces.ViewMode)mode;
        }
      }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.setMode((CadKit.Viewer.Glue.Viewer.ViewMode)value);
          if (value == CadKit.Interfaces.ViewMode.PICK)
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


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      lock (this.Mutex)
      {
        _viewer.camera(option);
      }
    }

    CadKit.Interfaces.Filters CadKit.Interfaces.IExportImage.Filters
    {
      get 
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("JPEG Image (*.jpg)", "*.jpg"));
        filters.Add(new CadKit.Interfaces.Filter("PNG Image (*.png)", "*.png"));
        filters.Add(new CadKit.Interfaces.Filter("BMP Image (*.bmp)", "*.bmp"));
        filters.Add(new CadKit.Interfaces.Filter("RoboMet 3D (*.r3d)", "*.r3d"));
        return filters;
      }
    }

    bool CadKit.Interfaces.IExportImage.exportImage(string filename)
    {
      return _viewer.writeImageFile(filename);
    }

    bool CadKit.Interfaces.IExportImage.exportImage(string filename, int width, int height)
    {
      return _viewer.writeImageFile(filename, width, height);
    }

    CadKit.Interfaces.Filters CadKit.Interfaces.IExportScene.Filters
    {
      get 
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph Binary (*.ive)", "*.ive"));
        filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph ASCII (*.osg)", "*.osg"));
        return filters;
      }
    }

    bool CadKit.Interfaces.IExportScene.exportScene(string filename)
    {
      return _viewer.writeSceneFile(filename);
    }

    public string Directory
    {
      get
      {
        return _viewer.Directory;
      }
      set
      {
        _viewer.Directory = value;
      }
    }

    public string Filename
    {
      get
      {
        return _viewer.Filename;
      }
      set
      {
        _viewer.Filename = value;
      }
    }

    public string Extension
    {
      get
      {
        return _viewer.Extension;
      }
      set
      {
        _viewer.Extension = value;
      }
    }

    public bool DumpFrames
    {
      get
      {
        return _viewer.DumpFrames;
      }
      set
      {
        _viewer.DumpFrames = value;
      }
    }
  }
}
