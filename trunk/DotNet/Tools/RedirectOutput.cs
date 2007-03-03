
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// See http://www.ureader.com/message/1003496.aspx
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class RedirectOutput : System.IDisposable
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private static RedirectOutput _instance = null;
    private string _file = null;
    private System.IO.FileStream _stream = null;
    private long _position = 0;
    private System.IO.FileSystemWatcher _watcher = null;
    private NotifyDelegate _notify = null;
    private bool _isNotifying = false;
    private object _mutex = new object();
    private System.IO.TextWriter _original = System.Console.Out;

    /// <summary>
    /// Constants.
    /// </summary>
    private const string SYSTEM_KERNEL_DLL = "kernel32.dll";
    private const int STD_OUTPUT_HANDLE = -11; // See http://msdn2.microsoft.com/en-gb/library/ms686244.aspx

    /// <summary>
    /// Set the standard output handle.
    /// </summary>
    [System.Runtime.InteropServices.DllImport(CadKit.Tools.RedirectOutput.SYSTEM_KERNEL_DLL, SetLastError = true)]
    private static extern int SetStdHandle(int device, System.IntPtr handle);

    /// <summary>
    /// Single instance.
    /// </summary>
    public static RedirectOutput Instance
    {
      get
      {
        lock ("CadKit.Tools.RedirectOutput.Instance")
        {
          if (null == _instance)
            _instance = new RedirectOutput();
          return _instance;
        }
      }
    }


    /// <summary>
    /// Constructor
    /// </summary>
    private RedirectOutput()
    {
      try
      {
        string program = System.Reflection.Assembly.GetEntryAssembly().GetName().Name; // Handles ".vshost"
        _file = System.Windows.Forms.Application.UserAppDataPath + "\\" + program + ".out";

        System.IO.FileMode mode = System.IO.FileMode.Create;
        System.IO.FileAccess access = System.IO.FileAccess.ReadWrite;
        System.IO.FileShare share = System.IO.FileShare.Read;
        System.IO.FileOptions options = System.IO.FileOptions.RandomAccess;
        _stream = new System.IO.FileStream(_file, mode, access, share, 1024, options);

        System.IO.StreamWriter sw = new System.IO.StreamWriter(_stream);
        sw.AutoFlush = true;
        System.Console.SetOut(sw);

        // This turns on capturing of native side. Use with caution.
        CadKit.Tools.RedirectOutput.SetStdHandle(CadKit.Tools.RedirectOutput.STD_OUTPUT_HANDLE, _stream.SafeFileHandle.DangerousGetHandle());
      }
      catch (System.Exception e)
      {
        this._clear();

        // Write to standard output stream.
        System.Console.WriteLine("Error 3149241310: {0}", e.Message);
      }
    }


    /// <summary>
    /// Destructor
    /// </summary>
    ~RedirectOutput()
    {
      this.Dispose();
    }


    /// <summary>
    /// Called by the system to dispose this object.
    /// </summary>
    public void Dispose()
    {
      this._clear();
    }


    /// <summary>
    /// Clear data members and reset standard output.
    /// </summary>
    private void _clear()
    {
      try
      {
        // Reset members.
        _file = null;
        _stream = null;
        _watcher = null;

        // Reset console to print to standard output stream.
        if (null != _original)
        {
          System.Console.SetOut(_original);
          _original = null;
        }
      }
      catch (System.Exception e)
      {
        // Do not send to console.
        System.Diagnostics.Trace.WriteLine("Error 1936388322: {0}", e.Message);
      }
    }


    /// <summary>
    /// Start the file watcher.
    /// </summary>
    private void _startFileWatcher()
    {
      System.IO.FileInfo info = new System.IO.FileInfo(_file);
      _watcher = new System.IO.FileSystemWatcher();
      _watcher.Path = info.DirectoryName;
      _watcher.Filter = info.Name;
      _watcher.NotifyFilter = System.IO.NotifyFilters.Size | System.IO.NotifyFilters.Attributes;
      _watcher.Changed += this._fileChanged;
      _watcher.EnableRaisingEvents = true;
    }

    /// <summary>
    /// Called when the file changed.
    /// </summary>
    private void _fileChanged(object source, System.IO.FileSystemEventArgs args)
    {
      lock (_mutex)
      {
        // Do not recursively enter this function.
        if (false == _isNotifying)
        {
          try
          {
            // We are in process of notifying...
            _isNotifying = true;

            long original = _stream.Position;
            _stream.Position = _position;

            System.IO.StreamReader reader = new System.IO.StreamReader(_stream);
            string text = reader.ReadToEnd();

            _position = _stream.Position;
            _stream.Position = original;

            if (null != _notify)
              _notify(text);
          }
          catch (System.Exception)
          {
            // Catch and eat exceptions to avoid an infinite loop.
          }
          finally
          {
            // We are done notifying.
            _isNotifying = false;
          }
        }
      }
    }

    /// <summary>
    /// Return the file name.
    /// </summary>
    public string FileName
    {
      get { lock (_mutex) { return _file; } }
    }


    /// <summary>
    /// Get/set the synchronizing object. If you do not set this then the 
    /// callbacks happen in a thread other than the main one.
    /// </summary>
    public System.ComponentModel.ISynchronizeInvoke SynchronizingObject
    {
      get
      {
        lock (_mutex)
        {
          return (null != _watcher) ? _watcher.SynchronizingObject : null;
        }
      }
      set
      {
        lock (_mutex)
        {
          if (null != _watcher)
          {
            _watcher.SynchronizingObject = value;
          }
        }
      }
    }

    /// <summary>
    /// Delegate to notify about new text.
    /// </summary>
    public delegate void NotifyDelegate(string text);

    /// <summary>
    /// Set/get the notify delegate.
    /// </summary>
    public NotifyDelegate Notify
    {
      get { lock (_mutex) { return _notify; } }
      set 
      { 
        lock (_mutex) 
        { 
          _notify = value;

          /// Start the file watcher if we haven't already.
          try
          {
            if (null == _watcher && null != _notify)
            {
              this._startFileWatcher();
              this._fileChanged(this, null);
            }
          }
          catch (System.Exception e)
          {
            this._clear();

            // Write to standard output stream.
            System.Console.WriteLine("Error 1767897052: {0}", e.Message);
          }
        } 
      }
    }
  }
}
