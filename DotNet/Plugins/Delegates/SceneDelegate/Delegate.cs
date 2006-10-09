
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.SceneDelegate
{
  public class Delegate : CadKit.Interfaces.IGuiDelegate
  {
    /// <summary>
    /// Constants
    /// </summary>
    public const string TypeName = "Scene Delegate";

    /// <summary>
    /// Data members.
    /// </summary>
    CadKit.Interfaces.IDocument _document = null;
    private object _mutex = new object();

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
      get { lock (_mutex) { return _document; } }
      set { lock (_mutex) { _document = value as CadKit.Interfaces.IDocument; } }
    }

    /// <summary>
    /// Create the default gui.
    /// </summary>
    void CadKit.Interfaces.IGuiDelegate.create(object caller)
    {
      lock (_mutex)
      {
        CadKit.Viewer.Viewer view = new CadKit.Viewer.Viewer();
        view.Icon = System.Windows.Forms.Application.OpenForms[0].Icon;
        view.Text = _document.Name;

        CadKit.Interfaces.IBuildScene buildScene = _document as CadKit.Interfaces.IBuildScene;

        if( null != buildScene )
        {
          view.Scene = buildScene.Scene;
        }

        CadKit.Interfaces.IDockPanel getPanel = caller as CadKit.Interfaces.IDockPanel;
        WeifenLuo.WinFormsUI.DockPanel panel = (null != getPanel) ? (getPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel) : null;

        if (null != panel && panel.DocumentStyle == WeifenLuo.WinFormsUI.DocumentStyles.SystemMdi)
        {
          view.MdiParent = caller as System.Windows.Forms.Form;
          view.Show();
        }
        else
        {
          view.Show(panel);
        }

        view.init();
      }
    }

    /// <summary>
    /// Returns true if the document type is handled.
    /// </summary>
    bool CadKit.Interfaces.IGuiDelegate.doesHandle ( string type )
    {
      lock (_mutex)
      {
        return (type == "Scene Document");
      }
    }
  }
}
