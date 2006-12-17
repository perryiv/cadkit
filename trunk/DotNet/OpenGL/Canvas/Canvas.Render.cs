
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

    public void startRenderTimer(int interval)
    {
      _timer = new System.Windows.Forms.Timer();
      _timer.Interval = interval;
      _timer.Tick += new System.EventHandler(OnTick);
      _timer.Enabled = true;
      _timer.Start();
    }

    private void OnTick(object sender, System.EventArgs e)
    {
      this.render();
    }

    public void endRenderTimer()
    {
      _timer.Enabled = false;
      _timer.Stop();
      _timer = null;

    }
  }
}
