
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
    public new event System.EventHandler Click;


    /// <summary>
    /// 
    /// </summary>
    protected void OnClick(object sender, System.EventArgs e)
    {
      try
      {
        this.Click(this, e);
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 3895419088: {0}", ex.Message);
      }
    }


    /// <summary>
    /// Paint the background. We override this to prevent flicker.
    /// </summary>
    protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs a)
    {
    }


    /// <summary>
    /// Called when the control needs to be painted.  Override this because painting done in inner panel.
    /// </summary>
    protected override void OnPaint(System.Windows.Forms.PaintEventArgs args)
    {
    }


    /// <summary>
    /// Called when the inner panel needs painting.
    /// </summary>
    private void _paint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        this.render();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2296032136: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    public virtual void OnResize(object sender, System.EventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          // Sizing a form before it is shown will generate a resize event. 
          // That is too soon to render.
          System.Windows.Forms.Form parent = this.FindForm();
          if (null != parent && true == parent.Visible && true == parent.IsHandleCreated)
          {
            this.render();
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1921802922: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditProperties(object sender, System.EventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.editProperties();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3043831848: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditBackgroundColor(object sender, System.EventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.editBackgroundColor();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1478840786: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditPixelFormat(object sender, System.EventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.editPixelFormat();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1718323897: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called before the parent form closes.
    /// </summary>
    public virtual void OnFormClosing(object sender, System.EventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this._deleteRenderingContext();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3582908140: {0}", e.Message);
      }
    }


    /// <summary>
    /// A key was pressed.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnKeyPress(object sender, System.Windows.Forms.KeyPressEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnKeyPress(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3405793112: {0}", e.Message);
      }
    }

    /// <summary>
    /// Key is down.  Delegate
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnKeyDown(object sender, System.Windows.Forms.KeyEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnKeyDown(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2771064075: {0}", e.Message);
      }
    }


    /// <summary>
    /// The mouse wheel moved.  Delegate.
    /// </summary>
    private void OnMouseWheel(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnMouseWheel(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4147551851: {0}", e.Message);
      }
    }


    /// <summary>
    /// A mouse button was pressed.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnMouseDown(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnMouseDown(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4190868247: {0}", e.Message);
      }
    }


    /// <summary>
    /// A mouse button was released.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnMouseUp(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnMouseUp(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3572890167: {0}", e.Message);
      }
    }


    /// <summary>
    /// The mouse has moved.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnMouseMove(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      try
      {
        if (false == this.InvokeRequired)
        {
          this.OnMouseMove(args);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2544964694: {0}", e.Message);
      }
    }
  }
}
