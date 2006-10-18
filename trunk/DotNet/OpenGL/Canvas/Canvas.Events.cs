
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
    /// Paint the background. We override this to prevent flicker.
    /// </summary>
    protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs a)
    {
    }

    /// <summary>
    /// Called when the control needs to be painted.
    /// </summary>
    protected override void OnPaint(System.Windows.Forms.PaintEventArgs a)
    {
      this.render();
    }

    /// <summary>
    /// Called when the control is resized.
    /// </summary>
    public virtual void OnResize(object sender, System.EventArgs e)
    {
      // Sizing a form before it is shown will generate a resize event. 
      // That is too soon to render.
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent && true == parent.Visible && true == parent.IsHandleCreated)
        this.render();
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditProperties(object sender, System.EventArgs e)
    {
      this.editProperties();
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditBackgroundColor(object sender, System.EventArgs e)
    {
      this.editBackgroundColor();
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void OnEditPixelFormat(object sender, System.EventArgs e)
    {
      this.editPixelFormat();
    }

    /// <summary>
    /// Called before the parent form closes.
    /// </summary>
    public virtual void OnFormClosing(object sender, System.EventArgs e)
    {
      this._deleteRenderingContext();
    }


    /// <summary>
    /// A key was pressed.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnKeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
    {
      this.OnKeyPress(e);
    }

    /// <summary>
    /// Key is down.  Delegate
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnKeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
    {
      this.OnKeyDown(e);
    }


    /// <summary>
    /// The mouse wheel moved.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnMouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      this.OnMouseWheel(e);
    }


    /// <summary>
    /// A mouse button was pressed.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      this.OnMouseDown(e);
    }


    /// <summary>
    /// A mouse button was released.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      this.OnMouseUp(e);
    }


    /// <summary>
    /// The mouse has moved.  Delegate.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnMouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      this.OnMouseMove(e);
    }
 }
}
