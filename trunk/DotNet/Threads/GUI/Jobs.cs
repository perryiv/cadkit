
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.GUI
{
  public partial class Jobs : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate void VoidReturnZeroArguments();
    private delegate void VoidReturnJobArgument(CadKit.Threads.Jobs.Job job);

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();

    /// <summary>
    /// Constructor
    /// </summary>
    public Jobs()
    {
      this.InitializeComponent();
      _layout.Controls.Clear();
      this.reset();

      CadKit.Threads.Jobs.Manager.Instance.JobAdded += this._jobAdded;
      CadKit.Threads.Jobs.Manager.Instance.JobRemoved += this._jobRemoved;
      CadKit.Threads.Jobs.Manager.Instance.JobFinished += this._jobFinished;

      _layout.Resize += this._resized;
    }

    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    private void _resized(object sender, System.EventArgs e)
    {
      System.Diagnostics.Debug.Assert(false == this.InvokeRequired);
      this._resizeControls();
    }

    /// <summary>
    /// Resize the job controls.
    /// </summary>
    private void _resizeControls()
    {
      lock (this.Mutex)
      {
        foreach (System.Windows.Forms.Control control in _layout.Controls)
        {
          this._resizeControl(control as CadKit.Threads.GUI.Job);
        }
      }
    }

    /// <summary>
    /// Resize the job control.
    /// </summary>
    private void _resizeControl(CadKit.Threads.GUI.Job job)
    {
      lock (this.Mutex)
      {
        if (null != job)
        {
          // The value "26" keeps the horizontal scroll bar from 
          // showing when the vertical one shows.
          job.Size = new System.Drawing.Size(_layout.Width - 26, job.Height);
        }
      }
    }

    /// <summary>
    /// Reset the rows from the manager.
    /// </summary>
    private void reset()
    {
      if (true == this.InvokeRequired)
      {
        this.BeginInvoke(new VoidReturnZeroArguments(this.reset));
        return;
      }
      try
      {
        lock (this.Mutex)
        {
          _layout.Controls.Clear();
          lock (CadKit.Threads.Jobs.Manager.Instance.Mutex)
          {
            foreach (CadKit.Threads.Jobs.Job job in CadKit.Threads.Jobs.Manager.Instance.Jobs)
            {
              this._add(job);
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3322731981: {0}", e.Message);
      }
    }

    /// <summary>
    /// Clear the jobs.
    /// </summary>
    private void clear()
    {
      lock (this.Mutex)
      {
        CadKit.Threads.Jobs.Manager.Instance.clear();
      }
    }

    /// <summary>
    /// Called when a job is added.
    /// </summary>
    private void _jobAdded(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        this._add(job);
      }
    }

    /// <summary>
    /// Called when a job is removed.
    /// </summary>
    private void _jobRemoved(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        this._remove(job);
      }
    }

    /// <summary>
    /// Called when a job finishes.
    /// </summary>
    private void _jobFinished(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        this._remove(job);
      }
    }

    /// <summary>
    /// Add the job.
    /// </summary>
    private void _add(CadKit.Threads.Jobs.Job job)
    {
      if (true == this.InvokeRequired)
      {
        this.BeginInvoke(new VoidReturnJobArgument(this._add), new object[] { job });
        return;
      }
      try
      {
        lock (this.Mutex)
        {
          if (null == job)
            return;

          // Should already be added to manager.
          System.Diagnostics.Debug.Assert(true == CadKit.Threads.Jobs.Manager.Instance.Contains(job));

          // Make a new job row.
          CadKit.Threads.GUI.Job row = new CadKit.Threads.GUI.Job(job);

          // Add it to the control.
          _layout.Controls.Add(row);

          // Resize it to fit.
          this._resizeControl(row);

          // Repaint.
          _layout.Invalidate(true);
          _layout.Update();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2946493990: {0}", e.Message);
      }
    }

    /// <summary>
    /// Remove the job.
    /// </summary>
    private void _remove(CadKit.Threads.Jobs.Job job)
    {
      if (true == this.InvokeRequired)
      {
        this.BeginInvoke(new VoidReturnJobArgument(this._remove), new object[] { job });
        return;
      }
      try
      {
        lock (this.Mutex)
        {
          if (null == job)
            return;

          // Should already be removed from manager.
          System.Diagnostics.Debug.Assert(false == CadKit.Threads.Jobs.Manager.Instance.Contains(job));

          // Find the row.
          CadKit.Threads.GUI.Job row = this._findRow(job);

          if (null != row)
          {
            // Remove the job row.
            _layout.Controls.Remove(row);
            _layout.Invalidate(true);
            _layout.Update();
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2867827834: {0}", e.Message);
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    private Job _findRow(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        // Should be true.
        System.Diagnostics.Debug.Assert(false == this.InvokeRequired);

        foreach (System.Windows.Forms.Control control in _layout.Controls)
        {
          CadKit.Threads.GUI.Job row = control as CadKit.Threads.GUI.Job;
          if (null != row)
          {
            if (row.getJob() == job)
            {
              return row;
            }
          }
        }
        return null;
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    private object Mutex
    {
      get { return _mutex; }
    }
  }
}
