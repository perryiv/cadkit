
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Viewer
{
  public class Viewer :
    WeifenLuo.WinFormsUI.DockContent,
    CadKit.Interfaces.IViewer,
    CadKit.Interfaces.IViewerMode,
    CadKit.Interfaces.ICamera,
    CadKit.Interfaces.IExportImage,
    CadKit.Interfaces.IExportScene,
    CadKit.Interfaces.IFrameDump,
    CadKit.Interfaces.IDocumentView,
    CadKit.Interfaces.IClearColor,
    CadKit.Interfaces.IPropertyGridObject,
    CadKit.Interfaces.ISnapShot,
    CadKit.Interfaces.IJitterAntialias,
    CadKit.Interfaces.IDisplayListUse,
    CadKit.Interfaces.IPolygonMode,
    CadKit.Interfaces.IShadeModel,
    CadKit.Interfaces.ITextureEnvironment,
    CadKit.Interfaces.ITextureMode,
    CadKit.Interfaces.IRenderLoop
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void DocumentChangedDelegate(object changed, object caller);
    public class DocumentHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.IDocument> { }
    public class NotifyChangedHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.INotifyChanged>
    {
      public NotifyChangedHandle(CadKit.Interfaces.INotifyChanged notify) : base(notify) { }
    }


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = null;
    private CadKit.Viewer.Panel _panel = new CadKit.Viewer.Panel();
    private DocumentHandle _document = new DocumentHandle();


    /// <summary>
    /// Constructor.
    /// </summary>
    public Viewer()
    {
      this.Controls.Add(this.Panel);
      this.Panel.Dock = System.Windows.Forms.DockStyle.Fill;

      this.DockableAreas = WeifenLuo.WinFormsUI.DockAreas.Document | WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.Float;

      this.FormClosed += this._formClosed;
      this.Shown += this._viewerShown;
      this.Activated += this._viewerActivated;
      this.Shown += new System.EventHandler(_onViewerShown);
      this.Panel.Click += new System.EventHandler(_onPanelClick);
    }


    /// <summary>
    /// Make this view active.
    /// </summary>
    private void _makeActiveView()
    {
      try
      {
        if (CadKit.Documents.Manager.Instance.ActiveView != this)
        {
          CadKit.Documents.Manager.Instance.ActiveView = this as CadKit.Interfaces.IDocumentView;
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 3677938968: {0}", ex.Message);
      }
    }


    /// <summary>
    /// Make this view active.
    /// </summary>
    void _onPanelClick(object sender, System.EventArgs e)
    {
      this._makeActiveView();
    }


    /// <summary>
    /// Make this view active,
    /// </summary>
    void _onViewerShown(object sender, System.EventArgs e)
    {
      this._makeActiveView();
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Viewer()
    {
      try
      {
        this.Panel = null;

        if (true == _document.Valid)
        {
          _document.Value.remove(this);
          _document.Value = null;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(System.String.Format("Error 3341185068: {0}", e.Message));
      }
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      if (false == this.InvokeRequired)
      {
        this.Panel.init();
      }
    }


    /// <summary>
    /// Called when the viewer is shown.
    /// </summary>
    private void _viewerShown(object sender, System.EventArgs args)
    {
      try
      {
        this.init();
        this.camera(CadKit.Interfaces.CameraOption.FIT);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4168059452: {0}", e.Message);
      }
    }


    /// <summary>
    /// The form is closed.  Clean up.
    /// </summary>
    private void _formClosed(object sender, System.Windows.Forms.FormClosedEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          if (this == CadKit.Documents.Manager.Instance.ActiveView)
            CadKit.Documents.Manager.Instance.ActiveView = null;

          CadKit.Interfaces.IDocument document = this.Document;
          CadKit.Viewer.Panel panel = this.Panel;

          // Call this first because it circles back around and expects the panel to exist.
          if (null != document)
          {
            document.remove(this);
            this.Document = null;
          }

          // Now clear the panel.
          if (null != panel)
          {
            panel.clear();
            this.Panel = null;
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 6554359560: {0}", e.Message);
      }
    }


    /// <summary>
    /// The form is activated.
    /// </summary>
    void _viewerActivated(object sender, System.EventArgs args)
    {
      this._makeActiveView();
    }


    /// <summary>
    /// Get the viewer.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer HeliosViewer
    {
      get { using (this.Lock.read()) { return this.Panel.Viewer; } }
    }


    /// <summary>
    /// Get/Set the scene.
    /// </summary>
    public object Scene
    {
      get { using (this.Lock.read()) { return this.Panel.Viewer.Scene; } }
      set { using (this.Lock.write()) { this.Panel.Viewer.Scene = value as CadKit.OSG.Glue.Node; } }
    }


    /// <summary>
    /// Render.
    /// </summary>
    void CadKit.Interfaces.IViewer.render()
    {
      this.render();
    }


    /// <summary>
    /// Get/Set view mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get { return this.Panel.Mode; }
      set { this.Panel.Mode = value; }
    }


    /// <summary>
    /// Start the render timer.
    /// </summary>
    /// <param name="interval"></param>
    public void startRenderTimer(int interval)
    {
      this.Panel.startRenderTimer(interval);
    }


    /// <summary>
    /// End the render timer.
    /// </summary>
    public void endRenderTimer()
    {
      this.Panel.endRenderTimer();
    }


    /// <summary>
    /// Render the scene.
    /// </summary>
    public void render()
    {
      this.Panel.render();
    }


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      this.Panel.camera(option);
    }


    /// <summary>
    /// Return the export filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportImage.Filters
    {
      get
      {
        CadKit.Interfaces.IExportImage export = this.Panel as CadKit.Interfaces.IExportImage;
        return (null == export) ? new CadKit.Interfaces.Filters() : export.Filters;
      }
    }


    /// <summary>
    /// Export the image.
    /// </summary>
    void CadKit.Interfaces.IExportImage.export(string filename)
    {
      CadKit.Interfaces.IExportImage export = this.Panel as CadKit.Interfaces.IExportImage;
      if (null == export)
      {
        throw new System.Exception(System.String.Format("Error 1078988943: Failed to export image file '{0}', feature not supported", filename));
      }
      export.export(filename);
    }


    /// <summary>
    /// Image export width.
    /// </summary>
    float CadKit.Interfaces.IExportImage.Scale
    {
      get { return this.Panel.FrameScale; }
      set { this.Panel.FrameScale = value; }
    }


    /// <summary>
    /// Return the export filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportScene.Filters
    {
      get
      {
        CadKit.Interfaces.IExportScene export = this.Panel as CadKit.Interfaces.IExportScene;
        return (null == export) ? new CadKit.Interfaces.Filters() : export.Filters;
      }
    }


    /// <summary>
    /// Export the scene.
    /// </summary>
    void CadKit.Interfaces.IExportScene.export(string filename, CadKit.Interfaces.SceneExport.Option option)
    {
      CadKit.Interfaces.IExportScene export = this.Panel as CadKit.Interfaces.IExportScene;
      if (null == export)
      {
        throw new System.Exception(System.String.Format("Error 1824824745: Failed to export scene file '{0}', feature not supported", filename));
      }
      export.export(filename, option);
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    public CadKit.Threads.Tools.Lock Lock
    {
      get
      {
        // If this gets called from the finalizer then the lock may have 
        // already been destroyed and set to null.
        if (null == _lock)
        {
          _lock = new CadKit.Threads.Tools.Lock();
        }
        return _lock;
      }
    }


    /// <summary>
    /// Get/set the document.
    /// </summary>
    public CadKit.Interfaces.IDocument Document
    {
      get
      {
        using (this.Lock.read())
        {
          return _document.Value;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          this._removeDocumentDelegates();
          _document.Value = value;
          this._addDocumentDelegates();
        }
      }
    }


    /// <summary>
    /// Get/set the document.
    /// </summary>
    CadKit.Interfaces.IDocument CadKit.Interfaces.IDocumentView.Document
    {
      get { return this.Document; }
      set { this.Document = value; }
    }


    /// <summary>
    /// Get/set the document.
    /// </summary>
    void CadKit.Interfaces.IDocumentView.close()
    {
      try
      {
        this.Close();
        if (this == CadKit.Documents.Manager.Instance.ActiveView)
        {
          CadKit.Documents.Manager.Instance.ActiveView = null;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3636387781: {0}", e.Message);
      }
    }


    /// <summary>
    /// Add new document delegates.
    /// </summary>
    private void _addDocumentDelegates()
    {
      using (this.Lock.write())
      {
        if (true == _document.Valid)
        {
          using (NotifyChangedHandle changed = new NotifyChangedHandle(_document.Value as CadKit.Interfaces.INotifyChanged))
          {
            if (true == changed.Valid)
            {
              changed.Value.NotifyChanged += this._documentChanged;
            }
          }
        }
      }
    }


    /// <summary>
    /// Remove existing document delegates.
    /// </summary>
    private void _removeDocumentDelegates()
    {
      using (this.Lock.write())
      {
        if (true == _document.Valid)
        {
          using (NotifyChangedHandle changed = new NotifyChangedHandle(_document.Value as CadKit.Interfaces.INotifyChanged))
          {
            if (true == changed.Valid)
            {
              changed.Value.NotifyChanged -= this._documentChanged;
            }
          }
        }
      }
    }


    /// <summary>
    /// Called when the document changed.
    /// </summary>
    private void _documentChanged(object changed, object caller)
    {
      if (true == this.InvokeRequired)
      {
        this.BeginInvoke(new DocumentChangedDelegate(this._documentChanged), new object[] { changed, caller });
      }
      else
      {
        this.Invalidate(true);
      }
    }


    /// <summary>
    /// Get/set the directory where frames are dumped.
    /// </summary>
    public string Directory
    {
      get { return this.Panel.Directory; }
      set { this.Panel.Directory = value; }
    }


    /// <summary>
    /// Get/set the base filename for frame dumping.
    /// </summary>
    public string BaseFilename
    {
      get { return this.Panel.BaseFilename; }
      set { this.Panel.BaseFilename = value; }
    }


    /// <summary>
    /// Get/set the extension for frame-dumped files.
    /// </summary>
    public string Extension
    {
      get { return this.Panel.Extension; }
      set { this.Panel.Extension = value; }
    }


    /// <summary>
    /// Get/set the flag for dumping frames.
    /// </summary>
    public bool DumpFrames
    {
      get { return this.Panel.DumpFrames; }
      set { this.Panel.DumpFrames = value; }
    }


    /// <summary>
    /// Get/set the background color.
    /// </summary>
    object CadKit.Interfaces.IClearColor.ClearColor
    {
      get { return this.ClearColor; }
      set
      {
        try
        {
          this.ClearColor = (System.Drawing.Color)value;
        }
        catch (System.InvalidCastException)
        {
        }
      }
    }


    /// <summary>
    /// Get/set the clear color.
    /// </summary>
    public System.Drawing.Color ClearColor
    {
      get { return this.Panel.ClearColor; }
      set { this.Panel.ClearColor = value; }
    }


    /// <summary>
    /// Get/set the clear color corner.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer.Corners Corner
    {
      get { return this.Panel.Corners; }
      set { this.Panel.Corners = value; }
    }


    /// <summary>
    /// Return object appropriate for property grid.
    /// </summary>
    public virtual object PropertyGridObject
    {
      get { return this.Panel.PropertyGridObject; }
    }


    /// <summary>
    /// Take a single picture.
    /// </summary>
    void CadKit.Interfaces.ISnapShot.takePicture(string file, uint numRenderPasses, float frameSizeScale, float scatterScale)
    {
      if (null != this.Panel)
      {
        this.Panel.takePicture(file, numRenderPasses, frameSizeScale, scatterScale);
      }
    }


    /// <summary>
    /// Return available rendering passes.
    /// </summary>
    uint[] CadKit.Interfaces.IJitterAntialias.AvailableRenderingPasses
    {
      get { return this.AvailableRenderingPasses; }
    }


    /// <summary>
    /// Return available rendering passes.
    /// </summary>
    public uint[] AvailableRenderingPasses
    {
      get { return (null == this.Panel) ? null : this.Panel.AvailableRenderingPasses; }
    }


    /// <summary>
    /// Return jitter scatter scale.
    /// </summary>
    double CadKit.Interfaces.IJitterAntialias.ScatterScale
    {
      get { return this.ScatterScale; }
      set { this.ScatterScale = value; }
    }


    /// <summary>
    /// Return jitter scatter scale.
    /// </summary>
    public double ScatterScale
    {
      get { return (null == this.Panel) ? 1.0f : this.Panel.ScatterScale; }
      set
      {
        if (null != this.Panel)
        {
          this.Panel.ScatterScale = value;
        }
      }
    }


    /// <summary>
    /// Return jitter scatter scale.
    /// </summary>
    uint CadKit.Interfaces.IJitterAntialias.RenderingPasses
    {
      get { return this.RenderingPasses; }
      set { this.RenderingPasses = value; }
    }


    /// <summary>
    /// Return jitter scatter scale.
    /// </summary>
    public uint RenderingPasses
    {
      get { return (null == this.Panel) ? 1 : this.Panel.RenderingPasses; }
      set
      {
        if (null != this.Panel)
        {
          this.Panel.RenderingPasses = value;
        }
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
      get { return this.Panel.UseDisplayLists; }
      set { this.Panel.UseDisplayLists = value; }
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
      get { return this.Panel.UseRenderLoop; }
      set { this.Panel.UseRenderLoop = value; }
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
    CadKit.Interfaces.DisplayListUseChangedDelegate DisplayListUseChanged
    {
      get { return this.Panel.DisplayListUseChanged; }
      set { this.Panel.DisplayListUseChanged = value; }
    }


    /// <summary>
    /// Get/set the panel.
    /// </summary>
    public CadKit.Viewer.Panel Panel
    {
      get { using (this.Lock.read()) { return _panel; } }
      set { using (this.Lock.write()) { _panel = value; } }
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
      get { return this.Panel.PolygonFace; }
      set { this.Panel.PolygonFace = value; }
    }


    /// <summary>
    /// Get/set the polygon-mode mode.
    /// </summary>
    public CadKit.Interfaces.PolygonMode.Mode PolygonMode
    {
      get { return this.Panel.PolygonMode; }
      set { this.Panel.PolygonMode = value; }
    }


    /// <summary>
    /// Get/Set the matrix manipulator.
    /// </summary>
    public object MatrixManipulator
    {
      get { using (this.Lock.read()) { return this.Panel.Viewer.MatrixManipulator; } }
      set { using (this.Lock.write()) { this.Panel.Viewer.MatrixManipulator = value as CadKit.OSG.Glue.MatrixManipulator; } }
    }


    /// <summary>
    /// Should the near far clipping planes be computed?
    /// </summary>
    /// <param name="b"></param>
    public void computeNearFar(bool b)
    {
      this.Panel.Viewer.computeNearFar(b);
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
      get { return this.Panel.ShadeModel; }
      set { this.Panel.ShadeModel = value; }
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
      get { return this.Panel.TextureEnvironment; }
      set { this.Panel.TextureEnvironment = value; }
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    void CadKit.Interfaces.ITextureMode.mode(CadKit.Interfaces.TextureMode.Mode mode, bool state)
    {
      this.textureMode(mode, state);
    }


    /// <summary>
    /// Set/get the texture mode.
    /// </summary>
    public void textureMode(CadKit.Interfaces.TextureMode.Mode mode, bool state)
    {
      this.Panel.textureMode(mode, state);
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
      return this.Panel.textureMode(mode);
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
      get { return this.Panel.TextureModeOneD; }
      set { this.Panel.TextureModeOneD = value; }
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
      get { return this.Panel.TextureModeTwoD; }
      set { this.Panel.TextureModeTwoD = value; }
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
      get { return this.Panel.TextureModeThreeD; }
      set { this.Panel.TextureModeThreeD = value; }
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
      get { return this.Panel.TextureModeRectangle; }
      set { this.Panel.TextureModeRectangle = value; }
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
      get { return this.Panel.TextureModeCubeMap; }
      set { this.Panel.TextureModeCubeMap = value; }
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.reference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference(bool allowCleanup)
    {
      // TODO
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    uint CadKit.Interfaces.IReferenced.RefCount
    {
      get { return 0; } // TODO
    }
  }
}
