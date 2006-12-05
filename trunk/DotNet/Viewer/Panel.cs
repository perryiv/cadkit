
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  public partial class Panel :
    CadKit.OpenGL.Canvas,
    System.IDisposable,
    CadKit.Interfaces.IViewerMode,
    CadKit.Interfaces.ICamera,
    CadKit.Interfaces.IExportImage,
    CadKit.Interfaces.IExportScene,
    CadKit.Interfaces.IFrameDump
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Viewer.Glue.Viewer _viewer = new CadKit.Viewer.Glue.Viewer();
    private string REGISTRY_SECTION = "CadKit.Viewer.Panel";
    private string FRAME_DUMP_DIRECTORY_KEY = "FrameDumpDirectory";
    private string FRAME_DUMP_FILENAME_KEY = "FrameDumpFilename";
    private string FRAME_DUMP_EXTENSION_KEY = "FrameDumpExtension";
    private string FRAME_DUMP_SCALE_KEY = "FrameDumpScale";

    /// <summary>
    /// Contructor.
    /// </summary>
    public Panel()
    {
      _viewer.setMode(CadKit.Viewer.Glue.Viewer.ViewMode.NAVIGATION);
      this.ContextMenuStrip = null;
      this.Directory = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_DIRECTORY_KEY, this.Directory);
      this.BaseFilename = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_FILENAME_KEY, this.BaseFilename);
      this.Extension = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_EXTENSION_KEY, this.Extension);
      this.FrameScale = CadKit.Persistence.Registry.Instance.getFloat(REGISTRY_SECTION, this.FRAME_DUMP_SCALE_KEY, this.FrameScale);
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~Panel()
    {
      this.Dispose();
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
    /// Clear color
    /// </summary>
    public override System.Drawing.Color ClearColor
    {
      get { lock (this.Mutex) { return base.ClearColor; } }
      set
      {
        lock (this.Mutex)
        {
          base.ClearColor = value;
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
        System.Drawing.Color color = this.ClearColor;
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
            this.camera(CadKit.Interfaces.CameraOption.RESET);
          }

          // FIT
          else if (e.KeyCode == System.Windows.Forms.Keys.F)
          {
            // Move the camera.
            this.camera(CadKit.Interfaces.CameraOption.FIT);
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
    /// The mouse has moved.
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
      lock (this.Mutex) { _viewer.camera(option); }
    }


    /// <summary>
    /// Return the appropriate filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportImage.Filters
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
          filters.Add(new CadKit.Interfaces.Filter("JPEG Image (*.jpg)", "*.jpg"));
          filters.Add(new CadKit.Interfaces.Filter("PNG Image (*.png)", "*.png"));
          filters.Add(new CadKit.Interfaces.Filter("BMP Image (*.bmp)", "*.bmp"));
          return filters;
        }
      }
    }


    /// <summary>
    /// Return appropriate filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportScene.Filters
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
          filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph Binary (*.ive)", "*.ive"));
          filters.Add(new CadKit.Interfaces.Filter("OpenSceneGraph ASCII (*.osg)", "*.osg"));
          return filters;
        }
      }
    }


    /// <summary>
    /// Export the image.
    /// </summary>
    void CadKit.Interfaces.IExportImage.export(string filename)
    {
      lock (this.Mutex)
      {
        if (false == _viewer.writeImageFile(filename))
        {
          throw new System.Exception(System.String.Format("Error 3812935736: Failed to export image file: {0}", filename));
        }
      }
    }


    /// <summary>
    /// Image export width.
    /// </summary>
    float CadKit.Interfaces.IExportImage.Scale
    {
      get { lock (this.Mutex) { return this.FrameScale; }}
      set { lock (this.Mutex) { this.FrameScale = value;}}
    }


    /// <summary>
    /// Export the scene.
    /// </summary>
    void CadKit.Interfaces.IExportScene.export(string filename)
    {
      lock (this.Mutex)
      {
        if (false == _viewer.writeSceneFile(filename))
        {
          throw new System.Exception(System.String.Format("Error 1243330417: Failed to export scene file: {0}", filename));
        }
      }
    }


    /// <summary>
    /// Return the appropriate object for the property grid.
    /// </summary>
    public override object PropertyGridObject
    {
      get { lock (this.Mutex) { return new CadKit.Viewer.Panel.PropertyProxy(this); } }
    }


    /// <summary>
    /// Number of rendering passes property.
    /// </summary>
    public uint RenderingPasses
    {
      get
      {
        lock (this.Mutex)
        {
          return ((null == _viewer) ? 0 : _viewer.numRenderPasses());
        }
      }
      set
      {
        lock (this.Mutex)
        {
          if (null != _viewer)
          {
            _viewer.numRenderPasses(value);
          }
        }
      }
    }


    /// <summary>
    /// Scatter scale.
    /// </summary>
    public double ScatterScale
    {
      get
      {
        lock (this.Mutex)
        {
          return ((null == _viewer) ? 0 : _viewer.scatterScale());
        }
      }
      set
      {
        lock (this.Mutex)
        {
          if (null != _viewer)
          {
            _viewer.scatterScale(value);
          }
        }
      }
    }


    /// <summary>
    /// Get/set the directory where screen-dumps are written.
    /// </summary>
    public string Directory
    {
      get { lock (this.Mutex) { return _viewer.Directory; } }
      set
      {
        lock (this.Mutex)
        {
          _viewer.Directory = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_DIRECTORY_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the filename used when making screen-dumps.
    /// </summary>
    public string BaseFilename
    {
      get { lock (this.Mutex) { return _viewer.Filename; } }
      set
      {
        lock (this.Mutex)
        {
          _viewer.Filename = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_FILENAME_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the file extension used when making screen-dumps.
    /// </summary>
    public string Extension
    {
      get { lock (this.Mutex) { return _viewer.Extension; } }
      set
      {
        lock (this.Mutex)
        {
          _viewer.Extension = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_EXTENSION_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the flag that says whether or not to dump frames.
    /// </summary>
    public bool DumpFrames
    {
      get { lock (this.Mutex) { return _viewer.DumpFrames; } }
      set { lock (this.Mutex) { _viewer.DumpFrames = value; } }
    }


    /// <summary>
    /// Get/set the size-scale to dump frames.
    /// </summary>
    public float FrameScale
    {
      get
      {
        lock (this.Mutex)
        {
          return _viewer.frameDumpScale();
        }
      }
      set
      {
        lock (this.Mutex)
        {
          _viewer.frameDumpScale ( value );
          CadKit.Persistence.Registry.Instance.setFloat(REGISTRY_SECTION, this.FRAME_DUMP_SCALE_KEY, value);
        }
      }
    }


    /// <summary>
    /// Called by the system when this object is about to be destroyed.
    /// </summary>
    void System.IDisposable.Dispose()
    {
    }
  }
}
