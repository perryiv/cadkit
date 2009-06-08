
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.SimFlowDelegate
{
  public class Delegate : 
    CadKit.Delegates.Delegate,
    CadKit.Interfaces.IGuiDelegate
  {
      private CadKit.Viewer.Viewer _view = null;

    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "SimFlow Delegate";

    /// <summary>
    /// Construct a delegate.
    /// </summary>
    public Delegate()
    {
    }

    /// <summary>
    /// Set the document.
    /// </summary>
    object CadKit.Interfaces.IGuiDelegate.Document
    {
      get { lock (this.Mutex) { return this.Document; } }
      set { lock (this.Mutex) { this.Document = value as CadKit.Interfaces.IDocument; } }
    }

    /// <summary>
    /// Create the default gui.
    /// </summary>
    void CadKit.Interfaces.IGuiDelegate.create(object caller)
    {
      lock (this.Mutex)
      {
        if (null == this.Document)
          return;

        _view = new CadKit.Viewer.Viewer();
        _view.Icon = System.Windows.Forms.Application.OpenForms[0].Icon;
        _view.Text = this.Document.Name;
        _view.KeyDown += new System.Windows.Forms.KeyEventHandler(_view_KeyDown);

        // Attach viewer and document to each other.
        _view.Document = this.Document;
        _view.Document.add(_view);

        CadKit.Interfaces.IUpdateScene update = this.Document as CadKit.Interfaces.IUpdateScene;

        if (null != update)
          _view.Panel.BeginPaintEvent += update.updateScene;
        //view.FormClosed += this._viewClosed;

        CadKit.Interfaces.IBuildScene buildScene = this.Document as CadKit.Interfaces.IBuildScene;
        if (null != buildScene)
        {
          _view.Scene = buildScene.Scene;
        }

        CadKit.Interfaces.IDockPanel getPanel = caller as CadKit.Interfaces.IDockPanel;
        WeifenLuo.WinFormsUI.DockPanel panel = (null != getPanel) ? (getPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel) : null;

        if (null != panel)
        {
          if (panel.DocumentStyle == WeifenLuo.WinFormsUI.DocumentStyles.SystemMdi)
          {
            _view.MdiParent = caller as System.Windows.Forms.Form;
            CadKit.Tools.Size.mdiChild(panel.DocumentRectangle.Size,_view);
            _view.Show();
          }
          else
          {
            _view.Show(panel);
          }
        }
      }
    }

    void _view_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
    {
      if (e.KeyCode == System.Windows.Forms.Keys.S)
      {
        _view.startRenderTimer(15);
      }

      if (e.KeyCode == System.Windows.Forms.Keys.P)
      {
        _view.endRenderTimer();
      }
    }

    /// <summary>
    /// Returns true if the document type is handled.
    /// </summary>
    bool CadKit.Interfaces.IGuiDelegate.doesHandle ( string type )
    {
      lock (this.Mutex)
      {
        return (type == "SimFlow Document");
      }
    }
  }
}
