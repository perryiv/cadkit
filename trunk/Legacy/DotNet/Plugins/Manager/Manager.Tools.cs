
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
    /// Load the assembly.
    /// </summary>
    private static System.Reflection.Assembly _loadAssembly(string file)
    {
      try
      {
        System.Reflection.Assembly assembly = System.Reflection.Assembly.LoadFrom(file);
        if (null == assembly)
        {
          throw new System.Exception(System.String.Format("Error 8782173720: failed to load assembly file: {0}", file));
        }
        return assembly;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1074387242: Failed to load assembly file '{0}', {1}", file, e.Message);
        throw;
      }
    }


    /// <summary>
    /// Return a new class factory.
    /// </summary>
    private static CadKit.Interfaces.IClassFactory _createClassFactory(System.Reflection.Assembly assembly, string file)
    {
      try
      {
        object temp = assembly.CreateInstance("CadKit.Plugins.Factory");
        if (null == temp)
        {
          throw new System.Exception(System.String.Format("Error 3490341679: failed to create class factory in assembly file: {0}", file));
        }
        using (ClassFactoryHandle factory = new ClassFactoryHandle(temp as CadKit.Interfaces.IClassFactory))
        {
          if (false == factory.Valid)
          {
            throw new System.Exception(System.String.Format("Error 1228345170: object created assembly file '{0}' is not a factory", file));
          }
          return factory.release();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2104420766: Failed to create class factory in assembly file '{0}', {1}", file, e.Message);
        throw;
      }
    }


    /// <summary>
    /// Return a new plugin interface.
    /// </summary>
    private static CadKit.Interfaces.IPlugin _createInstance(CadKit.Interfaces.IClassFactory factory, string file)
    {
      try
      {
        object temp = factory.createInstance("CadKit.Interfaces.IPlugin");
        if (null == temp)
        {
          throw new System.Exception(System.String.Format("Error 3887634468: Failed to create plugin instance in assembly file: {0}", file));
        }
        using (PluginHandle plugin = new PluginHandle(temp as CadKit.Interfaces.IPlugin))
        {
          if (false == plugin.Valid)
          {
            throw new System.Exception(System.String.Format("Error 1228345170: object created in assembly file '{0}' is not a plugin", file));
          }
          return plugin.release();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1716255460: Failed to create plugin interface in assembly file '{0}', {1}", file, e.Message);
        throw;
      }
    }


    /// <summary>
    /// Get the file names from the XML file.
    /// </summary>
    private static void _getPluginFileNames(string pluginDir, string xmlFile, Names names)
    {
      // Open the configuration file.
      try
      {
        // Read the file.
        System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
        doc.Load(xmlFile);

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
                Manager._addAttributes(pluginDir, node, names);
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


    /// <summary>
    /// Add the attributes to the list.
    /// </summary>
    private static void _addAttributes(string dir, System.Xml.XmlNode node, Names names)
    {
      string file = "";
      bool load = true;
      bool native = false;
      foreach (System.Xml.XmlAttribute attr in node.Attributes)
      {
        if (attr.Name == "file")
        {
          file = Manager._getAbsolutePathIfFile(dir, attr.Value);
        }
        else if (attr.Name == "load")
        {
          load = bool.Parse(attr.Value);
        }
        else if (attr.Name == "native")
        {
          native = bool.Parse(attr.Value);
        }
      }

      if (false == native)
        names.Add(file, load);
      else
      {
        if (load)
          CadKit.Init.Glue.Plugins.loadPlugin(file);
      }
    }


    /// <summary>
    /// If the attribute string is a file then return the absolute path.
    /// </summary>
    private static string _getAbsolutePathIfFile(string dir, string attr)
    {
      string path = dir + '/' + attr;
      path = path.Replace('\\', '/');
      path = path.Replace("//", "/");
      System.IO.FileInfo file = new System.IO.FileInfo(path);
      return (true == file.Exists) ? path : attr;
    }
  }
}
