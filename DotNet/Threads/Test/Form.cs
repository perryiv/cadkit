
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Blade.Test
{
  public partial class Form : CadKit.Persistence.Form
  {
    public Form()
    {
      InitializeComponent();
    }

    private void _onAddJob(object sender, System.EventArgs e)
    {
      CadKit.Threads.Jobs.Job job = new CadKit.Threads.Jobs.Job();
      job.Start += this._threadStart;
      job.Finish += this._threadFinish;
      CadKit.Threads.Jobs.Manager.Instance.add(job);
    }

    private void _onClearJobs(object sender, System.EventArgs e)
    {
      CadKit.Threads.Jobs.Manager.Instance.clear();
    }

    private void _threadStart(CadKit.Threads.Jobs.Job job)
    {
      int total = 10000000;
      job.Progress.Minimum = 0;
      job.Progress.Maximum = total;
      for (int i = 0; i < total; ++i)
      {
        job.Progress.Value = i;
        //System.Threading.Thread.Sleep(10);
      }
    }

    private void _threadFinish(CadKit.Threads.Jobs.Job job)
    {
    }
  }
}
