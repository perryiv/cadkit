
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Jobs
{
  public class Manager
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public class JobList : System.Collections.Generic.List<CadKit.Threads.Jobs.Job> { }
    public delegate void CollectionChangedDelegate(CadKit.Threads.Jobs.Job job);

    /// <summary>
    /// Data members.
    /// </summary>
    private static Manager _instance = null;
    private object _mutex = new object();
    private JobList _jobs = new JobList();
    private CollectionChangedDelegate _notifyAdd = null;
    private CollectionChangedDelegate _notifyRemove = null;
    private CollectionChangedDelegate _notifyFinish = null;

    /// <summary>
    /// Constructor
    /// </summary>
    private Manager()
    {
    }

    /// <summary>
    /// Single instance.
    /// </summary>
    public static Manager Instance
    {
      get
      {
        lock ("CadKit.Threads.Jobs.Instance")
        {
          if (null == _instance)
            _instance = new Manager();
          return _instance;
        }
      }
    }

    /// <summary>
    /// Set/get the notify-add delegate.
    /// </summary>
    public CollectionChangedDelegate JobAdded
    {
      get { lock (this.Mutex) { return _notifyAdd; } }
      set { lock (this.Mutex) { _notifyAdd = value; } }
    }

    /// <summary>
    /// Set/get the notify-remove delegate.
    /// </summary>
    public CollectionChangedDelegate JobRemoved
    {
      get { lock (this.Mutex) { return _notifyRemove; } }
      set { lock (this.Mutex) { _notifyRemove = value; } }
    }

    /// <summary>
    /// Set/get the notify-finish delegate.
    /// </summary>
    public CollectionChangedDelegate JobFinished
    {
      get { lock (this.Mutex) { return _notifyFinish; } }
      set { lock (this.Mutex) { _notifyFinish = value; } }
    }

    /// <summary>
    /// Add a job.
    /// </summary>
    public void add(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        if (null == job)
          return;

        if (true == _jobs.Contains(job))
          return;

        _jobs.Add(job);
        job.queue();

        if (null != _notifyAdd)
          _notifyAdd(job);
      }
    }

    /// <summary>
    /// Remove a job.
    /// </summary>
    public void remove(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        if (null == job)
          return;

        if (false == this.Contains(job))
          return;

        _jobs.Remove(job);
        job.cancel();

        if (null != _notifyRemove)
          _notifyRemove(job);
      }
    }

    /// <summary>
    /// Remove the job because it is done. Called from the job's thread handler.
    /// </summary>
    public void done(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        _jobs.Remove(job);

        if (null != _notifyFinish)
          _notifyFinish(job);
      }
    }

    /// <summary>
    /// Clear all the jobs.
    /// </summary>
    public void clear()
    {
      lock (this.Mutex)
      {
        while (this.NumJobs > 0)
        {
          CadKit.Threads.Jobs.Job job = _jobs[_jobs.Count - 1];
          this.remove(job);
        }
      }
    }

    /// <summary>
    /// See if the job is in the list.
    /// </summary>
    public bool Contains(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        return _jobs.Contains(job);
      }
    }

    /// <summary>
    /// Get the number of running jobs.
    /// </summary>
    public uint NumRunning
    {
      get
      {
        lock (this.Mutex)
        {
          uint running = 0;
          foreach (Job job in _jobs)
          {
            if (true == job.Running)
              ++running;
          }
          return running;
        }
      }
    }

    /// <summary>
    /// Get the number of jobs.
    /// </summary>
    public uint NumJobs
    {
      get
      {
        lock (this.Mutex)
        {
          System.Diagnostics.Debug.Assert(_jobs.Count >= 0);
          return (uint)_jobs.Count;
        }
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    public object Mutex
    {
      get { return _mutex; }
    }

    /// <summary>
    /// Get the job list.
    /// </summary>
    public JobList Jobs
    {
      get { return _jobs; }
    }
  }
}
