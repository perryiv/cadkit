
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
    CadKit.Interfaces.IExportScene
  {
    CadKit.Viewer.Panel _panel = new Panel();

    /// <summary>
    /// Constructor.
    /// </summary>
    public Viewer()
    {
      this.Controls.Add(_panel);
      _panel.Dock = System.Windows.Forms.DockStyle.Fill;

      this.DockableAreas = WeifenLuo.WinFormsUI.DockAreas.Document | WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.Float;

      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(OnFormClosed);
      this.Shown += new System.EventHandler(Viewer_Shown);
    }


    void Viewer_Shown(object sender, System.EventArgs e)
    {
      this.init();
    }


    /// <summary>
    /// The form is closed.  Clean up.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnFormClosed(object sender, System.Windows.Forms.FormClosedEventArgs e)
    {
      _panel.clear();
      _panel = null;
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
      _panel.init();
    }


    /// <summary>
    /// Get the viewer.
    /// </summary>
    public CadKit.Viewer.Glue.Viewer HeliosViewer
    {
      get
      {
        return _panel.Viewer;
      }
    }


    /// <summary>
    /// Get/Set the scene.
    /// </summary>
    public object Scene
    {
      get
      {
        return _panel.Viewer.Scene;
      }
      set
      {
        _panel.Viewer.Scene = value as CadKit.OSG.Glue.Node;
      }
    }


    /// <summary>
    /// Get/Set view mode.
    /// </summary>
    public CadKit.Interfaces.ViewMode Mode
    {
      get
      {
        return _panel.Mode;
      }
      set
      {
        _panel.Mode = value;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="interval"></param>
    public void startRenderTimer(int interval)
    {
      _panel.startRenderTimer(interval);
    }


    /// <summary>
    /// 
    /// </summary>
    public void endRenderTimer()
    {
      _panel.endRenderTimer();
    }


    /// <summary>
    /// 
    /// </summary>
    public void render()
    {
      _panel.render();
    }


    /// <summary>
    /// Set the camera.
    /// </summary>
    public void camera(CadKit.Interfaces.CameraOption option)
    {
      _panel.camera(option);
    }

    CadKit.Interfaces.Filters CadKit.Interfaces.IExportImage.Filters
    {
      get 
      { 
        CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
        if(null != export)
          return export.Filters;
        return new CadKit.Interfaces.Filters();
      }
    }

    bool CadKit.Interfaces.IExportImage.exportImage(string filename)
    {
      CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
      if (null != export)
        return export.exportImage(filename);
      return false;
    }

    bool CadKit.Interfaces.IExportImage.exportImage(string filename, int width, int height)
    {
      CadKit.Interfaces.IExportImage export = _panel as CadKit.Interfaces.IExportImage;
      if (null != export)
        return export.exportImage(filename, width, height);
      return false;
    }

    CadKit.Interfaces.Filters CadKit.Interfaces.IExportScene.Filters
    {
      get 
      {
        CadKit.Interfaces.IExportScene export = _panel as CadKit.Interfaces.IExportScene;
        if (null != export)
          return export.Filters;
        return new CadKit.Interfaces.Filters(); 
      }
    }

    bool CadKit.Interfaces.IExportScene.exportScene(string filename)
    {
      CadKit.Interfaces.IExportScene export = _panel as CadKit.Interfaces.IExportScene;
      if (null != export)
        return export.exportScene(filename);
      return false;
    }
  }
}
