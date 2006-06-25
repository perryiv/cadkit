
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public partial class Application
  {
    /// <summary>
    /// Constructor
    /// </summary>
    private Application()
    {
    }

    /// <summary>
    /// Single instance.
    /// </summary>
    public static Application instance()
    {
      if ( null == _instance )
        _instance = new Application();
      return _instance;
    }

    /// <summary>
    /// Set/get the name of the program. Gets set from the assembly (program) name.
    /// </summary>
    public string Name
    {
      get { return _name; }
      set { _name = value; }
    }

    /// <summary>
    /// Set/get the main form.
    /// </summary>
    public CadKit.Helios.MainForm MainForm
    {
      get { return _mainForm; }
      set { _mainForm = value; }
    }

    /// <summary>
    /// Get the directory where this assembly resides.
    /// </summary>
    public string directory()
    {
      string path = System.Windows.Forms.Application.ExecutablePath;
      string dir = System.IO.Path.GetDirectoryName( path );
      return dir;
    }

    /// <summary>
    /// Get the assembly file name.
    /// </summary>
    public string file()
    {
      return System.Reflection.Assembly.GetEntryAssembly().GetName().Name + ".exe";
    }

    /// <summary>
    /// Get the icons directory.
    /// </summary>
    public string iconDir()
    {
      string dir = this.directory();
      dir = dir.Replace( "\\bin\\Debug", "" );
      dir = dir.Replace( "\\bin\\Release", "" );
      return dir;
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private static Application _instance = null;
    private string _name = System.Reflection.Assembly.GetEntryAssembly().GetName().Name; // Handles ".vshost";
    private CadKit.Helios.MainForm _mainForm = null;
  }
}
