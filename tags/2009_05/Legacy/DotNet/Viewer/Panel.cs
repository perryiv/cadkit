
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
    CadKit.Interfaces.ISnapShot,
    CadKit.Interfaces.IDisplayListUse,
    CadKit.Interfaces.IPolygonMode,
    CadKit.Interfaces.IShadeModel,
    CadKit.Interfaces.ITextureEnvironment,
    CadKit.Interfaces.ITextureMode,
    CadKit.Interfaces.IRenderLoop
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
    private readonly string DATABASE_PAGER_PRE_COMPILE = "DatabasePagerPreCompile";
    private readonly string COLOR_CORNERS = "ColorCorners";
    private CadKit.Interfaces.DisplayListUseChangedDelegate _displayListUseChanged = null;
    private bool _leftButtonDown = false;
    private bool _middleButtonDown = false;
    private bool _rightButtonDown = false;

    /// <summary>
    /// Contructor.
    /// </summary>
    public Panel()
    {
      this.Viewer.mode(CadKit.Viewer.Glue.Viewer.ViewMode.NAVIGATION);
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
      try
      {
        this.Dispose();
        _viewer = null;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(System.String.Format("Error 1425051324: {0}", e.Message));
      }
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
      using (this.Lock.write())
      {
        this.Viewer.clear();
      }
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      if (false == this.InvokeRequired)
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
    /// Get/set the clear color corners.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer.Corners Corners
    {
      get { using (this.Lock.read()) { return this.Viewer.backgroundCorners(); } }
      set
      {
        using (this.Lock.write())
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
      get { using (this.Lock.read()) { return _viewer; } }
    }


    /// <summary>
    /// A key was pressed.
    /// </summary>
    /// <param name="e"></param>
    protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
    {
      try
      {
        if (false == this.InvokeRequired)
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
        if (false == this.InvokeRequired)
        {
          //bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          //bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          //bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          bool mouse = _leftButtonDown || _middleButtonDown || _rightButtonDown;
          if (false == mouse)
            return;

          CadKit.Viewer.Glue.Viewer.Type type = mouse ? CadKit.Viewer.Glue.Viewer.Type.DRAG : CadKit.Viewer.Glue.Viewer.Type.MOVE;

          if (null != this.Viewer)
          {
            this.Viewer.mouseMove(x, y, _leftButtonDown, _middleButtonDown, _rightButtonDown, type);
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
        if (false == this.InvokeRequired)
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
        if (false == this.InvokeRequired)
        {
          bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          if (null != this.Viewer)
          {
            this.Viewer.buttonRelease(x, y, left, middle, right, CadKit.Viewer.Glue.Viewer.Type.RELEASE);
          }

          if (left)
            _leftButtonDown = false;
          if (middle)
            _middleButtonDown = false;
          if (right)
            _rightButtonDown = false;
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
        if (false == this.InvokeRequired)
        {
          bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
          bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
          bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

          float x = e.Location.X;
          float y = this.Size.Height - e.Location.Y;

          if (null != this.Viewer)
          {
            this.Viewer.buttonPress(x, y, left, middle, right, CadKit.Viewer.Glue.Viewer.Type.PUSH);
          }

          if (left)
            _leftButtonDown = true;
          if (middle)
            _middleButtonDown = true;
          if (right)
            _rightButtonDown = true;
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
      if (false == this.InvokeRequired)
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
      if (false == this.InvokeRequired)
      {
        if (null != this.Viewer)
        {
          this.Viewer.resize(this.Size.Width, this.Size.Height);
        }
      }
    }


    /// <summary>
    /// Get/Set the mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get
      {
        using (this.Lock.read())
        {
          CadKit.Viewer.Glue.Viewer.ViewMode mode = this.Viewer.mode();
          return (CadKit.Interfaces.ViewMode)mode;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.mode((CadKit.Viewer.Glue.Viewer.ViewMode)value);
        }
        System.Windows.Forms.ContextMenuStrip strip = (value == CadKit.Interfaces.ViewMode.PICK) ? this.buildContextMenu() : null;
        using (this.Lock.write())
        {
          this.ContextMenuStrip = strip;
        }
      }
    }


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      if (false == this.InvokeRequired)
      {
        CadKit.Viewer.Glue.Viewer viewer = this.Viewer;
        if (null != viewer)
        {
          using (this.Lock.write())
          {
            viewer.camera(option);
          }
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
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.Add(new CadKit.Interfaces.Filter("JPEG Image (*.jpg)", "*.jpg"));
        filters.Add(new CadKit.Interfaces.Filter("PNG Image (*.png)", "*.png"));
        filters.Add(new CadKit.Interfaces.Filter("BMP Image (*.bmp)", "*.bmp"));
        return filters;
      }
    }


    /// <summary>
    /// Return appropriate filters.
    /// </summary>
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
      if (false == this.InvokeRequired) // Need multi-threaded way to dump images...
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
      get { return this.FrameScale; }
      set { this.FrameScale = value; }
    }


    /// <summary>
    /// Export the scene.
    /// </summary>
    void CadKit.Interfaces.IExportScene.export(string filename, CadKit.Interfaces.SceneExport.Option option)
    {
      using (this.Lock.read())
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
      get { return new CadKit.Viewer.Panel.PropertyProxy(this); }
    }


    /// <summary>
    /// Number of rendering passes property.
    /// </summary>
    public uint RenderingPasses
    {
      get { using (this.Lock.read()) { return this.Viewer.numRenderPasses(); } }
      set { using (this.Lock.write()) { this.Viewer.numRenderPasses(value); } }
    }


    /// <summary>
    /// Scatter scale.
    /// </summary>
    public double ScatterScale
    {
      get { using (this.Lock.read()) { return this.Viewer.scatterScale(); } }
      set { using (this.Lock.write()) { this.Viewer.scatterScale(value); } }
    }


    /// <summary>
    /// Available rendering passes.
    /// </summary>
    public System.UInt32[] AvailableRenderingPasses
    {
      get
      {
        using (this.Lock.read())
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
      get { using (this.Lock.read()) { return this.Viewer.Directory; } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.Directory = value;
        }
        CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_DIRECTORY_KEY, value);
      }
    }


    /// <summary>
    /// Get/set the filename used when making screen-dumps.
    /// </summary>
    public string BaseFilename
    {
      get { using (this.Lock.read()) { return this.Viewer.Filename; } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.Filename = value;
        }
        CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_FILENAME_KEY, value);
      }
    }


    /// <summary>
    /// Get/set the file extension used when making screen-dumps.
    /// </summary>
    public string Extension
    {
      get { using (this.Lock.read()) { return this.Viewer.Extension; } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.Extension = value;
        }
        CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, this.FRAME_DUMP_EXTENSION_KEY, value);
      }
    }


    /// <summary>
    /// Get/set the flag that says whether or not to dump frames.
    /// </summary>
    public bool DumpFrames
    {
      get { using (this.Lock.read()) { return this.Viewer.DumpFrames; } }
      set { using (this.Lock.write()) { this.Viewer.DumpFrames = value; } }
    }


    /// <summary>
    /// Get/set the size-scale to dump frames.
    /// </summary>
    public float FrameScale
    {
      get { using (this.Lock.read()) { return this.Viewer.frameDumpScale(); } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.frameDumpScale(value);
        }
        CadKit.Persistence.Registry.Instance.setFloat(REGISTRY_SECTION, this.FRAME_DUMP_SCALE_KEY, value);
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


    /// <summary>
    /// Set/get the use of display lists.
    /// </summary>
    bool CadKit.Interfaces.IDisplayListUse.UseDisplayLists
    {
      get { return this.UseDisplayLists; }
      set { this.UseDisplayLists = value; }
    }


    /// <summary>
    /// Set/get the use of display lists.
    /// </summary>
    public bool UseDisplayLists
    {
      get { using (this.Lock.read()) { return this.Viewer.useDisplayLists(); } }
      set
      {
        bool old = this.UseDisplayLists;
        using (this.Lock.write())
        {
          this.Viewer.useDisplayLists(value);
        }
        CadKit.Interfaces.DisplayListUseChangedDelegate changed = this.DisplayListUseChanged;
        if (null != changed)
        {
          changed(this, old, value);
        }
      }
    }


    /// <summary>
    /// Set/get the use of render loop.
    /// </summary>
    bool CadKit.Interfaces.IRenderLoop.UseRenderLoop
    {
      get { return this.UseRenderLoop; }
      set { this.UseRenderLoop = value; }
    }


    /// <summary>
    /// Set/get the use of display lists.
    /// </summary>
    public bool UseRenderLoop
    {
      get { return this.RenderTimer; }
      set { this.RenderTimer = value; }
    }


    /// <summary>
    /// Get/set the display-list-use-changed delegate.
    /// </summary>
    CadKit.Interfaces.DisplayListUseChangedDelegate CadKit.Interfaces.IDisplayListUse.DisplayListUseChanged
    {
      get { return this.DisplayListUseChanged; }
      set { this.DisplayListUseChanged = value; }
    }


    /// <summary>
    /// Get/set the display-list-use-changed delegate.
    /// </summary>
    public CadKit.Interfaces.DisplayListUseChangedDelegate DisplayListUseChanged
    {
      get { using (this.Lock.read()) { return _displayListUseChanged; } }
      set { using (this.Lock.write()) { _displayListUseChanged = value; } }
    }


    /// <summary>
    /// Get/set the polygon-mode face.
    /// </summary>
    CadKit.Interfaces.PolygonMode.Face CadKit.Interfaces.IPolygonMode.Face
    {
      get { return this.PolygonFace; }
      set { this.PolygonFace = value; }
    }


    /// <summary>
    /// Get/set the polygon-mode mode.
    /// </summary>
    CadKit.Interfaces.PolygonMode.Mode CadKit.Interfaces.IPolygonMode.Mode
    {
      get { return this.PolygonMode; }
      set { this.PolygonMode = value; }
    }


    /// <summary>
    /// Get/set the polygon-mode face.
    /// </summary>
    public CadKit.Interfaces.PolygonMode.Face PolygonFace
    {
      get { return CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK; } // For now.
      set { }
    }


    /// <summary>
    /// Get/set the polygon-mode mode.
    /// </summary>
    public CadKit.Interfaces.PolygonMode.Mode PolygonMode
    {
      get { using (this.Lock.read()) { return this.Viewer.polygonMode(CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK); } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.polygonMode(value, CadKit.Interfaces.PolygonMode.Face.FRONT_AND_BACK);
        }
        if (false == this.InvokeRequired)
        {
          this.Invalidate(true);
        }
      }
    }


    /// <summary>
    /// Set/get the shade model.
    /// </summary>
    CadKit.Interfaces.ShadeModel.Model CadKit.Interfaces.IShadeModel.Model
    {
      get { return this.ShadeModel; }
      set { this.ShadeModel = value; }
    }


    /// <summary>
    /// Set/get the shade model.
    /// </summary>
    public CadKit.Interfaces.ShadeModel.Model ShadeModel
    {
      get { using (this.Lock.read()) { return this.Viewer.shadeModel(); } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.shadeModel(value);
        }
        if (false == this.InvokeRequired)
        {
          this.Invalidate(true);
        }
      }
    }


    /// <summary>
    /// Set/get the texture environment.
    /// </summary>
    CadKit.Interfaces.TextureEnvironment.Mode CadKit.Interfaces.ITextureEnvironment.Mode
    {
      get { return this.TextureEnvironment; }
      set { this.TextureEnvironment = value; }
    }


    /// <summary>
    /// Set/get the texture environment.
    /// </summary>
    public CadKit.Interfaces.TextureEnvironment.Mode TextureEnvironment
    {
      get { using (this.Lock.read()) { return this.Viewer.textureEnvironment(); } }
      set
      {
        using (this.Lock.write())
        {
          this.Viewer.textureEnvironment(value);
          if (false == this.InvokeRequired)
          {
            this.Invalidate(true);
          }
        }
      }
    }


    /// <summary>
    /// Set the texture mode.
    /// </summary>
    void CadKit.Interfaces.ITextureMode.mode(CadKit.Interfaces.TextureMode.Mode mode, bool state)
    {
      this.textureMode(mode, state);
    }


    /// <summary>
    /// Get the texture mode.
    /// </summary>
    public void textureMode(CadKit.Interfaces.TextureMode.Mode mode, bool state)
    {
      using (this.Lock.write()) { this.Viewer.textureMode(mode, state); }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.mode(CadKit.Interfaces.TextureMode.Mode mode)
    {
      return this.textureMode(mode);
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool textureMode(CadKit.Interfaces.TextureMode.Mode mode)
    {
      using (this.Lock.read()) { return this.Viewer.textureMode(mode); }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.OneD
    {
      get { return this.TextureModeOneD; }
      set { this.TextureModeOneD = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool TextureModeOneD
    {
      get { using (this.Lock.read()) { return this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D1); } }
      set { using (this.Lock.write()) { this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D1, value); } }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.TwoD
    {
      get { return this.TextureModeTwoD; }
      set { this.TextureModeTwoD = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool TextureModeTwoD
    {
      get { using (this.Lock.read()) { return this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D2); } }
      set { using (this.Lock.write()) { this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D2, value); } }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.ThreeD
    {
      get { return this.TextureModeThreeD; }
      set { this.TextureModeThreeD = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool TextureModeThreeD
    {
      get { using (this.Lock.read()) { return this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D3); } }
      set { using (this.Lock.write()) { this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.D3, value); } }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.Rectangle
    {
      get { return this.TextureModeRectangle; }
      set { this.TextureModeRectangle = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool TextureModeRectangle
    {
      get { using (this.Lock.read()) { return this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.RECTANGLE); } }
      set { using (this.Lock.write()) { this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.RECTANGLE, value); } }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    bool CadKit.Interfaces.ITextureMode.CubeMap
    {
      get { return this.TextureModeCubeMap; }
      set { this.TextureModeCubeMap = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public bool TextureModeCubeMap
    {
      get { using (this.Lock.read()) { return this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.CUBE_MAP); } }
      set { using (this.Lock.write()) { this.Viewer.textureMode(CadKit.Interfaces.TextureMode.Mode.CUBE_MAP, value); } }
    }
  }
}
