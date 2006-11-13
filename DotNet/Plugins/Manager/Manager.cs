
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins
{
  public class Manager
  {
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
    /// Get the file names from the XML file.
    /// </summary>
    private void _getPluginFileNames(string file)
    {
      lock (_mutex)
      {
        // Open the configuration file.
        try
        {
          // Parse the path string.
          System.IO.FileInfo info = new System.IO.FileInfo(file);

          // Read the file.
          System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
          doc.Load(file);

          // Get top-level node.
          System.Xml.XmlNode root = doc.FirstChild;

          // While there is a top-level node...
          while (null != root)
          {
            if ("plugins" == root.Name)
            {
              System.Xml.XmlNodeList kids = root.ChildNodes;
              for (int i = 0; i < kids.Count; ++i)
              {
                System.Xml.XmlNode node = kids[i];
                if ("plugin" == node.Name)
                {
                  this._addAttributes(_names, info.DirectoryName, node);
                }
              }
            }

            // Go to next top-level node.
            root = root.NextSibling;
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1813945695: {0}", e.Message);
        }
      }
    }

    /// <summary>
    /// Add the attributes to the list.
    /// </summary>
    private void _addAttributes(Names names, string dir, System.Xml.XmlNode node)
    {
      lock (_mutex)
      {
        string file = "";
        bool load = true;
        foreach (System.Xml.XmlAttribute attr in node.Attributes)
        {
          if(attr.Name == "file")
            file = this._getAbsolutePathIfFile(dir, attr.Value);
          else if(attr.Name == "load")
            load = bool.Parse(attr.Value);
        }

        _names.Add(file,load);
        
      }
    }

    /// <summary>
    /// If the attribute string is a file then return the absolute path.
    /// </summary>
    private string _getAbsolutePathIfFile(string dir, string attr)
    {
      string path = dir + '/' + attr;
      path = path.Replace('\\', '/');
      path = path.Replace("//", "/");
      System.IO.FileInfo file = new System.IO.FileInfo(path);
      return (true == file.Exists) ? path : attr;
    }

    /// <summary>
    /// See if there is a plugin with the given interface.
    /// </summary>
    public bool has<T>()
    {
      lock (_mutex)
      {
        return (null != this.getFirst<T>());
      }
    }

    /// <summary>
    /// Get the first plugin that supports the given type.
    /// </summary>
    public T getFirst<T>()
    {
      lock (_mutex)
      {
        foreach (CadKit.Interfaces.IPlugin plugin in _plugins)
        {
          try
          {
            T t = (T)plugin;
            if (null != t)
              return t;
          }
          catch (System.InvalidCastException)
          {
          }
        }
        return default(T);
      }
    }


    /// <summary>
    /// Get all the plugins that supports the given type.
    /// </summary>
    public T[] getAll<T>()
    {
      lock (_mutex)
      {
        System.Collections.Generic.List<T> types = new System.Collections.Generic.List<T>();
        foreach (CadKit.Interfaces.IPlugin plugin in _plugins)
        {
          try
          {
            T t = (T)plugin;
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
    }


    /// <summary>
    /// Load the plugins.
    /// </summary>
    public void load(string file, object caller)
    {
      // Open the configuration file.
      try
      {
        lock (_mutex)
        {
          // Get all the names.
          this._getPluginFileNames(file);

          int number = 0;

          // Loop through the files.
          foreach (System.Collections.Generic.KeyValuePair<string, bool> kvp in _names)
          {
            if (kvp.Value)
            {
              CadKit.Tools.Progress.notify("Loading: " + kvp.Key, number, _names.Count, caller);
              this._loadPlugin(kvp.Key, caller);
            }

            ++number;
          }

          // Done.
          CadKit.Tools.Progress.notify("Done loading plugins", _names.Count, _names.Count, caller);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1132181386: {0}", e.Message);
      }
    }


    /// <summary>
    /// Data members.
    /// </summary>
    private void _loadPlugin(string file, object caller )
    {
      try
      {
        lock (_mutex)
        {
          System.Reflection.Assembly assembly = this._findOrLoadAssembly(file);
          if (null == assembly)
          {
            System.Console.WriteLine("Error 7948918190: Failed to load assembly file: {0}", file);
            return;
          }

          CadKit.Interfaces.IClassFactory factory = (CadKit.Interfaces.IClassFactory)assembly.CreateInstance("CadKit.Plugins.Factory");
          if (null == factory)
          {
            System.Console.WriteLine("Error 7218841160: Assembly file '{0}' does not have a class factory", file);
            return;
          }

          CadKit.Interfaces.IPlugin plugin = (CadKit.Interfaces.IPlugin)factory.createInstance("CadKit.Interfaces.IPlugin");
          if (null == plugin)
          {
            System.Console.WriteLine("Error 9585018640: Failed to create plugin instance in assembly file: {0}", file);
            return;
          }

          plugin.startupNotify(caller);

          if (false == _plugins.Contains(plugin))
            _plugins.Add(plugin);
        }
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 4156259049: {0}", e.Message );
      }
    }


    /// <summary>
    /// Find or load the assembly.
    /// </summary>
    private System.Reflection.Assembly _findOrLoadAssembly(string file)
    {
      lock (_mutex)
      {
        System.Reflection.Assembly assembly = null;
        try
        {
          _assemblies.TryGetValue(file, out assembly);
          if (null == assembly)
          {
            // This may raise a LoadFromContext exception (or debugger warning). 
            // However, LoadFrom() is the right function to call. 
            // See http://www.gotdotnet.com/team/clr/LoadFromIsolation.aspx
            assembly = System.Reflection.Assembly.LoadFrom(file);
            _assemblies.Add(file, assembly);
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 4035049426: {0}", e.Message);
        }
        return assembly;
      }
    }


    /// <summary>
    /// Check to see if the plugin is loaded.
    /// </summary>
    public bool isLoaded(string file)
    {
      return _names[file];
    }


    class DummyPlugin : CadKit.Interfaces.IPlugin
    {
      string _name = "";
      string _description = "";

      public DummyPlugin(string name, string description)
      {
      }

      #region IPlugin Members

      public string Name
      {
        get { return _name; }
      }

      public string Description
      {
        get { return _description; }
      }

      public void startupNotify(object caller)
      {
        // Do nothing.
      }

      #endregion
    }

    public CadKit.Interfaces.IPlugin pluginInterface(string file)
    {
      System.Reflection.Assembly assembly = null;

      // Check to see if we have the plugin.
      _assemblies.TryGetValue(file, out assembly);

      if (assembly == null)
      {
        // See: http://msdn2.microsoft.com/en-US/library/7hcs6az6.aspx
        System.AppDomain domain = System.AppDomain.CreateDomain("CadKit.Plugins.Factory");
        object obj = domain.CreateInstanceFrom(file, "CadKit.Plugins.Factory");
        //domain.AssemblyResolve += new System.ResolveEventHandler(domain_AssemblyResolve);

        //assembly = domain.Load(file);
        //object obj = domain.CreateInstance(file, "CadKit.Plugins.Factory");

        //CadKit.Interfaces.IPlugin plugin = _getInterface(file, assembly);

        //DummyPlugin dummy = new DummyPlugin(plugin.Name, plugin.Description);

        System.AppDomain.Unload(domain);

        //return dummy;
        return new DummyPlugin("", "");
      }

      return _getInterface(file, assembly);
    }

    byte[] loadFile(string file)
    {
      System.IO.FileStream fs = new System.IO.FileStream(file, System.IO.FileMode.Open);
      byte[] buffer = new byte[(int)fs.Length];
      fs.Read(buffer, 0, buffer.Length);
      fs.Close();

      return buffer;
    }

    System.Reflection.Assembly domain_AssemblyResolve(object sender, System.ResolveEventArgs args)
    {
      System.AppDomain domain = (System.AppDomain)sender;

      byte[] rawAssembly = loadFile(args.Name);
      //byte[] rawSymbolStore = loadFile("temp.pdb");
      //Assembly assembly = domain.Load(rawAssembly, rawSymbolStore);
      return domain.Load(rawAssembly);
    }

    private static CadKit.Interfaces.IPlugin _getInterface(string file, System.Reflection.Assembly assembly)
    {
      CadKit.Interfaces.IClassFactory factory = (CadKit.Interfaces.IClassFactory)assembly.CreateInstance("CadKit.Plugins.Factory");
      if (null == factory)
      {
        throw new System.Exception("Error 1407956654: Assembly file " + file + " does not have a class factory");
      }

      CadKit.Interfaces.IPlugin plugin = (CadKit.Interfaces.IPlugin)factory.createInstance("CadKit.Interfaces.IPlugin");
      if (null == plugin)
      {
        throw new System.Exception("Error 1689831654: Failed to create plugin instance in assembly file: " + file);
      }
      return plugin;
    }


    /// <summary>
    /// Return the number of plugins.
    /// </summary>
    public int NumPlugins
    {
      get { lock (_mutex) { return _plugins.Count; } }
    }


    /// <summary>
    /// Return the plugin names.
    /// </summary>
    public string[] PluginNames
    {
      get
      {
        lock (_mutex)
        {
          System.Collections.Generic.List<string> names = new System.Collections.Generic.List<string>();
          foreach (CadKit.Interfaces.IPlugin plugin in _plugins)
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
          return names.ToArray();
        }
      }
    }


    /// <summary>
    /// Get the names.
    /// </summary>
    public string[] FileNames
    {
      get
      {
        System.Collections.Generic.List<string> names = new System.Collections.Generic.List<string>();
        foreach (System.Collections.Generic.KeyValuePair<string, bool> kvp in _names)
        {
          names.Add(kvp.Key);
        }

        return names.ToArray();
      }
    }


    /// <summary>
    /// Local types.
    /// </summary>
    class Assemblies : System.Collections.Generic.Dictionary<string, System.Reflection.Assembly> { }
    class Names : System.Collections.Generic.Dictionary<string, bool> { }
    class Plugins : System.Collections.Generic.List<CadKit.Interfaces.IPlugin> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
    private object _mutex = new object();
    private Assemblies _assemblies = new Assemblies();
    private Names _names = new Names();
    private Plugins _plugins = new Plugins();
  }
}
