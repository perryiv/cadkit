
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Jobs
{
  public class Progress : 
    CadKit.Referenced.Base,
    CadKit.Interfaces.IProgressBar
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void NotifyDelegate(CadKit.Threads.Jobs.Job job);


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = null;
    private int _min = 0;
    private int _max = 100;
    private int _value = 0;
    private string _text = null;
    private System.DateTime _lastTime = System.DateTime.Now;
    private System.TimeSpan _updateRate = new System.TimeSpan(0, 0, 0, 1, 0);
    private CadKit.Threads.Jobs.Job _job = null;
    private NotifyDelegate _notifyDelegate = null;


    /// <summary>
    /// Constructor
    /// </summary>
    public Progress(CadKit.Threads.Jobs.Job job)
    {
      if (null == job)
        throw new System.ArgumentNullException("Error 3494865470: Null job given", (System.Exception)null);

      _job = job;
    }


    /// <summary>
    /// Called when the reference count goes to zero.
    /// </summary>
    protected override void _cleanup()
    {
      try
      {
        _lock = null;
        _text = null;
        _job = null;
        _notifyDelegate = null;
        base._cleanup();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1615620548: {0}", e.Message);
      }
    }


    /// <summary>
    /// Notify about progress.
    /// </summary>
    private void _notify()
    {
      System.DateTime now = System.DateTime.Now;
      System.DateTime last = this.LastTime;
      System.TimeSpan rate = this.UpdateRate;

      if (now > last + rate)
      {
        this.LastTime = now;

        if (null != this.Notify)
        {
          this.Notify(_job);
        }

        System.Diagnostics.Debug.Assert(_job.ThreadId == System.Threading.Thread.CurrentThread.ManagedThreadId);
        //System.Console.WriteLine(System.String.Format("{0}: Thread = {1}, Progress = {2}", _job.Name, _job.ThreadId, (uint) this.Percent));
      }
    }


    /// <summary>
    /// Get/set the notification delegate.
    /// </summary>
    public NotifyDelegate Notify
    {
      get { using (this.Lock.read()) { return _notifyDelegate; } }
      set { using (this.Lock.write()) { _notifyDelegate = value; } }
    }


    /// <summary>
    /// Get/set the minimum.
    /// </summary>
    public int Minimum
    {
      get { using (this.Lock.read()) { return _min; } }
      set { using (this.Lock.write()) { _min = value; } }
    }


    /// <summary>
    /// Get/set the maximum.
    /// </summary>
    public int Maximum
    {
      get { using (this.Lock.read()) { return _max; } }
      set { using (this.Lock.write()) { _max = value; } }
    }


    /// <summary>
    /// Get/set the value.
    /// </summary>
    public int Value
    {
      get { using (this.Lock.read()) { return _value; } }
      set
      {
        using (this.Lock.write())
        {
          _value = value;
        }
        this._notify();
      }
    }


    /// <summary>
    /// Get the fraction completed in the range [0.0,1.0].
    /// </summary>
    public float Fraction
    {
      get
      {
        float min = this.Minimum;
        float max = this.Maximum;
        float value = this.Value;
        return ((value - min) / (max - min));
      }
    }


    /// <summary>
    /// Get the percentage completed in the range [0.0,100.0].
    /// </summary>
    public float Percent
    {
      get { return (100.0f * this.Fraction); }
    }


    /// <summary>
    /// Get/set the last time the value was updated.
    /// </summary>
    public System.DateTime LastTime
    {
      get { using (this.Lock.read()) { return _lastTime; } }
      set { using (this.Lock.write()) { _lastTime = value; } }
    }


    /// <summary>
    /// Get/set the update-rate.
    /// </summary>
    public System.TimeSpan UpdateRate
    {
      get { using (this.Lock.read()) { return _updateRate; } }
      set { using (this.Lock.write()) { _updateRate = value; } }
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


    /// <summary>
    /// Get/set the text.
    /// </summary>
    string CadKit.Interfaces.IProgressBar.Text
    {
      get { return this.Text; }
      set { this.Text = value; }
    }


    /// <summary>
    /// Get/set the text.
    /// </summary>
    public string Text
    {
      get { using (this.Lock.read()) { return _text; } }
      set { using (this.Lock.write()) { _text = value; } }
    }


    /// <summary>
    /// Get the renge.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Range
    {
      get { return (this.Maximum - this.Minimum); }
    }


    /// <summary>
    /// Get/set the value.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Value
    {
      get { return this.Value; }
      set { this.Value = value; }
    }
  }
}
