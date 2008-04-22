
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.SnapShot
{
  public class Component :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IPlugin
  {
    /// <summary>
    /// Construct a component.
    /// </summary>
    public Component()
    {
    }

    /// <summary>
    /// Called when the plugin is loaded.
    /// </summary>
    void CadKit.Interfaces.IPlugin.start(object caller)
    {
      CadKit.Interfaces.IMainForm mw = caller as CadKit.Interfaces.IMainForm;
      if (null != mw)
      {
        System.Windows.Forms.Form parent = mw.Form as System.Windows.Forms.Form;
        if (null != parent)
        {
          System.Diagnostics.Debug.Assert(false == parent.InvokeRequired);
          parent.Shown += this._parentShown;
        }
      }
    }

    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }


    /// <summary>
    /// Called when parent form is shown.
    /// </summary>
    private void _parentShown(object sender, System.EventArgs args)
    {
      try
      {
        CadKit.Plugins.Windows.SnapShot.SnapShotWindow form = new CadKit.Plugins.Windows.SnapShot.SnapShotWindow();
        System.Windows.Forms.Form parent = sender as System.Windows.Forms.Form;

        Component._configureDockWindow(sender, form);

        CadKit.Interfaces.IWindowMenu windowMenu = sender as CadKit.Interfaces.IWindowMenu;
        if (null != windowMenu)
        {
          windowMenu.addFormWindowMenu(form.Text, form);
        }

        parent.Activate();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1771862310: {0}", e.Message);
      }
    }


    /// <summary>
    /// Show the dock window in the proper way.
    /// </summary>
    private static void _configureDockWindow(object sender, CadKit.Plugins.Windows.SnapShot.SnapShotWindow form)
    {
      CadKit.Interfaces.IDockPanel dockPanel = sender as CadKit.Interfaces.IDockPanel;
      if (null != dockPanel)
      {
        WeifenLuo.WinFormsUI.DockPanel panel = dockPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel;
        if (null != panel)
        {

          CadKit.Interfaces.IPersistantFormData register = sender as CadKit.Interfaces.IPersistantFormData;

          if (null != register)
            register.registerPersistanceForm(typeof(CadKit.Plugins.Windows.SnapShot.SnapShotWindow).ToString(), form);

          // Show the form if we don't have persistant data.  If there is persistant data, it will be shown elsewhere.
          if (false == register.hasPersistantFormData())
            form.Show(panel);
        }
        else
        {
          form.Show();
        }
      }
      else
      {
        form.Show();
      }
    }


    /// <summary>
    /// Get the name of this plugin.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { return "SnapShot Window"; }
    }


    /// <summary>
    /// Get the plugin's description.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { return "Controls for a single off-screen rendering."; }
    }
  }
}
