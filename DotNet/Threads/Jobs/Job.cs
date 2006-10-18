
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Jobs
{
  public class Job
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void NotifyDelegate(CadKit.Threads.Jobs.Job job);
    public class Cancel : System.Exception { }

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private string _name = null;
    private System.Threading.Thread _thread = null;
    private NotifyDelegate _notifyStart = null;
    private NotifyDelegate _notifyFinish = null;
    private bool _queued = false;
    private bool _canceled = false;
    private System.Exception _exception = null;
    private Progress _progress = null;

    /// <summary>
    /// Constructor
    /// </summary>
    public Job()
    {
      _progress = new Progress(this);
    }

    /// <summary>
    /// Queue the thread. Called by the thread manager.
    /// </summary>
    public void queue()
    {
      lock (this.Mutex)
      {
        // Return if already queued.
        if (true == this.Queued)
          return;

        // Should be true.
        System.Diagnostics.Debug.Assert(null == this.Thread);

        // Queue the thread.
        System.Threading.ThreadPool.QueueUserWorkItem(this._executeThread);

        // Set flag.
        this._setQueued(true);
      }
    }

    /// <summary>
    /// Cancel the thread. Called by the thread manager.
    /// </summary>
    public void cancel()
    {
      lock (this.Mutex)
      {
        // Set flag here in case it didn't start yet.
        this._setCanceled(true);

        // If it's not queued then just return.
        if (false == this.Queued)
          return;

        // If there is no thread then it didn't start yet, so just return.
        if (null == this.Thread)
          return;

        // If the current thread is this job's thread.
        if (System.Threading.Thread.CurrentThread == this.Thread)
          throw new Cancel();

        // If we get to here then there is a currently running thread.
        System.Threading.Thread thread = this.Thread;
        this.Thread = null;
        thread.Abort();
      }
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
      catch (Cancel)
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
      lock (this.Mutex) { _exception = e; }
    }

    /// <summary>
    /// Set the queued state.
    /// </summary>
    private void _setQueued(bool state)
    {
      lock (this.Mutex) { _queued = state; }
    }

    /// <summary>
    /// Set the cancelled state.
    /// </summary>
    private void _setCanceled(bool state)
    {
      lock (this.Mutex) { _canceled = state; }
    }

    /// <summary>
    /// Does the thread have the state?
    /// </summary>
    public bool _hasState(System.Threading.ThreadState state)
    {
      lock (this.Mutex)
      {
        if (null == this.Thread)
        {
          return false;
        }
        if ((this.Thread.ThreadState & state) == state)
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
    /// Is the thread running?
    /// </summary>
    public bool Running
    {
      get
      {
        lock (this.Mutex)
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
    }

    /// <summary>
    /// Is the thread suspended?
    /// </summary>
    public bool Suspended
    {
      get { lock (this.Mutex) { return this._hasState(System.Threading.ThreadState.Suspended); } }
    }

    /// <summary>
    /// Is the thread suspended?
    /// </summary>
    public bool Queued
    {
      get { lock (this.Mutex) { return _queued; } }
    }

    /// <summary>
    /// Has the thread been canceled?
    /// </summary>
    public bool Canceled
    {
      get { lock (this.Mutex) { return _canceled; } }
    }

    /// <summary>
    /// Get the thread's exception.
    /// </summary>
    public System.Exception Exception
    {
      get { lock (this.Mutex) { return _exception; } }
    }

    /// <summary>
    /// Did the thread exit successfuly?
    /// </summary>
    public bool Success
    {
      get { lock (this.Mutex) { return (null == this.Exception); } }
    }

    /// <summary>
    /// Set/get the name.
    /// </summary>
    public string Name
    {
      get { lock (this.Mutex) { return _name; } }
      set { lock (this.Mutex) { _name = value; } }
    }

    /// <summary>
    /// Set/get the thread-start delegate.
    /// </summary>
    public NotifyDelegate Start
    {
      get { lock (this.Mutex) { return _notifyStart; } }
      set { lock (this.Mutex) { _notifyStart = value; } }
    }

    /// <summary>
    /// Set/get the thread-finished delegate.
    /// </summary>
    public NotifyDelegate Finish
    {
      get { lock (this.Mutex) { return _notifyFinish; } }
      set { lock (this.Mutex) { _notifyFinish = value; } }
    }

    /// <summary>
    /// Get/set the thread member.
    /// </summary>
    private System.Threading.Thread Thread
    {
      get { lock (this.Mutex) { return _thread; } }
      set { lock (this.Mutex) { _thread = value; } }
    }

    /// <summary>
    /// Get the progress member.
    /// </summary>
    public CadKit.Threads.Jobs.Progress Progress
    {
      get
      {
        lock (this.Mutex)
        {
          return _progress;
        }
      }
    }

    /// <summary>
    /// Get the thread id.
    /// </summary>
    public int ThreadId
    {
      get
      {
        lock (this.Mutex)
        {
          if (null == this.Thread)
            return 0;
          else
            return this.Thread.ManagedThreadId;
        }
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    private object Mutex
    {
      get { return _mutex; }
    }

    /// <summary>
    /// Resume the thread if it is suspended.
    /// </summary>
    public void resume()
    {
      lock (this.Mutex)
      {
        try
        {
          if (null == this.Thread)
            return;

          if (true == this.Suspended)
            this.Thread.Resume();
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 2158168122: {0}", e.Message);
        }
      }
    }

    /// <summary>
    /// Pause the thread if it is running.
    /// </summary>
    public void pause()
    {
      lock (this.Mutex)
      {
        try
        {
          if (null == this.Thread)
            return;

          if (true == this.Running)
          {
            // The trouble with pausing and resuming is that when you call this function, the worker thread may suspend right in the middle of having one or more mutexes locked. When the GUI tries to do something with the same mutex (like resume) it blocks... forever. What you need is a way to 1) block here until the worker thread is actually suspended and 2) ensure that the worker thread does not suspend when it has a mutex locked.
            this.Thread.Suspend();
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1266022253: {0}", e.Message);
        }
      }
    }
  }
}
