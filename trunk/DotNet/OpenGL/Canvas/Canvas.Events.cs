
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class Canvas : System.Windows.Forms.Panel
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
      _flags.hasFormClosingCallback = false;
    }
  }
}
