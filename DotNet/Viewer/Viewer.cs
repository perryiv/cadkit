
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
    CadKit.Interfaces.IRenderLoop
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void DocumentChangedDelegate(object changed, object caller);


    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private CadKit.Viewer.Panel _panel = new CadKit.Viewer.Panel();
    private CadKit.Interfaces.IDocument _document = null;


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
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~Viewer()
    {
      this.Panel = null;
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      lock (this.Mutex) { this.Panel.init(); }
    }


    /// <summary>
    /// Called when the viewer is shown.
    /// </summary>
    private void _viewerShown(object sender, System.EventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          this.init();
          this.camera(CadKit.Interfaces.CameraOption.FIT);
        }
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
        lock (this.Mutex)
        {
          // Call this first because it circles back around and expects the panel to exist.
          if (null != this.Document)
          {
            this.Document.remove(this);
            this.Document = null;
          }

          // Now clear the panel.
          if (null != this.Panel)
          {
            this.Panel.clear();
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
      try
      {
        lock (this.Mutex)
        {
          CadKit.Documents.Manager.Instance.ActiveView = this as CadKit.Interfaces.IDocumentView;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3677938968: {0}", e.Message);
      }
    }


    /// <summary>
    /// Get the viewer.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer HeliosViewer
    {
      get { lock (this.Mutex) { return this.Panel.Viewer; } }
    }


    /// <summary>
    /// Get/Set the scene.
    /// </summary>
    public object Scene
    {
      get { lock (this.Mutex) { return this.Panel.Viewer.Scene; } }
      set { lock (this.Mutex) { this.Panel.Viewer.Scene = value as CadKit.OSG.Glue.Node; } }
    }


    /// <summary>
    /// Get/Set view mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get { lock (this.Mutex) { return this.Panel.Mode; } }
      set { lock (this.Mutex) { this.Panel.Mode = value; } }
    }


    /// <summary>
    /// Start the render timer.
    /// </summary>
    /// <param name="interval"></param>
    public void startRenderTimer(int interval)
    {
      lock (this.Mutex) { this.Panel.startRenderTimer(interval); }
    }


    /// <summary>
    /// End the render timer.
    /// </summary>
    public void endRenderTimer()
    {
      lock (this.Mutex) { this.Panel.endRenderTimer(); }
    }


    /// <summary>
    /// Render the scene.
    /// </summary>
    public void render()
    {
      lock (this.Mutex) { this.Panel.render(); }
    }


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      lock (this.Mutex) { this.Panel.camera(option); }
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
      lock (this.Mutex)
      {
        CadKit.Interfaces.IExportImage export = this.Panel as CadKit.Interfaces.IExportImage;
        if (null == export)
        {
          throw new System.Exception(System.String.Format("Error 1078988943: Failed to export image file '{0}', feature not supported", filename));
        }
        export.export(filename);
      }
    }


    /// <summary>
    /// Image export width.
    /// </summary>
    float CadKit.Interfaces.IExportImage.Scale
    {
      get { lock (this.Mutex) { return this.Panel.FrameScale; } }
      set { lock (this.Mutex) { this.Panel.FrameScale = value; } }
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
      lock (this.Mutex)
      {
        CadKit.Interfaces.IExportScene export = this.Panel as CadKit.Interfaces.IExportScene;
        if (null == export)
        {
          throw new System.Exception(System.String.Format("Error 1824824745: Failed to export scene file '{0}', feature not supported", filename));
        }
        export.export(filename, option);
      }
    }


    /// <summary>
    /// Get the mutex.
    /// </summary>
    public object Mutex
    {
      get { return _mutex; }
    }


    /// <summary>
    /// Get/set the document.
    /// </summary>
    public CadKit.Interfaces.IDocument Document
    {
      get { lock (this.Mutex) { return _document; } }
      set
      {
        lock (this.Mutex)
        {
          this._removeDocumentDelegates();
          _document = value;
          this._addDocumentDelegates();
        }
      }
    }


    /// <summary>
    /// Get/set the document.
    /// </summary>
    CadKit.Interfaces.IDocument CadKit.Interfaces.IDocumentView.Document
    {
      get { lock (this.Mutex) { return this.Document; } }
      set { lock (this.Mutex) { this.Document = value; } }
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
      lock (this.Mutex)
      {
        if (null != _document)
        {
          CadKit.Interfaces.INotifyChanged changed = _document as CadKit.Interfaces.INotifyChanged;
          if (null != changed)
          {
            changed.NotifyChanged += this._documentChanged;
          }
        }
      }
    }


    /// <summary>
    /// Remove existing document delegates.
    /// </summary>
    private void _removeDocumentDelegates()
    {
      lock (this.Mutex)
      {
        if (null != _document)
        {
          CadKit.Interfaces.INotifyChanged changed = _document as CadKit.Interfaces.INotifyChanged;
          if (null != changed)
          {
            changed.NotifyChanged -= this._documentChanged;
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
      get { lock (this.Mutex) { return this.Panel.Directory; } }
      set { lock (this.Mutex) { this.Panel.Directory = value; } }
    }


    /// <summary>
    /// Get/set the base filename for frame dumping.
    /// </summary>
    public string BaseFilename
    {
      get { lock (this.Mutex) { return this.Panel.BaseFilename; } }
      set { lock (this.Mutex) { this.Panel.BaseFilename = value; } }
    }


    /// <summary>
    /// Get/set the extension for frame-dumped files.
    /// </summary>
    public string Extension
    {
      get { lock (this.Mutex) { return this.Panel.Extension; } }
      set { lock (this.Mutex) { this.Panel.Extension = value; } }
    }


    /// <summary>
    /// Get/set the flag for dumping frames.
    /// </summary>
    public bool DumpFrames
    {
      get { lock (this.Mutex) { return this.Panel.DumpFrames; } }
      set { lock (this.Mutex) { this.Panel.DumpFrames = value; } }
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
      get { lock (this.Mutex) { return this.Panel.ClearColor; } }
      set { lock (this.Mutex) { this.Panel.ClearColor = value; } }
    }


    /// <summary>
    /// Get/set the clear color corner.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer.Corners Corner
    {
      get { lock (this.Mutex) { return this.Panel.Corners; } }
      set { lock (this.Mutex) { this.Panel.Corners = value; } }
    }


    /// <summary>
    /// Return object appropriate for property grid.
    /// </summary>
    public virtual object PropertyGridObject
    {
      get { lock (this.Mutex) { return this.Panel.PropertyGridObject; } }
    }


    /// <summary>
    /// Take a single picture.
    /// </summary>
    void CadKit.Interfaces.ISnapShot.takePicture(string file, uint numRenderPasses, float frameSizeScale, float scatterScale)
    {
      if (null != this.Panel)
      {
        lock (this.Mutex)
        {
          this.Panel.takePicture(file, numRenderPasses, frameSizeScale, scatterScale);
        }
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
      get { lock (this.Mutex) { return (null == this.Panel) ? null : this.Panel.AvailableRenderingPasses; } }
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
      get { lock (this.Mutex) { return (null == this.Panel) ? 1.0f : this.Panel.ScatterScale; } }
      set
      {
        if (null != this.Panel)
        {
          lock (this.Mutex)
          {
            this.Panel.ScatterScale = value;
          }
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
      get { lock (this.Mutex) { return (null == this.Panel) ? 1 : this.Panel.RenderingPasses; } }
      set
      {
        if (null != this.Panel)
        {
          lock (this.Mutex)
          {
            this.Panel.RenderingPasses = value;
          }
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
      get { lock (this.Mutex) { return this.Panel.UseDisplayLists; } }
      set { lock (this.Mutex) { this.Panel.UseDisplayLists = value; } }
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
      get { lock (this.Mutex) { return this.Panel.UseRenderLoop; } }
      set { lock (this.Mutex) { this.Panel.UseRenderLoop = value; } }
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
      get { lock (this.Mutex) { return this.Panel.DisplayListUseChanged; } }
      set { lock (this.Mutex) { this.Panel.DisplayListUseChanged = value; } }
    }


    /// <summary>
    /// Get/set the panel.
    /// </summary>
    public CadKit.Viewer.Panel Panel
    {
      get { lock (this.Mutex) { return _panel; } }
      set { lock (this.Mutex) { _panel = value; } }
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
      get { lock (this.Mutex) { return this.Panel.PolygonFace; } }
      set { lock (this.Mutex) { this.Panel.PolygonFace = value; } }
    }


    /// <summary>
    /// Get/set the polygon-mode mode.
    /// </summary>
    public CadKit.Interfaces.PolygonMode.Mode PolygonMode
    {
      get { lock (this.Mutex) { return this.Panel.PolygonMode; } }
      set { lock (this.Mutex) { this.Panel.PolygonMode = value; } }
    }


    /// <summary>
    /// Get/Set the database pager.
    /// </summary>
    public object DatabasePager
    {
      get { lock (this.Mutex) { return this.Panel.Viewer.DatabasePager; } }
      set { lock (this.Mutex) { this.Panel.Viewer.DatabasePager = value as CadKit.OSG.Glue.DatabasePager; } }
    }


    /// <summary>
    /// Get/Set the matrix manipulator.
    /// </summary>
    public object MatrixManipulator
    {
      get { lock (this.Mutex) { return this.Panel.Viewer.MatrixManipulator; } }
      set { lock (this.Mutex) { this.Panel.Viewer.MatrixManipulator = value as CadKit.OSG.Glue.MatrixManipulator; } }
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
      get { lock (this.Mutex) { return this.Panel.ShadeModel; } }
      set { lock (this.Mutex) { this.Panel.ShadeModel = value; } }
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
      get { lock (this.Mutex) { return this.Panel.TextureEnvironment; } }
      set { lock (this.Mutex) { this.Panel.TextureEnvironment = value; } }
    }
  }
}
