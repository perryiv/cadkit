
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.LayerManager
{
  public class Component :
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.ICreateLayer
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
    }


    /// <summary>
    /// Called when parent form is shown.
    /// </summary>
    private void _parentShown(object sender, System.EventArgs args)
    {
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
      get { lock (_mutex) { return "PostGIS Layers"; } }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "Add Layer form postGIS database."; } }
    }

    /// <summary>
    /// 
    /// </summary>
    object CadKit.Interfaces.ICreateLayer.contents(object caller)
    {
      return new AddLayerForm(caller as CadKit.Interfaces.IOptionsPage);
    }


    /// <summary>
    /// 
    /// </summary>
    object CadKit.Interfaces.ICreateLayer.Image
    {
      get
      {
        return "postGIS.gif";
      }
    }


    /// <summary>
    /// 
    /// </summary>
    string CadKit.Interfaces.ICreateLayer.Name
    {
      get { return "PostGIS"; }
    }
  }
}
