
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
    CadKit.Interfaces.IFrameDump,
    CadKit.Interfaces.ISnapShot
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Viewer.Glue.Viewer _viewer = new CadKit.Viewer.Glue.Viewer();
    private readonly string REGISTRY_SECTION = "CadKit.Viewer.Panel";
    private readonly string FRAME_DUMP_DIRECTORY_KEY = "FrameDumpDirectory";
    private readonly string FRAME_DUMP_FILENAME_KEY = "FrameDumpFilename";
    private readonly string FRAME_DUMP_EXTENSION_KEY = "FrameDumpExtension";
    private readonly string FRAME_DUMP_SCALE_KEY = "FrameDumpScale";
    private readonly string COLOR_CORNERS = "ColorCorners";


    /// <summary>
    /// Contructor.
    /// </summary>
    public Panel()
    {
      this.Viewer.setMode(CadKit.Viewer.Glue.Viewer.ViewMode.NAVIGATION);
      this.ContextMenuStrip = null;
      this.Directory = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_DIRECTORY_KEY, this.Directory);
      this.BaseFilename = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_FILENAME_KEY, this.BaseFilename);
      this.Extension = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, this.FRAME_DUMP_EXTENSION_KEY, this.Extension);
      this.FrameScale = CadKit.Persistence.Registry.Instance.getFloat(REGISTRY_SECTION, this.FRAME_DUMP_SCALE_KEY, this.FrameScale);

      this.Corners = (CadKit.Viewer.Glue.Viewer.Corners)CadKit.Persistence.Registry.Instance.getUint(REGISTRY_SECTION, COLOR_CORNERS, (uint)this.Corners);
      this._initClearColor(CadKit.Viewer.Glue.Viewer.Corners.TOP_LEFT);
      this._initClearColor(CadKit.Viewer.Glue.Viewer.Corners.TOP_RIGHT);
      this._initClearColor(CadKit.Viewer.Glue.Viewer.Corners.BOTTOM_LEFT);
      this._initClearColor(CadKit.Viewer.Glue.Viewer.Corners.BOTTOM_RIGHT);

      this.Load += this._load;
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
    /// Called when the panel is loaded.
    /// </summary>
    private void _load(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form form = this.FindForm();
      if (null != form)
      {
        form.FormClosing += new System.Windows.Forms.FormClosingEventHandler(_formClosing);
      }
    }


    /// <summary>
    /// Called when the form is closing.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      this._saveColor(CadKit.Viewer.Glue.Viewer.Corners.TOP_LEFT);
      this._saveColor(CadKit.Viewer.Glue.Viewer.Corners.TOP_RIGHT);
      this._saveColor(CadKit.Viewer.Glue.Viewer.Corners.BOTTOM_LEFT);
      this._saveColor(CadKit.Viewer.Glue.Viewer.Corners.BOTTOM_RIGHT);
    }


    /// <summary>
    /// Initialize the clear color.
    /// </summary>
    private void _initClearColor(CadKit.Viewer.Glue.Viewer.Corners corner)
    {
      CadKit.Viewer.Glue.Viewer.Corners original = this.Corners;
      try
      {
        this.Corners = corner;
        this.ClearColor = CadKit.Persistence.Registry.Instance.getColor(REGISTRY_SECTION, corner.ToString(), this.ClearColor);
      }
      finally
      {
        this.Corners = original;
      }
    }


    /// <summary>
    /// Clear.
    /// </summary>
    public void clear()
    {
      lock (this.Mutex)
      {
        this.Viewer.clear();
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
        this.Viewer.RenderContext = this.RenderContext;

        // Create the viewer.
        this.Viewer.create();
      }
    }


    /// <summary>
    /// Clear color
    /// </summary>
    public override System.Drawing.Color ClearColor
    {
      get
      {
        return (System.Drawing.Color)this.Viewer.backgroundColor();
      }
      set
      {
        base.ClearColor = value;
        this.Viewer.backgroundColor(value);
      }
    }


    /// <summary>
    /// Save colors to disk.
    /// </summary>
    private void _saveColor(CadKit.Viewer.Glue.Viewer.Corners corner)
    {
      CadKit.Viewer.Glue.Viewer.Corners original = this.Corners;
      try
      {
        this.Corners = corner;
        CadKit.Persistence.Registry.Instance.setColor(REGISTRY_SECTION, corner.ToString(), this.ClearColor);
      }
      finally
      {
        this.Corners = original;
      }
    }


    /// <summary>
    /// Get/set the clear color corner.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer.Corners Corners
    {
      get { lock (this.Mutex) { return this.Viewer.backgroundCorners(); } }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.backgroundCorners(value);
          CadKit.Persistence.Registry.Instance.setUint(REGISTRY_SECTION, COLOR_CORNERS, (uint)value);
        }
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

          if (null != this.Viewer)
          {
            this.Viewer.handleNavigation(x, y, left, middle, right, type);
          }
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

          if (null != this.Viewer)
          {
            this.Viewer.buttonRelease(x, y, left, middle, right);
          }
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

          if (null != this.Viewer)
          {
            this.Viewer.buttonPress(x, y, left, middle, right);
            this.Viewer.handleSeek(x, y, left);
          }
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
        if (null != this.Viewer)
          this.Viewer.render();
      }
    }


    /// <summary>
    /// Resize the viewport.
    /// </summary>
    protected override void _resizeOpenGL()
    {
      lock (this.Mutex)
      {
        if (null != this.Viewer)
          this.Viewer.resize(this.Size.Width, this.Size.Height);
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
        if (null != this.Viewer)
        {
          this.Viewer.camera(option);
        }
      }
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
    void CadKit.Interfaces.IExportImage.export(string file)
    {
      this.writeImageFile(file);
    }


    /// <summary>
    /// Export the image.
    /// </summary>
    public void writeImageFile(string file)
    {
      lock (this.Mutex)
      {
        if (false == this.Viewer.writeImageFile(file))
        {
          throw new System.Exception(System.String.Format("Error 3812935736: Failed to export image file: {0}", file));
        }
      }
    }


    /// <summary>
    /// Image export width.
    /// </summary>
    float CadKit.Interfaces.IExportImage.Scale
    {
      get { lock (this.Mutex) { return this.FrameScale; } }
      set { lock (this.Mutex) { this.FrameScale = value; } }
    }


    /// <summary>
    /// Export the scene.
    /// </summary>
    void CadKit.Interfaces.IExportScene.export(string filename, CadKit.Interfaces.SceneExport.Option option)
    {
      lock (this.Mutex)
      {
        if (CadKit.Interfaces.SceneExport.Option.ENTIRE_SCENE == option)
        {
          if (false == this.Viewer.writeSceneFile(filename))
          {
            throw new System.Exception(System.String.Format("Error 1243330417: Failed to export scene file: {0}", filename));
          }
        }
        else if (CadKit.Interfaces.SceneExport.Option.MODEL_ONLY == option)
        {
          if (false == this.Viewer.writeModelFile(filename))
          {
            throw new System.Exception(System.String.Format("Error 3455377944: Failed to export model file: {0}", filename));
          }
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
          return this.Viewer.numRenderPasses();
        }
      }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.numRenderPasses(value);
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
          return this.Viewer.scatterScale();
        }
      }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.scatterScale(value);
        }
      }
    }


    /// <summary>
    /// Available rendering passes.
    /// </summary>
    public System.UInt32[] AvailableRenderingPasses
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Viewer.Glue.Viewer.RenderPasses passes = this.Viewer.availableRenderPasses();
          return (null == passes) ? null : passes.ToArray();
        }
      }
    }


    /// <summary>
    /// Get/set the directory where screen-dumps are written.
    /// </summary>
    public string Directory
    {
      get { lock (this.Mutex) { return this.Viewer.Directory; } }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.Directory = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_DIRECTORY_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the filename used when making screen-dumps.
    /// </summary>
    public string BaseFilename
    {
      get { lock (this.Mutex) { return this.Viewer.Filename; } }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.Filename = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_FILENAME_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the file extension used when making screen-dumps.
    /// </summary>
    public string Extension
    {
      get { lock (this.Mutex) { return this.Viewer.Extension; } }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.Extension = value;
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_EXTENSION_KEY, value);
        }
      }
    }


    /// <summary>
    /// Get/set the flag that says whether or not to dump frames.
    /// </summary>
    public bool DumpFrames
    {
      get { lock (this.Mutex) { return this.Viewer.DumpFrames; } }
      set { lock (this.Mutex) { this.Viewer.DumpFrames = value; } }
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
          return this.Viewer.frameDumpScale();
        }
      }
      set
      {
        lock (this.Mutex)
        {
          this.Viewer.frameDumpScale(value);
          CadKit.Persistence.Registry.Instance.setFloat(REGISTRY_SECTION, this.FRAME_DUMP_SCALE_KEY, value);
        }
      }
    }


    /// <summary>
    /// Called by the system when this object is about to be destroyed.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this.clear();
    }


    /// <summary>
    /// Take a single picture.
    /// </summary>
    void CadKit.Interfaces.ISnapShot.takePicture(string file, uint numRenderPasses, float frameSizeScale, float scatterScale)
    {
      this.takePicture(file, numRenderPasses, frameSizeScale, scatterScale);
    }


    /// <summary>
    /// Take a single picture.
    /// </summary>
    public void takePicture(string file, uint numRenderPasses, float frameSizeScale, float scatterScale)
    {
      lock (this.Mutex)
      {
        uint originalNumPasses = this.RenderingPasses;
        float originalFrameScale = this.FrameScale;
        double originalScatterScale = this.ScatterScale;
        try
        {
          this.RenderingPasses = numRenderPasses;
          this.FrameScale = frameSizeScale;
          this.ScatterScale = scatterScale;
          this.writeImageFile(file);
        }
        finally
        {
          this.RenderingPasses = originalNumPasses;
          this.FrameScale = originalFrameScale;
          this.ScatterScale = originalScatterScale;
        }
      }
    }
  }
}
