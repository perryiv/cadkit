
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
    private Names _getPluginFileNames(string file)
    {
      lock (_mutex)
      {
        // Initialize the names.
        Names names = new Names();

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
                  this._addFileName(names, info, node, "file");
                  this._addFileName(names, info, node, "delegate");
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

        // Return the names we have.
        return names;
      }
    }

    /// <summary>
    /// Add the name to the list.
    /// </summary>
    private void _addFileName(Names names, System.IO.FileInfo info, System.Xml.XmlNode node, string attribute)
    {
      lock (_mutex)
      {
        System.Xml.XmlAttribute attr = node.Attributes[attribute];
        if (null != attr)
        {
          string path = info.DirectoryName + '/' + attr.Value;
          path = path.Replace('\\', '/');
          path = path.Replace("//", "/");
          names.Add(path);
        }
      }
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
          Names names = this._getPluginFileNames(file);

          // Loop through the files.
          for (int i = 0; i < names.Count; ++i)
          {
            CadKit.Tools.Progress.notify("Loading: " + names[i], i, names.Count, caller);
            this._loadPlugin(names[i], caller);
          }

          // Done.
          CadKit.Tools.Progress.notify("Done loading plugins", names.Count, names.Count, caller);
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
          CadKit.Interfaces.IClassFactory factory = (CadKit.Interfaces.IClassFactory)assembly.CreateInstance("CadKit.Plugins.Factory");
          CadKit.Interfaces.IPlugin plugin = (CadKit.Interfaces.IPlugin)factory.createInstance("CadKit.Interfaces.IPlugin");
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
        try
        {
          return _assemblies[file];
        }
        catch (System.Collections.Generic.KeyNotFoundException)
        {
          System.Reflection.Assembly assembly = System.Reflection.Assembly.LoadFrom(file);
          _assemblies[file] = assembly;
          return assembly;
        }
      }
    }

    /// <summary>
    /// Local types.
    /// </summary>
    class Assemblies : System.Collections.Generic.Dictionary<string, System.Reflection.Assembly> { }
    class Names : System.Collections.Generic.List<string> { }
    class Plugins : System.Collections.Generic.List<CadKit.Interfaces.IPlugin> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private Assemblies _assemblies = new Assemblies();
    private Plugins _plugins = new Plugins();
    private static Manager _instance = null;
    private object _mutex = new object();
  }
}
