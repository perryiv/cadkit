
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.SceneDelegate
{
  public class Delegate : 
    CadKit.Delegates.Delegate,
    CadKit.Interfaces.IGuiDelegate
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Scene Delegate";

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

        // Make new viewer.
        CadKit.Viewer.Viewer view = new CadKit.Viewer.Viewer();
        view.Icon = System.Windows.Forms.Application.OpenForms[0].Icon;
        view.Text = this.Document.Name;

        // Attach viewer and document to each other.
        view.Document = this.Document;
        view.Document.add(view);

        // Build the scene.
        CadKit.Interfaces.IBuildScene buildScene = this.Document as CadKit.Interfaces.IBuildScene;
        if (null != buildScene)
        {
          view.Scene = buildScene.Scene;
        }

        // Get dock-panel and show.
        CadKit.Interfaces.IDockPanel getPanel = caller as CadKit.Interfaces.IDockPanel;
        WeifenLuo.WinFormsUI.DockPanel panel = (null != getPanel) ? (getPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel) : null;
        if (null != panel)
        {
          if (panel.DocumentStyle == WeifenLuo.WinFormsUI.DocumentStyles.SystemMdi)
          {
            view.MdiParent = caller as System.Windows.Forms.Form;
            CadKit.Tools.Size.mdiChild(panel.DocumentRectangle.Size,view);
            view.Show();
          }
          else
          {
            view.Show(panel);
          }
        }
      }
    }

    /// <summary>
    /// Returns true if the document type is handled.
    /// </summary>
    bool CadKit.Interfaces.IGuiDelegate.doesHandle ( string type )
    {
      lock (this.Mutex)
      {
        return (type == "Scene Document");
      }
    }
  }
}
