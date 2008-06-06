
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
    private CadKit.Threads.Tools.Lock _lock = null;
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
      get { using (this.Lock.read()) { return _notifyAdd; } }
      set { using (this.Lock.write()) { _notifyAdd = value; } }
    }

    /// <summary>
    /// Set/get the notify-remove delegate.
    /// </summary>
    public CollectionChangedDelegate JobRemoved
    {
      get { using (this.Lock.read()) { return _notifyRemove; } }
      set { using (this.Lock.write()) { _notifyRemove = value; } }
    }

    /// <summary>
    /// Set/get the notify-finish delegate.
    /// </summary>
    public CollectionChangedDelegate JobFinished
    {
      get { using (this.Lock.read()) { return _notifyFinish; } }
      set { using (this.Lock.write()) { _notifyFinish = value; } }
    }

    /// <summary>
    /// Add a job.
    /// </summary>
    public void add(CadKit.Threads.Jobs.Job job)
    {
      if (null == job)
        return;

      if (true == this.Contains(job))
        return;

      using (this.Lock.write())
      {
        _jobs.Add(job);
      }

      job.queue();

      if (null != this.JobAdded)
      {
        this.JobAdded(job);
      }
    }

    /// <summary>
    /// Remove a job.
    /// </summary>
    public void remove(CadKit.Threads.Jobs.Job job)
    {
      if (null == job)
        return;

      if (false == this.Contains(job))
        return;

      using (this.Lock.write())
      {
        _jobs.Remove(job);
      }

      job.cancel();

      if (null != this.JobRemoved)
      {
        this.JobRemoved(job);
      }
    }

    /// <summary>
    /// Remove the job because it is done. Called from the job's thread handler.
    /// </summary>
    public void done(CadKit.Threads.Jobs.Job job)
    {
      using (this.Lock.write())
      {
        _jobs.Remove(job);
      }
      if (null != this.JobFinished)
      {
        this.JobFinished(job);
      }
    }

    /// <summary>
    /// Clear all the jobs.
    /// </summary>
    public void clear()
    {
      while (this.NumJobs > 0)
      {
        CadKit.Threads.Jobs.Job job = null;
        using (this.Lock.read())
        {
          job = _jobs[_jobs.Count - 1];
        }
        this.remove(job);
      }
    }

    /// <summary>
    /// See if the job is in the list.
    /// </summary>
    public bool Contains(CadKit.Threads.Jobs.Job job)
    {
      using (this.Lock.read())
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
        uint running = 0;
        JobList jobs = this.JobsCopy;
        foreach (Job job in jobs)
        {
          if (true == job.Running)
          {
            ++running;
          }
        }
        return running;
      }
    }

    /// <summary>
    /// Get the number of jobs.
    /// </summary>
    public uint NumJobs
    {
      get { using (this.Lock.read()) { return (uint)_jobs.Count; } }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    public CadKit.Threads.Tools.Lock Lock
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
    /// Get a copy of the job list.
    /// </summary>
    public JobList JobsCopy
    {
      get
      {
        JobList jobs = new JobList();
        using (this.Lock.read())
        {
          _jobs.AddRange(jobs);
        }
        return jobs;
      }
    }
  }
}
