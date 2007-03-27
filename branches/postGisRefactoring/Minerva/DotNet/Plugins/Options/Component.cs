
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Options
{
  public class Component :
    CadKit.Referenced.Base,
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
    void CadKit.Interfaces.IPlugin.start(object caller)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
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
      get { lock (_mutex) { return "Minerva Options"; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "Options for the Minerva"; } }
    }

    /// <summary>
    /// Get the name of the page.
    /// </summary>
    string CadKit.Interfaces.IOptionsPageAdd.Name
    {
      get { lock (_mutex) { return "Minerva"; } }
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
        DT.Minerva.Plugins.Options.PageContent content = new DT.Minerva.Plugins.Options.PageContent(parent as CadKit.Interfaces.IOptionsPage);
        return content;
      }
    }
  }
}
