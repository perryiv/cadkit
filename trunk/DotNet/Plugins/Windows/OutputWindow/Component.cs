
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.OutputWindow
{
  public class Component : CadKit.Interfaces.IPlugin
  {
    /// <summary>
    /// Construct a component.
    /// </summary>
    private object _mutex = new object();

    /// <summary>
    /// Construct a component.
    /// </summary>
    public Component()
    {
    }

    /// <summary>
    /// Called when the plugin is loaded.
    /// </summary>
    void CadKit.Interfaces.IPlugin.startupNotify ( object caller )
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IMainForm mw = caller as CadKit.Interfaces.IMainForm;
        if (null != mw)
        {
          System.Windows.Forms.Form parent = mw.Form as System.Windows.Forms.Form;
          if (null != parent)
          {
            parent.Shown += this._parentShown;
          }
        }
      }
    }

    /// <summary>
    /// Called when parent form is shown.
    /// </summary>
    private void _parentShown(object sender, System.EventArgs args)
    {
      try
      {
        CadKit.Interfaces.IDockPanel getPanel = sender as CadKit.Interfaces.IDockPanel;
        WeifenLuo.WinFormsUI.DockPanel panel = (null != getPanel) ? (getPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel) : null;

        if (null != panel)
        {
          CadKit.Plugins.Windows.OutputWindow.Form form = new CadKit.Plugins.Windows.OutputWindow.Form();
          form.Text = "Output Window";
          panel.DocumentStyle = WeifenLuo.WinFormsUI.DocumentStyles.DockingWindow;
          form.DockState = WeifenLuo.WinFormsUI.DockState.Unknown;
          form.VisibleState = WeifenLuo.WinFormsUI.DockState.Unknown;
          form.Show(panel);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4206815434: {0}", e.Message);
      }
    }

    /// <summary>
    /// Get the name of this plugin.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return "Output Window"; } }
    }

    /// <summary>
    /// Get the plugin's description.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get
      {
        lock (_mutex)
        {
          string message = "Window that displays program messages. These messages are also found in: ";
          message += CadKit.Tools.RedirectOutput.Instance.FileName;
          return message;
        }
      }
    }
  }
}
