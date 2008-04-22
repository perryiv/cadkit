
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class ReadDocumentTask : 
    System.IDisposable,
    CadKit.Interfaces.IProgressBar
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class ProgressBarHandle : CadKit.Interfaces.ScopedReference<CadKit.Interfaces.IProgressBar> { }


    /// <summary>
    /// Local data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = null;
    private object _caller = null;
    private string _file = null;
    private ProgressBarHandle _progress = new ProgressBarHandle();


    /// <summary>
    /// Constructor.
    /// </summary>
    protected ReadDocumentTask(string file, object caller)
      : base()
    {
      // Set members.
      _caller = caller;
      _file = file;

      // Check state.
      if (null == _file)
        throw new System.ArgumentNullException("Error 1310704890: Given file name is null");
      if (0 == _file.Length)
        throw new System.ArgumentException("Error 3052440170: Length of given file name is zero");

      // Make a job.
      CadKit.Threads.Jobs.Job job = new CadKit.Threads.Jobs.Job();

      // Set delegates.
      job.Start += this._start;
      job.Finish += this._finish;
      job.Progress.Minimum = 0;
      job.Progress.Maximum = this._fileSize(file);

      // Progress interface.
      _progress.Value = job.Progress as CadKit.Interfaces.IProgressBar;

      // Queue the job.
      CadKit.Threads.Jobs.Manager.Instance.add(job);
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~ReadDocumentTask()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Called by the system to dispose this instance.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this._localCleanup();
    }


    /// <summary>
    /// Clean this instance.
    /// </summary>
    private void _localCleanup()
    {
      try
      {
        _lock = null;
        _caller = null;
        _file = null;
        _progress.Value = null;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4122087760: {0}", e.Message);
      }
    }


    /// <summary>
    /// Return the file size.
    /// </summary>
    protected int _fileSize(string file)
    {
      System.IO.FileInfo info = new System.IO.FileInfo(file);
      return (int)info.Length;
    }


    /// <summary>
    /// Called when the thread starts.
    /// </summary>
    private void _start(CadKit.Threads.Jobs.Job job)
    {
      this._startJob(job);
    }


    /// <summary>
    /// Called when the thread finishes.
    /// </summary>
    private void _finish(CadKit.Threads.Jobs.Job job)
    {
      this._finishJob(job);
    }


    /// <summary>
    /// Start the job.
    /// </summary>
    protected virtual void _startJob(CadKit.Threads.Jobs.Job job)
    {
    }


    /// <summary>
    /// Finish the job.
    /// </summary>
    protected virtual void _finishJob(CadKit.Threads.Jobs.Job job)
    {
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    protected CadKit.Threads.Tools.Lock Lock
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
    /// Get the file.
    /// </summary>
    protected string File
    {
      get { using (this.Lock.read()) { return _file; } }
    }


    /// <summary>
    /// Get the caller.
    /// </summary>
    protected object Caller
    {
      get { using (this.Lock.read()) { return _caller; } }
    }


    /// <summary>
    /// Set the text.
    /// </summary>
    string CadKit.Interfaces.IProgressBar.Text
    {
      get
      {
        using (this.Lock.read())
        {
          return (true == _progress.Valid) ? _progress.Value.Text : "";
        }
      }
      set
      {
        using (this.Lock.write())
        {
          if (true == _progress.Valid)
          {
            _progress.Value.Text = value;
          }
        }
      }
    }


    /// <summary>
    /// Get the range.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Range
    {
      get
      {
        using (this.Lock.read())
        {
          return (true == _progress.Valid) ? _progress.Value.Range : 100;
        }
      }
    }


    /// <summary>
    /// Set the value.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Value
    {
      get
      {
        using (this.Lock.read())
        {
          return (true == _progress.Valid) ? _progress.Value.Value : 0;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          if (true == _progress.Valid)
          {
            _progress.Value.Value = value;
          }
        }
      }
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.reference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference(bool allowCleanup)
    {
      // TODO
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    uint CadKit.Interfaces.IReferenced.RefCount
    {
      get { return 0; } // TODO
    }
  }
}
