
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
    CadKit.Interfaces.IViewer
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

      this.BackColorChanged += new System.EventHandler(_panel.OnBackColorChanged);
      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(OnFormClosed);
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
  }
}
