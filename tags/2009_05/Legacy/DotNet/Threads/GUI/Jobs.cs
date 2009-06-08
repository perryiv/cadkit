
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
    private class ControlList : System.Collections.Generic.List<System.Windows.Forms.Control> { }

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
      ControlList controls = this.ControlsCopy;
      foreach (System.Windows.Forms.Control control in controls)
      {
        this._resizeControl(control as CadKit.Threads.GUI.Job);
      }
    }

    /// <summary>
    /// Resize the job control.
    /// </summary>
    private void _resizeControl(CadKit.Threads.GUI.Job job)
    {
      if (null != job)
      {
        lock (this.Mutex)
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
      try
      {
        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnZeroArguments(this.reset));
        }
        else
        {
          lock (this.Mutex)
          {
            _layout.Controls.Clear();
          }
          CadKit.Threads.Jobs.Manager.JobList jobs = CadKit.Threads.Jobs.Manager.Instance.JobsCopy;
          foreach (CadKit.Threads.Jobs.Job job in jobs)
          {
            this._add(job);
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
      CadKit.Threads.Jobs.Manager.Instance.clear();
    }

    /// <summary>
    /// Called when a job is added.
    /// </summary>
    private void _jobAdded(CadKit.Threads.Jobs.Job job)
    {
        this._add(job);
    }

    /// <summary>
    /// Called when a job is removed.
    /// </summary>
    private void _jobRemoved(CadKit.Threads.Jobs.Job job)
    {
        this._remove(job);
    }

    /// <summary>
    /// Called when a job finishes.
    /// </summary>
    private void _jobFinished(CadKit.Threads.Jobs.Job job)
    {
        this._remove(job);
    }

    /// <summary>
    /// Add the job.
    /// </summary>
    private void _add(CadKit.Threads.Jobs.Job job)
    {
      try
      {
        if (null == job)
          return;

        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnJobArgument(this._add), new object[] { job });
        }

        else
        {
          // Should already be added to manager.
          System.Diagnostics.Debug.Assert(true == CadKit.Threads.Jobs.Manager.Instance.Contains(job));

          // Make a new job row.
          CadKit.Threads.GUI.Job row = new CadKit.Threads.GUI.Job(job);

          // Add it to the control.
          lock (this.Mutex) { _layout.Controls.Add(row); }

          // Resize it to fit.
          this._resizeControl(row);

          // Repaint.
          lock (this.Mutex)
          {
            _layout.Invalidate(true);
            _layout.Update();
          }
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
      try
      {
        if (null == job)
          return;

        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnJobArgument(this._remove), new object[] { job });
        }

        else
        {
          // Should already be removed from manager.
          System.Diagnostics.Debug.Assert(false == CadKit.Threads.Jobs.Manager.Instance.Contains(job));

          // Find the row.
          CadKit.Threads.GUI.Job row = this._findRow(job);

          if (null != row)
          {
            lock (this.Mutex)
            {
              // Remove the job row.
              _layout.Controls.Remove(row);
              _layout.Invalidate(true);
              _layout.Update();
            }
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
      // Should be true.
      System.Diagnostics.Debug.Assert(false == this.InvokeRequired);

      ControlList controls = this.ControlsCopy;
      foreach (System.Windows.Forms.Control control in controls)
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

    /// <summary>
    /// Get layout panel.
    /// </summary>
    private System.Windows.Forms.FlowLayoutPanel FlowLayoutPanel
    {
      get { lock (this.Mutex) { return _layout; } }
    }

    /// <summary>
    /// Get a copy of the list of child controls.
    /// </summary>
    ControlList ControlsCopy
    {
      get
      {
        System.Windows.Forms.FlowLayoutPanel layout = this.FlowLayoutPanel;
        ControlList newList = new ControlList();
        System.Windows.Forms.Control.ControlCollection currentList = layout.Controls;
        lock (this.Mutex)
        {
          foreach (System.Windows.Forms.Control control in currentList)
          {
            newList.Add(control);
          }
        }
        return newList;
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
