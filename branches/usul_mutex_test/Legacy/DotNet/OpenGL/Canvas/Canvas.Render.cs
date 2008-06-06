
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class Canvas
  {
    /// <summary>
    /// Delegate used to re-direct a render.
    /// </summary>
    private delegate void RenderDelegate();


    /// <summary>
    /// Call to repaint.
    /// </summary>
    public void render()
    {
      try
      {
        if (this.InvokeRequired)
        {
          this.BeginInvoke(new RenderDelegate(render));
        }
        else
        {
          // Re-initializes the OpenGL context if needed.
          this.initRenderingContext();

          // Resize the viewport.
          this.resizeOpenGL();

          // Paint the window.
          this.paintOpenGL();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4054174740: {0}", e.Message);
      }
    }


    /// <summary>
    /// Start the render timer.
    /// </summary>
    public void startRenderTimer(int interval)
    {
      System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
      timer.Interval = interval;
      timer.Tick += new System.EventHandler(_onTick);
      timer.Enabled = true;
      using (this.Lock.write())
      {
        _timer = timer;
      }
      timer.Start();
    }


    /// <summary>
    /// Called every timer tick.
    /// </summary>
    private void _onTick(object sender, System.EventArgs args)
    {
      try
      {
        this.render();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2549683770: {0}", e.Message);
      }
    }


    /// <summary>
    /// End any render timer.
    /// </summary>
    public void endRenderTimer()
    {
      System.Windows.Forms.Timer timer = null;
      using (this.Lock.write())
      {
        timer = _timer;
        _timer = null;
      }
      if (null != timer)
      {
        timer.Enabled = false;
        timer.Stop();
      }
    }


    /// <summary>
    /// Set/get the render timer flag.
    /// </summary>
    public bool RenderTimer
    {
      get { using (this.Lock.read()) { return _timer != null && _timer.Enabled; } }
      set
      {
        if (true == value)
        {
          this.startRenderTimer(15);
        }
        else
        {
          this.endRenderTimer();
        }
      }
    }
  }
}
