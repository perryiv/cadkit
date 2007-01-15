
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Projection
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
    void CadKit.Interfaces.IPlugin.start(object caller)
    {
      lock (_mutex)
      {
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
      get { lock (_mutex) { return "Projection"; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "Projection options for GIS Layers"; } }
    }

    /// <summary>
    /// Get the name of the page.
    /// </summary>
    string CadKit.Interfaces.IOptionsPageAdd.Name
    {
      get { lock (_mutex) { return "Projection"; } }
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
    object CadKit.Interfaces.IOptionsPageAdd.contents(object parent)
    {
      lock (_mutex)
      {
        DT.Minerva.Plugins.Projection.ProjectionPanel content = new DT.Minerva.Plugins.Projection.ProjectionPanel(parent as CadKit.Interfaces.IOptionsPage);
        return content;
      }
    }
  }
}
