
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
    /// Load the plugins.
    /// </summary>
    public void load ( string file )
    {
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
                  this._loadPlugin( path ); // Not getting the proper path here...
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
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private void _loadPlugin( string file )
    {
      try
      {
        System.Reflection.Assembly plugin = System.Reflection.Assembly.Load( file );
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 4156259049: {0}", e.Message );
      }
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
  }
}
