
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
    CadKit.Interfaces.IJitterAntialias
  {
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
      this.Controls.Add(_panel);
      _panel.Dock = System.Windows.Forms.DockStyle.Fill;

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
      _panel = null;
    }


    /// <summary>
    /// Initalize.
    /// </summary>
    public void init()
    {
      lock (this.Mutex) { _panel.init(); }
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
          _panel.clear();
          _panel = null;
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
      get { lock (this.Mutex) { return _panel.Viewer; } }
    }


    /// <summary>
    /// Get/Set the scene.
    /// </summary>
    public object Scene
    {
      get { lock (this.Mutex) { return _panel.Viewer.Scene; } }
      set { lock (this.Mutex) { _panel.Viewer.Scene = value as CadKit.OSG.Glue.Node; } }
    }


    /// <summary>
    /// Get/Set view mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get { lock (this.Mutex) { return _panel.Mode; } }
      set { lock (this.Mutex) { _panel.Mode = value; } }
    }


    /// <summary>
    /// Start the render timer.
    /// </summary>
    /// <param name="interval"></param>
    public void startRenderTimer(int interval)
    {
      lock (this.Mutex) { _panel.startRenderTimer(interval); }
    }


    /// <summary>
    /// End the render timer.
    /// </summary>
    public void endRenderTimer()
    {
      lock (this.Mutex) { _panel.endRenderTimer(); }
    }


    /// <summary>
    /// Render the scene.
    /// </summary>
    public void render()
    {
      lock (this.Mutex) { _panel.render(); }
    }


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      lock (this.Mutex) { _panel.camera(option); }
    }


    /// <summary>
    /// Return the export filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportImage.Filters
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
          if (null != export)
            return export.Filters;
          return new CadKit.Interfaces.Filters();
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
        CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
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
      get { lock (this.Mutex) { return _panel.FrameScale; } }
      set { lock (this.Mutex) { _panel.FrameScale = value; } }
    }


    /// <summary>
    /// Return the export filters.
    /// </summary>
    CadKit.Interfaces.Filters CadKit.Interfaces.IExportScene.Filters
    {
      get
      {
        lock (this.Mutex)
        {
          CadKit.Interfaces.IExportScene export = _panel as CadKit.Interfaces.IExportScene;
          if (null != export)
            return export.Filters;
          return new CadKit.Interfaces.Filters();
        }
      }
    }


    /// <summary>
    /// Export the scene.
    /// </summary>
    void CadKit.Interfaces.IExportScene.export(string filename, CadKit.Interfaces.SceneExport.Option option)
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IExportScene export = _panel as CadKit.Interfaces.IExportScene;
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
      lock (this.Mutex)
      {
      }
    }


    /// <summary>
    /// Get/set the directory where frames are dumped.
    /// </summary>
    public string Directory
    {
      get { lock (this.Mutex) { return _panel.Directory; } }
      set { lock (this.Mutex) { _panel.Directory = value; } }
    }


    /// <summary>
    /// Get/set the base filename for frame dumping.
    /// </summary>
    public string BaseFilename
    {
      get { lock (this.Mutex) { return _panel.BaseFilename; } }
      set { lock (this.Mutex) { _panel.BaseFilename = value; } }
    }


    /// <summary>
    /// Get/set the extension for frame-dumped files.
    /// </summary>
    public string Extension
    {
      get { lock (this.Mutex) { return _panel.Extension; } }
      set { lock (this.Mutex) { _panel.Extension = value; } }
    }


    /// <summary>
    /// Get/set the flag for dumping frames.
    /// </summary>
    public bool DumpFrames
    {
      get { lock (this.Mutex) { return _panel.DumpFrames; } }
      set { lock (this.Mutex) { _panel.DumpFrames = value; } }
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
      get { lock (this.Mutex) { return _panel.ClearColor; } }
      set { lock (this.Mutex) { _panel.ClearColor = value; } }
    }


    /// <summary>
    /// Get/set the clear color corner.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer.Corners Corner
    {
      get { lock (this.Mutex) { return _panel.Corners; } }
      set { lock (this.Mutex) { _panel.Corners = value; } }
    }


    /// <summary>
    /// Return object appropriate for property grid.
    /// </summary>
    public virtual object PropertyGridObject
    {
      get { lock (this.Mutex) { return _panel.PropertyGridObject; } }
    }


    /// <summary>
    /// Take a single picture.
    /// </summary>
    void CadKit.Interfaces.ISnapShot.takePicture(string file, uint numRenderPasses, float frameSizeScale, float scatterScale)
    {
      if (null != _panel)
      {
        lock (this.Mutex)
        {
          _panel.takePicture(file, numRenderPasses, frameSizeScale, scatterScale);
        }
      }
    }


    /// <summary>
    /// Return available rendering passes.
    /// </summary>
    uint[] CadKit.Interfaces.IJitterAntialias.AvailableRenderingPasses
    {
      get { lock (this.Mutex) { return (null == _panel) ? null : _panel.AvailableRenderingPasses; } }
    }


    /// <summary>
    /// Return available rendering passes.
    /// </summary>
    public uint[] AvailableRenderingPasses
    {
      get { lock (this.Mutex) { return (null == _panel) ? null : _panel.AvailableRenderingPasses; } }
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
      get { lock (this.Mutex) { return (null == _panel) ? 1.0f : _panel.ScatterScale; } }
      set
      {
        if (null != _panel)
        {
          lock (this.Mutex)
          {
            _panel.ScatterScale = value;
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
      get { lock (this.Mutex) { return (null == _panel) ? 1 : _panel.RenderingPasses; } }
      set
      {
        if (null != _panel)
        {
          lock (this.Mutex)
          {
            _panel.RenderingPasses = value;
          }
        }
      }
    }
  }
}
