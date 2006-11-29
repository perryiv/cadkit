
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class Application
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
    public static Application Instance
    {
      get
      {
        lock ("CadKit.Helios.Application.Instance")
        {
          if (null == _instance)
            _instance = new Application();
          return _instance;
        }
      }
    }

    /// <summary>
    /// Set/get the name of the program. Gets set from the assembly (program) name.
    /// </summary>
    public string Name
    {
      get { lock (_mutex) { return _name; } }
      set { lock (_mutex) { _name = value; } }
    }

    /// <summary>
    /// Set/get the main form.
    /// </summary>
    public CadKit.Helios.MainForm MainForm
    {
      get { lock (_mutex) { return _mainForm; } }
      set { lock (_mutex) { _mainForm = value; } }
    }

    /// <summary>
    /// Get the directory where this assembly resides.
    /// </summary>
    public string Directory
    {
      get
      {
        lock (_mutex)
        {
          string path = System.Windows.Forms.Application.ExecutablePath;
          string dir = System.IO.Path.GetDirectoryName(path);
          return dir;
        }
      }
    }

    /// <summary>
    /// Get the assembly file name.
    /// </summary>
    public string FileName
    {
      get
      {
        lock (_mutex)
        {
          return System.Reflection.Assembly.GetEntryAssembly().GetName().Name + ".exe";
        }
      }
    }

    /// <summary>
    /// Get the icons directory.
    /// </summary>
    public string IconDir
    {
      get
      {
        lock (_mutex)
        {
          string dir = this.Directory + "/icons";
          return dir;
        }
      }
    }


    /// <summary>
    /// Get/Set the splash screen image.
    /// </summary>
    public string SplashImage
    {
      get { lock (_mutex) { return _splashImage; } }
      set { lock (_mutex) { _splashImage = value; } }
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private static Application _instance = null;
    private string _name = System.Reflection.Assembly.GetEntryAssembly().GetName().Name; // Handles ".vshost";
    private string _splashImage = null;
    private CadKit.Helios.MainForm _mainForm = null;
    private object _mutex = new object();
  }
}
