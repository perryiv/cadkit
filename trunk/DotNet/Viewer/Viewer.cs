
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
    CadKit.Interfaces.IFrameDump
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
    private void _viewerShown(object sender, System.EventArgs e)
    {
      lock (this.Mutex)
      {
        this.init();
        this.camera(CadKit.Interfaces.CameraOption.FIT);
      }
    }


    /// <summary>
    /// The form is closed.  Clean up.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _formClosed(object sender, System.Windows.Forms.FormClosedEventArgs e)
    {
      lock (this.Mutex)
      {
        _panel.clear();
        _panel = null;
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
    bool CadKit.Interfaces.IExportImage.exportImage(string filename)
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
        if (null != export)
          return export.exportImage(filename);
        return false;
      }
    }


    /// <summary>
    /// Image export width.
    /// </summary>
    uint CadKit.Interfaces.IExportImage.Width
    {
      get { lock (this.Mutex) { return (uint)System.Math.Abs(_panel.FrameSize.Width); } }
      set { lock (this.Mutex) { _panel.FrameSize = new System.Drawing.Size((int)value, _panel.FrameSize.Height); } }
    }


    /// <summary>
    /// Image export height.
    /// </summary>
    uint CadKit.Interfaces.IExportImage.Height
    {
      get { lock (this.Mutex) { return (uint)System.Math.Abs(_panel.FrameSize.Height); } }
      set { lock (this.Mutex) { _panel.FrameSize = new System.Drawing.Size(_panel.FrameSize.Width, (int)value); } }
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
    /// Exposr the scene.
    /// </summary>
    bool CadKit.Interfaces.IExportScene.exportScene(string filename)
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IExportScene export = _panel as CadKit.Interfaces.IExportScene;
        if (null != export)
          return export.exportScene(filename);
        return false;
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

    public string Directory
    {
      get
      {
        return _panel.Directory;
      }
      set
      {
        _panel.Directory = value;
      }
    }

    public string Filename
    {
      get
      {
        return _panel.Filename;
      }
      set
      {
        _panel.Filename = value;
      }
    }

    public string Extension
    {
      get
      {
        return _panel.Extension;
      }
      set
      {
        _panel.Extension = value;
      }
    }

    public bool DumpFrames
    {
      get
      {
        return _panel.DumpFrames;
      }
      set
      {
        _panel.DumpFrames = value;
      }
    }
  }
}
