
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Jobs
{
  public class Job : System.IDisposable
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void NotifyDelegate(CadKit.Threads.Jobs.Job job);
    public class Cancel : System.Exception { }
    private class ProgressHandle : CadKit.Interfaces.ScopedReference<Progress> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = null;
    private string _name = null;
    private System.Threading.Thread _thread = null;
    private NotifyDelegate _notifyStart = null;
    private NotifyDelegate _notifyFinish = null;
    private bool _queued = false;
    private bool _canceled = false;
    private System.Exception _exception = null;
    private ProgressHandle _progress = new ProgressHandle();


    /// <summary>
    /// Constructor
    /// </summary>
    public Job()
    {
      _progress.Value = new Progress(this);
    }


    /// <summary>
    /// Destructor
    /// </summary>
    ~Job()
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
    /// Clea up this instance.
    /// </summary>
    private void _localCleanup()
    {
      _lock = null;
      _name = null;
      _thread = null;
      _notifyStart = null;
      _notifyFinish = null;
      _queued = false;
      _canceled = false;
      _exception = null;
      _progress.Value = null;
    }


    /// <summary>
    /// Queue the thread. Called by the thread manager.
    /// </summary>
    public void queue()
    {
      // Return if already queued.
      if (true == this.Queued)
        return;

      // Should be true.
      System.Diagnostics.Debug.Assert(null == this.Thread);

      // Set the flag first.
      this._setQueued(true);

      // Queue the thread.
      System.Threading.ThreadPool.QueueUserWorkItem(this._executeThread);
    }


    /// <summary>
    /// Cancel the thread. Called by the thread manager.
    /// </summary>
    public void cancel()
    {
      // Set flag here in case it didn't start yet.
      this._setCanceled(true);

      // If it's not queued then just return.
      if (false == this.Queued)
        return;

      // If there is no thread then it didn't start yet, so just return.
      System.Threading.Thread thread = this.Thread;
      if (null == thread)
        return;

      // If the current thread is this job's thread.
      if (System.Threading.Thread.CurrentThread == thread)
        throw new Cancel();

      // If we get to here then there is a currently running thread.
      this.Thread = null;
      thread.Abort();
    }


    /// <summary>
    /// Called by the system to run the thread.
    /// </summary>
    private void _executeThread(object data)
    {
      // If you lock the mutex then there is no way to communicate with this instance.
      try
      {
        // Return now if this job has been canceled.
        if (true == this.Canceled)
          return;

        // Should be true.
        System.Diagnostics.Debug.Assert(true == this.Queued);

        // Set this thread from the current thread.
        this.Thread = System.Threading.Thread.CurrentThread;

        // Set the priority.
        this.Thread.Priority = System.Threading.ThreadPriority.BelowNormal;

        // Execute the delegate if there is one.
        if (null != this.Start)
          this.Start(this);

        // Execute the delegate if there is one.
        if (null != this.Finish)
          this.Finish(this);
      }

      // If the caller canceled...
      catch (CadKit.Threads.Jobs.Job.Cancel)
      {
        this._setCanceled(true);
      }

      // If the caller canceled...
      catch (System.Threading.ThreadAbortException)
      {
        this._setCanceled(true);
      }

      // Handle exceptions.
      catch (System.Exception e)
      {
        this._setException(e);
        System.Console.WriteLine("Error 2823933800: {0}", e.Message);
      }

      // Clean up.
      finally
      {
        this._setQueued(false);
        this.Thread = null;
        CadKit.Threads.Jobs.Manager.Instance.done(this);
      }
    }


    /// <summary>
    /// Set the exception.
    /// </summary>
    private void _setException(System.Exception e)
    {
      using (this.Lock.write()) { _exception = e; }
    }


    /// <summary>
    /// Set the queued state.
    /// </summary>
    private void _setQueued(bool state)
    {
      using (this.Lock.write()) { _queued = state; }
    }


    /// <summary>
    /// Set the cancelled state.
    /// </summary>
    private void _setCanceled(bool state)
    {
      using (this.Lock.write()) { _canceled = state; }
    }


    /// <summary>
    /// Does the thread have the state?
    /// </summary>
    public bool _hasState(System.Threading.ThreadState state)
    {
      System.Threading.Thread thread = this.Thread;
      if (null == thread)
      {
        return false;
      }
      if ((thread.ThreadState & state) == state)
      {
        return true;
      }
      else
      {
        return false;
      }
    }


    /// <summary>
    /// Is the thread running?
    /// </summary>
    public bool Running
    {
      get
      {
        if (true == this._hasState(System.Threading.ThreadState.Running))
        {
          return true;
        }
        else if (true == this._hasState(System.Threading.ThreadState.Background))
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }


    /// <summary>
    /// Is the thread suspended?
    /// </summary>
    public bool Suspended
    {
      get { return this._hasState(System.Threading.ThreadState.Suspended); }
    }


    /// <summary>
    /// Is the thread suspended?
    /// </summary>
    public bool Queued
    {
      get { using (this.Lock.read()) { return _queued; } }
    }


    /// <summary>
    /// Has the thread been canceled?
    /// </summary>
    public bool Canceled
    {
      get { using (this.Lock.read()) { return _canceled; } }
    }


    /// <summary>
    /// Get the thread's exception.
    /// </summary>
    public System.Exception Exception
    {
      get { using (this.Lock.read()) { return _exception; } }
    }


    /// <summary>
    /// Did the thread exit successfuly?
    /// </summary>
    public bool Success
    {
      get { return (null == this.Exception); }
    }


    /// <summary>
    /// Set/get the name.
    /// </summary>
    public string Name
    {
      get { using (this.Lock.read()) { return _name; } }
      set { using (this.Lock.write()) { _name = value; } }
    }


    /// <summary>
    /// Set/get the thread-start delegate.
    /// </summary>
    public NotifyDelegate Start
    {
      get { using (this.Lock.read()) { return _notifyStart; } }
      set { using (this.Lock.write()) { _notifyStart = value; } }
    }


    /// <summary>
    /// Set/get the thread-finished delegate.
    /// </summary>
    public NotifyDelegate Finish
    {
      get { using (this.Lock.read()) { return _notifyFinish; } }
      set { using (this.Lock.write()) { _notifyFinish = value; } }
    }


    /// <summary>
    /// Get/set the thread member.
    /// </summary>
    private System.Threading.Thread Thread
    {
      get { using (this.Lock.read()) { return _thread; } }
      set { using (this.Lock.write()) { _thread = value; } }
    }


    /// <summary>
    /// Get the progress member.
    /// </summary>
    public CadKit.Threads.Jobs.Progress Progress
    {
      get { using (this.Lock.read()) { return _progress.Value; } }
    }


    /// <summary>
    /// Get the thread id.
    /// </summary>
    public int ThreadId
    {
      get
      {
        System.Threading.Thread thread = this.Thread;
        return (null == thread) ? 0 : thread.ManagedThreadId;
      }
    }


    /// <summary>
    /// Get the mutex.
    /// </summary>
    private CadKit.Threads.Tools.Lock Lock
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
  }
}
