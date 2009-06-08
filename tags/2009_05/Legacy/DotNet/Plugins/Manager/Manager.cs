
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins
{
  public partial class Manager
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class Names : System.Collections.Generic.Dictionary<string, bool> { }
    public class ClassFactoryHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.IClassFactory>
    {
      public ClassFactoryHandle(CadKit.Interfaces.IClassFactory factory) : base(factory) { }
    }
    public class PluginHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.IPlugin>
    {
      public PluginHandle() : base() { }
      public PluginHandle(CadKit.Interfaces.IPlugin plugin) : base(plugin) { }
    }
    class Plugins : System.Collections.Generic.List<Manager.Plugin> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
    private CadKit.Threads.Tools.Lock _lock = null;
    private Plugins _plugins = new Plugins();


    /// <summary>
    /// Constructor
    /// </summary>
    private Manager()
    {
    }


    /// <summary>
    /// Single instance.
    /// </summary>
    public static Manager Instance
    {
      get
      {
        lock ("CadKit.Plugins.Instance")
        {
          if (null == _instance)
            _instance = new Manager();
          return _instance;
        }
      }
    }


    /// <summary>
    /// See if there is a plugin with the given interface.
    /// </summary>
    public bool has<T>()
    {
      return (null != this.getFirst<T>());
    }


    /// <summary>
    /// Get the first plugin that supports the given type.
    /// </summary>
    public T getFirst<T>()
    {
      Plugins plugins = this._pluginsCopy();
      foreach (Plugin plugin in plugins)
      {
        try
        {
          T t = plugin.get<T>();
          if (null != t)
          {
            return t;
          }
        }
        catch (System.InvalidCastException)
        {
        }
      }
      return default(T);
    }


    /// <summary>
    /// Get all the plugins that supports the given type.
    /// </summary>
    public T[] getAll<T>()
    {
      Plugins plugins = this._pluginsCopy();
      System.Collections.Generic.List<T> types = new System.Collections.Generic.List<T>();
      foreach (Plugin plugin in plugins)
      {
        try
        {
          T t = plugin.get<T>();
          if (null != t)
          {
            types.Add(t);
          }
        }
        catch (System.InvalidCastException)
        {
        }
      }
      return types.ToArray();
    }


    /// <summary>
    /// Load the plugins.
    /// </summary>
    public void load(string pluginDir, string xmlFile, object caller)
    {
      // Open the configuration file.
      try
      {
        // Get all the names.
        Names names = new Names();
        Manager._getPluginFileNames(pluginDir, xmlFile, names);

        // Initialize.
        int number = 0;

        // Loop through the files.
        foreach (System.Collections.Generic.KeyValuePair<string, bool> pair in names)
        {
          // Make new plugin.
          Plugin plugin = new Plugin(pair.Key);

          // Load if we should.
          if (true == pair.Value)
          {
            CadKit.Tools.Progress.notify("Loading: " + plugin.File, number, names.Count, caller);
            plugin.load(caller);
          }

          // Increment.
          ++number;

          // Add to our list
          this._addPlugin(plugin);
        }

        // Done.
        CadKit.Tools.Progress.notify("Done loading plugins", names.Count, names.Count, caller);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1132181386: {0}", e.Message);
      }
    }


    /// <summary>
    /// Add the plugin to the list.
    /// </summary>
    private void _addPlugin(Plugin plugin)
    {
      if (null != plugin)
      {
        using (this.Lock.write())
        {
          _plugins.Add(plugin);
        }
      }
    }


    /// <summary>
    /// Check to see if the plugin is loaded.
    /// </summary>
    public bool isLoaded(string file)
    {
      Plugins plugins = this._pluginsCopy();
      foreach (Plugin plugin in plugins)
      {
        if (file == plugin.File)
        {
          if (true == plugin.IsLoaded)
          {
            return true;
          }
        }
      }
      return false;
    }


    /// <summary>
    /// Return the number of plugins.
    /// </summary>
    public int NumPlugins
    {
      get { using (this.Lock.read()) { return _plugins.Count; } }
    }


    /// <summary>
    /// Return the plugin names.
    /// </summary>
    public string[] PluginNames
    {
      get
      {
        Plugins plugins = this._pluginsCopy();
        System.Collections.Generic.List<string> names = new System.Collections.Generic.List<string>();
        foreach (Plugin plugin in plugins)
        {
          if (true == plugin.IsLoaded)
          {
            try
            {
              names.Add(plugin.Name);
            }
            catch (System.Exception e)
            {
              System.Console.WriteLine("Error 3013611395: {0}", e.Message);
            }
          }
        }
        return names.ToArray();
      }
    }


    /// <summary>
    /// Get the names.
    /// </summary>
    public string[] FileNames
    {
      get
      {
        Plugins plugins = this._pluginsCopy();
        System.Collections.Generic.List<string> names = new System.Collections.Generic.List<string>();
        foreach (Plugin plugin in plugins)
        {
          try
          {
            names.Add(plugin.File);
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 1170924029: {0}", e.Message);
          }
        }
        return names.ToArray();
      }
    }


    /// <summary>
    /// Release all the plugins.
    /// </summary>
    public void release(object caller)
    {
      Plugins plugins = this._pluginsCopy();
      foreach (Plugin plugin in plugins)
      {
        try
        {
          this._release(plugin, caller);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 8071198400: {0}", e.Message);
        }
      }
    }


    /// <summary>
    /// Release the plugins.
    /// </summary>
    private void _release(Plugin plugin, object caller)
    {
      if (null != plugin)
      {
        try
        {
          plugin.release(caller);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1813688115: {0}", e.Message);
        }
      }
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    private CadKit.Threads.Tools.Lock Lock
    {
      get
      {
        // If this gets called from the finalizer then the lock may have 
        // already been destroyed and set to null.
        if (null == _lock)
        {
          _lock = new CadKit.Threads.Tools.Lock();
        }
        return _lock;
      }
    }


    /// <summary>
    /// Return a copy of the list of plugins.
    /// </summary>
    private Plugins _pluginsCopy()
    {
      Plugins plugins = new Plugins();
      using (this.Lock.read())
      {
        plugins.AddRange(_plugins);
      }
      return plugins;
    }
  }
}
