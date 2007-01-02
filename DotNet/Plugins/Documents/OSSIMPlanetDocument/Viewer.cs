
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Documents.OSSIMPlanetDocument
{
  public class Viewer :
    WeifenLuo.WinFormsUI.DockContent,
    CadKit.Interfaces.IViewer,
    CadKit.Interfaces.IDocumentView,
    CadKit.Interfaces.IClearColor,
    CadKit.Interfaces.IPropertyGridObject,
    CadKit.Interfaces.IRenderLoop
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.Panel _panel = new CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.Panel();
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
    /// Get/Set the scene.
    /// </summary>
    public object Scene
    {
      get { lock (this.Mutex) { return this.Panel.scene(); } }
      set { lock (this.Mutex) { this.Panel.scene( value as CadKit.OSG.Glue.Node ); } }
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
      lock (this.Mutex)
      {
      }
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
      get { lock (this.Mutex) { return (System.Drawing.Color) this.Panel.backgroundColor(); } }
      set { lock (this.Mutex) { this.Panel.backgroundColor ( value ); } }
    }


    /// <summary>
    /// Return object appropriate for property grid.
    /// </summary>
    public virtual object PropertyGridObject
    {
      get { lock (this.Mutex) { return null; /*this.Panel.PropertyGridObject;*/ } }
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
      get { lock (this.Mutex) { return this.Panel.RenderTimer; } }
      set
      {
        lock (this.Mutex)
        {
          this.Panel.RenderTimer = value;
        }
      }
    }


    /// <summary>
    /// Get/set the panel.
    /// </summary>
    public CadKit.Plugins.Documents.OSSIMPlanetDocument.Glue.Panel Panel
    {
      get { lock (this.Mutex) { return _panel; } }
      set { lock (this.Mutex) { _panel = value; } }
    }
  }
}
