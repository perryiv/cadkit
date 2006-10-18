
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.GUI
{
  public partial class Job : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate void ProgessNotifyDelegate ( CadKit.Threads.Jobs.Job job );

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private CadKit.Threads.Jobs.Job _job = null;

    /// <summary>
    /// Constructor
    /// </summary>
    public Job(CadKit.Threads.Jobs.Job job)
    {
      if (null == job)
        throw new System.ArgumentNullException("Error 4294967295: Null job given", (System.Exception) null);

      this.InitializeComponent();

      _job = job;
      _job.Start += this._jobStart;
      _job.Finish += this._jobFinish;
      _job.Progress.Notify += this._progressNotify;

      _label.Text = _job.Name;
    }

    /// <summary>
    /// Called when the job starts.
    /// </summary>
    private void _jobStart(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        // Should be true.
        System.Diagnostics.Debug.Assert(job == _job);
      }
    }

    /// <summary>
    /// Called when the job finishes.
    /// </summary>
    private void _jobFinish(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        // Should be true.
        System.Diagnostics.Debug.Assert(job == _job);
      }
    }

    /// <summary>
    /// Get the job.
    /// </summary>
    public CadKit.Threads.Jobs.Job getJob()
    {
      lock (this.Mutex) { return _job; }
    }

    /// <summary>
    /// Called when the job progresses.
    /// </summary>
    private void _progressNotify(CadKit.Threads.Jobs.Job job)
    {
      // See if you need to invoke.
      if (true == this.InvokeRequired)
      {
        this.BeginInvoke(new ProgessNotifyDelegate(this._progressNotify), new object[] { job });
        return;
      }

      try
      {
        lock (this.Mutex)
        {
          // Should be true.
          System.Diagnostics.Debug.Assert(job == _job);

          // Move the progress bar.
          _progressBar.Minimum = _job.Progress.Minimum;
          _progressBar.Maximum = _job.Progress.Maximum;
          _progressBar.Value = _job.Progress.Value;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2088797797: {0}", e.Message);
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
    /// Called when the button is clicked.
    /// </summary>
    private void _buttonRunClick(object sender, System.EventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          System.Diagnostics.Debug.Assert(false == this.InvokeRequired);

          if (null == _job)
          {
            return;
          }
          if (true == _job.Suspended)
          {
            _job.resume();
            this._buttonRun.Image = global::CadKit.Threads.GUI.Properties.Resources.run;
          }
          else if (true == _job.Running)
          {
            _job.pause();
            this._buttonRun.Image = global::CadKit.Threads.GUI.Properties.Resources.pause;
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2504082161: {0}", e.Message);
      }
    }

    /// <summary>
    /// Called when the button is clicked.
    /// </summary>
    private void _buttonRemoveClick(object sender, System.EventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          System.Diagnostics.Debug.Assert(false == this.InvokeRequired);
          if (null != _job)
          {
            CadKit.Threads.Jobs.Manager.Instance.remove(_job);
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2134894630: {0}", e.Message);
      }
    }
  }
}
