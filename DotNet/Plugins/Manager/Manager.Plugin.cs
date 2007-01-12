
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
    /// Represents a single plugin.
    /// </summary>
    class Plugin : System.IDisposable
    {
      /// <summary>
      /// Data members.
      /// </summary>
      private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();
      private string _file = null;
      private CadKit.Interfaces.IPlugin _handle = null;
      private System.Reflection.Assembly _assembly = null;


      /// <summary>
      /// Constructor.
      /// </summary>
      public Plugin(string file)
      {
        _file = file;
      }


      /// <summary>
      /// Called when the object is deleted.
      /// </summary>
      ~Plugin()
      {
        try
        {
          this.Dispose();
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 3944176989: {0}", e.Message);
        }
      }


      /// <summary>
      /// Called when the object is disposed.
      /// </summary>
      public void Dispose()
      {
        try
        {
          this.release(null);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1103621480: {0}", e.Message);
        }
      }


      /// <summary>
      /// Load the plugin.
      /// </summary>
      public void load(object caller)
      {
        try
        {
          this.release(caller);
          this._load(caller);
        }
        catch (System.IO.FileNotFoundException e)
        {
          System.Console.WriteLine("Error 4292194948: Failed to load file.\n  File: {0}\n  Message: {1}", this.File, e.Message);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 4292194948: {0}", e.Message);
        }
      }


      /// <summary>
      /// Load the plugin.
      /// </summary>
      private void _load(object caller)
      {
        string file = this.File;
        System.Reflection.Assembly assembly = Manager._loadAssembly(file);
        this.Assembly = Assembly;
        if (null == assembly)
          return;

        CadKit.Interfaces.IClassFactory factory = Manager._createClassFactory(assembly, file);
        if (null == factory)
          return;

        CadKit.Interfaces.IPlugin plugin = Manager._createInstance(factory, file);
        this.Handle = plugin;
        if (null == plugin)
          return;

        plugin.start(caller);
      }


      /// <summary>
      /// Release the plugin.
      /// </summary>
      public void release(object caller)
      {
        try
        {
          this._release(caller);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1009236059: {0}", e.Message);
        }
      }


      /// <summary>
      /// Release the plugin.
      /// </summary>
      private void _release(object caller)
      {
        CadKit.Interfaces.IPlugin plugin = this.Handle;
        if (null != plugin)
        {
          plugin.finish(caller);
        }
        this.Handle = null;
        this.Assembly = null;
      }


      /// <summary>
      /// Set/get the assembly.
      /// </summary>
      private System.Reflection.Assembly Assembly
      {
        get { using (this.Lock.read()) { return _assembly; } }
        set { using (this.Lock.write()) { _assembly = value; } }
      }


      /// <summary>
      /// Set/get the handle.
      /// </summary>
      private CadKit.Interfaces.IPlugin Handle
      {
        get { using (this.Lock.read()) { return _handle; } }
        set { using (this.Lock.write()) { _handle = value; } }
      }


      /// <summary>
      /// Get the file name.
      /// </summary>
      public string File
      {
        get { using (this.Lock.read()) { return _file; } }
      }


      /// <summary>
      /// Get the plugin name.
      /// </summary>
      public string Name
      {
        get
        {
          CadKit.Interfaces.IPlugin plugin = this.Handle;
          if (null == plugin)
          {
            throw new System.Exception(System.String.Format("Error 1716053399: Plugin '{0}' is not loaded", this.File));
          }
          return plugin.Name;
        }
      }


      /// <summary>
      /// Is the plugin loaded?
      /// </summary>
      public bool IsLoaded
      {
        get { return (null != this.Handle); }
      }


      /// <summary>
      /// Get the lock.
      /// </summary>
      private CadKit.Threads.Tools.Lock Lock
      {
        get { return _lock; }
      }


      /// <summary>
      /// Get the interface, or null.
      /// </summary>
      public T get<T>()
      {
        try
        {
          CadKit.Interfaces.IPlugin plugin = this.Handle;
          if (null != plugin)
          {
            if(plugin is T )
            {
              return (T)plugin;
            }
          }
        }
        catch (System.InvalidCastException)
        {
        }
        return default(T);
      }
    }
  }
}
