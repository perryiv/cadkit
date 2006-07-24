
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Options.General
{
  public class Component : 
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IOptionsPageAdd
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
    void CadKit.Interfaces.IPlugin.startupNotify(object caller)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return "General Options"; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "General options for the application"; } }
    }

    /// <summary>
    /// Get the name of the page.
    /// </summary>
    string CadKit.Interfaces.IOptionsPageAdd.Name
    {
      get { lock (_mutex) { return "General"; } }
    }

    /// <summary>
    /// Get the image (System.Drawing.Image) or the name 
    /// of the image file (string).
    /// </summary>
    object CadKit.Interfaces.IOptionsPageAdd.Image
    {
      get { lock (_mutex) { return "general_options.png"; } }
    }

    /// <summary>
    /// Should be a System.Windows.Forms.Control.
    /// </summary>
    object CadKit.Interfaces.IOptionsPageAdd.contents ( object parent )
    {
      lock (_mutex)
      {
        CadKit.Plugins.Options.General.PageContent content = new CadKit.Plugins.Options.General.PageContent();
        //content.BackColor = CadKit.Tools.Random.color();
        return content;
      }
    }
  }
}
