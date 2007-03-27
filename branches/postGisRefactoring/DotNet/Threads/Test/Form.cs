
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Test
{
  public partial class Form : CadKit.Persistence.Form
  {
    private uint _count = 0;

    public Form()
    {
      InitializeComponent();
    }

    private void _onAddJob(object sender, System.EventArgs e)
    {
      CadKit.Threads.Jobs.Job job = new CadKit.Threads.Jobs.Job();
      job.Start += this._threadStart;
      job.Finish += this._threadFinish;
      job.Name = System.String.Format("Job {0}", _count++);
      CadKit.Threads.Jobs.Manager.Instance.add(job);
    }

    private void _onClearJobs(object sender, System.EventArgs e)
    {
      CadKit.Threads.Jobs.Manager.Instance.clear();
    }

    private void _threadStart(CadKit.Threads.Jobs.Job job)
    {
      System.TimeSpan rate = job.Progress.UpdateRate;
      try
      {
        int total = 10000000;
        job.Progress.Minimum = 0;
        job.Progress.Maximum = total;
        for (int i = 0; i < total; ++i)
        {
          job.Progress.Value = i;
        }
        job.Progress.UpdateRate = new System.TimeSpan(0);
        job.Progress.Value = total;
      }
      finally
      {
        job.Progress.UpdateRate = rate;
      }
    }

    private void _threadFinish(CadKit.Threads.Jobs.Job job)
    {
    }
  }
}
