
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
    public static Manager instance()
    {
      if ( null == _instance )
        _instance = new Manager();
      return _instance;
    }

    /// <summary>
    /// Get the file names from the XML file.
    /// </summary>
    private Names _getPluginFileNames ( string file )
    {
      // Initialize the names.
      Names names = new Names();

      // Open the configuration file.
      try
      {
        // Parse the path string.
        System.IO.FileInfo info = new System.IO.FileInfo ( file );

        // Read the file.
        System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
        doc.Load( file );

        // Get top-level node.
        System.Xml.XmlNode root = doc.FirstChild;

        // While there is a top-level node...
        while ( null != root )
        {
          if ( "plugins" == root.Name )
          {
            System.Xml.XmlNodeList kids = root.ChildNodes;
            for ( int i = 0; i < kids.Count; ++i )
            {
              System.Xml.XmlNode node = kids[i];
              if ( "plugin" == node.Name )
              {
                System.Xml.XmlAttribute attr = node.Attributes["file"];
                if ( null != attr )
                {
                  string path = info.DirectoryName + '/' + attr.Value;
                  path = path.Replace( '\\', '/' );
                  path = path.Replace( "//", "/" );
                  names.Add ( path );
                }
              }
            }
          }

          // Go to next top-level node.
          root = root.NextSibling;
        }
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 1813945695: {0}", e.Message );
      }

      // Return the names we have.
      return names;
    }


    /// <summary>
    /// Load the plugins.
    /// </summary>
    public void load(string file, object caller)
    {
      // Open the configuration file.
      try
      {
        // Get all the names.
        Names names = this._getPluginFileNames(file);

        // Loop through the files.
        for ( int i = 0; i < names.Count; ++i )
        {
          CadKit.Tools.Progress.notify("Loading file: " + names[i], i, names.Count, caller);
          this._loadPlugin(names[i], caller);
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
    /// Data members.
    /// </summary>
    private void _loadPlugin(string file, object caller )
    {
      try
      {
        System.Reflection.Assembly plugin = this._findOrLoadAssembly(file);
        CadKit.Interfaces.IClassFactory factory = ( CadKit.Interfaces.IClassFactory ) plugin.CreateInstance("CadKit.Plugins.Factory");
        CadKit.Interfaces.IPlugin startup = (CadKit.Interfaces.IPlugin) factory.createInstance("CadKit.Interfaces.IPlugin");
        startup.startupNotify(caller);
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

    /// <summary>
    /// Local types.
    /// </summary>
    class Assemblies : System.Collections.Generic.Dictionary<string, System.Reflection.Assembly> { }
    class Names : System.Collections.Generic.List<string> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private Assemblies _assemblies = new Assemblies();
    private static Manager _instance = null;
  }
}
