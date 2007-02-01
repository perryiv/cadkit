
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Animate
{
  public class Component :
    CadKit.Interfaces.IPlugin
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
    void CadKit.Interfaces.IPlugin.start(object caller)
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
      DT.Minerva.Plugins.Animate.Animate form = new DT.Minerva.Plugins.Animate.Animate();
      System.Windows.Forms.Form parent = sender as System.Windows.Forms.Form;
      CadKit.Tools.ToolWindow.configure(form, parent, "Animate", false);

      _configureDockWindow(sender, form);

      CadKit.Interfaces.IWindowMenu windowMenu = sender as CadKit.Interfaces.IWindowMenu;
      if (null != windowMenu)
      {
        windowMenu.addFormWindowMenu(form.Text, form);
      }

      parent.Activate();
    }

    private static void _configureDockWindow(object sender, DT.Minerva.Plugins.Animate.Animate form)
    {
      CadKit.Interfaces.IDockPanel dockPanel = sender as CadKit.Interfaces.IDockPanel;
      if (null != dockPanel)
      {
        WeifenLuo.WinFormsUI.DockPanel panel = dockPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel;
        if (null != panel)
        {

          CadKit.Interfaces.IPersistantFormData register = sender as CadKit.Interfaces.IPersistantFormData;

          if (null != register)
            register.registerPersistanceForm(typeof(DT.Minerva.Plugins.Animate.Animate).ToString(), form);

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
    /// Called when the plugin is finished
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return "Animate"; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "Animate time varying data in scene"; } }
    }
  }
}
