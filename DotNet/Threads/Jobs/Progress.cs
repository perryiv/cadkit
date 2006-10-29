
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Jobs
{
  public class Progress : CadKit.Interfaces.IProgressBar
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public delegate void NotifyDelegate(CadKit.Threads.Jobs.Job job);

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
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
    public Progress ( CadKit.Threads.Jobs.Job job )
    {
      if (null == job)
        throw new System.ArgumentNullException("Error 3494865470: Null job given", (System.Exception) null);

      _job = job;
    }

    /// <summary>
    /// Notify about progress.
    /// </summary>
    private void _notify()
    {
      lock (this.Mutex)
      {
        System.DateTime now = System.DateTime.Now;
        if (now > this.LastTime + this.UpdateRate)
        {
          this.LastTime = now;

          if (null != _notifyDelegate)
            _notifyDelegate(_job);

          System.Diagnostics.Debug.Assert ( _job.ThreadId == System.Threading.Thread.CurrentThread.ManagedThreadId );
          //System.Console.WriteLine(System.String.Format("{0}: Thread = {1}, Progress = {2}", _job.Name, _job.ThreadId, (uint) this.Percent));
        }
      }
    }

    /// <summary>
    /// Get/set the notification delegate.
    /// </summary>
    public NotifyDelegate Notify
    {
      get { lock (this.Mutex) { return _notifyDelegate; } }
      set { lock (this.Mutex) { _notifyDelegate = value; } }
    }

    /// <summary>
    /// Get/set the minimum.
    /// </summary>
    public int Minimum
    {
      get { lock (this.Mutex) { return _min; } }
      set { lock (this.Mutex) { _min = value; } }
    }

    /// <summary>
    /// Get/set the maximum.
    /// </summary>
    public int Maximum
    {
      get { lock (this.Mutex) { return _max; } }
      set { lock (this.Mutex) { _max = value; } }
    }

    /// <summary>
    /// Get/set the value.
    /// </summary>
    public int Value
    {
      get { lock (this.Mutex) { return _value; } }
      set
      {
        lock (this.Mutex)
        {
          _value = value;
          this._notify();
        }
      }
    }

    /// <summary>
    /// Get the fraction completed in the range [0.0,1.0].
    /// </summary>
    public float Fraction
    {
      get
      {
        lock (this.Mutex)
        {
          float min = this.Minimum;
          float max = this.Maximum;
          float value = this.Value;
          return ((value - min) / (max - min));
        }
      }
    }

    /// <summary>
    /// Get the percentage completed in the range [0.0,100.0].
    /// </summary>
    public float Percent
    {
      get
      {
        lock (this.Mutex)
        {
          return (100.0f * this.Fraction);
        }
      }
    }

    /// <summary>
    /// Get/set the last time the value was updated.
    /// </summary>
    public System.DateTime LastTime
    {
      get { lock (this.Mutex) { return _lastTime; } }
      set { lock (this.Mutex) { _lastTime = value; } }
    }

    /// <summary>
    /// Get/set the update-rate.
    /// </summary>
    public System.TimeSpan UpdateRate
    {
      get { lock (this.Mutex) { return _updateRate; } }
      set { lock (this.Mutex) { _updateRate = value; } }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    private object Mutex
    {
      get { return _mutex; }
    }

    /// <summary>
    /// Get/set the text.
    /// </summary>
    string CadKit.Interfaces.IProgressBar.Text
    {
      get { lock (_mutex) { return _text; } }
      set { lock ( _mutex ) { _text = value; } }
    }

    /// <summary>
    /// Get the renge.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Range
    {
      get { lock (_mutex) { return (this.Maximum - this.Minimum); } }
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
