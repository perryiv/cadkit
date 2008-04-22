
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Database
{
  public class Component :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IPlugin,
    CadKit.Interfaces.IDiskStorageCreate
  {
    /// <summary>
    /// Data members.
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
    /// Get the name of this plugin.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Name
    {
      get { return "Temporary Disk Storage"; }
    }

    /// <summary>
    /// Get the plugin's description.
    /// </summary>
    string CadKit.Interfaces.IPlugin.Description
    {
      get { return "SQLite-based disk storage for large data sets."; }
    }

    /// <summary>
    /// Create disk storage.
    /// </summary>
    CadKit.Interfaces.IDiskStorage CadKit.Interfaces.IDiskStorageCreate.create(string name)
    {
      lock (_mutex)
      {
        return new CadKit.Plugins.Database.Storage.DiskStorage(name);
      }
    }
  }
}
