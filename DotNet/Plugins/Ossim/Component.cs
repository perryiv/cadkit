
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Ossim
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
    /// Called when use of the plugin is finished.
    /// </summary>
    void CadKit.Interfaces.IPlugin.finish(object caller)
    {
    }


    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      // Re-entrant! Do not lock the mutex!
      get { return "Ossim layer"; }
    }

    /// <summary>
    /// Return the short name of this type.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      // Re-entrant! Do not lock the mutex!
      get { return "Plugin for working with OSSIM layers"; }
    }


    /// <summary>
    /// 
    /// </summary>
    object CadKit.Interfaces.ICreateLayer.contents( object caller )
    {
      return new OssimLayer(caller as CadKit.Interfaces.IOptionsPage);
    }


    /// <summary>
    /// 
    /// </summary>
    object CadKit.Interfaces.ICreateLayer.Image
    {
      get
      {
        return CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/general_options.png");
      }
    }


    /// <summary>
    /// 
    /// </summary>
    string CadKit.Interfaces.ICreateLayer.Name
    {
      get { return "Ossim"; }
    }
  }
}
