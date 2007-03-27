
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Delegates.SimFlowDelegate
{
  public class Component :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IGuiDelegateCreate
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
    void CadKit.Interfaces.IPlugin.start ( object caller )
    {
      lock (_mutex)
      {
      }
    }

      void CadKit.Interfaces.IPlugin.finish( object caller )
      {
      }

    /// <summary>
    /// Get the name of this plugin.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { lock (_mutex) { return "SimFlow Delegate"; } }
    }

    /// <summary>
    /// Get the plugin's description.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { lock (_mutex) { return "User-Interface for Scene Document."; } }
    }

    /// <summary>
    /// Create new gui-delegate.
    /// </summary>
    object CadKit.Interfaces.IGuiDelegateCreate.create ( object caller )
    {
      return new CadKit.Plugins.Delegates.SimFlowDelegate.Delegate();
    }
  }
}
