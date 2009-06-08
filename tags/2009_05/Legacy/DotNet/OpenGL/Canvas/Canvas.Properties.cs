
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
    /// Clear color
    /// </summary>
    public virtual System.Drawing.Color ClearColor
    {
      get
      {
        return this.BackColor;
      }
      set
      {
        this.BackColor = value;
        if (null != this.InnerControl)
        {
          this.InnerControl.BackColor = value;
        }
      }
    }


    /// <summary>
    /// Pixel format property.
    /// </summary>
    [
    System.ComponentModel.Category("OpenGL"),
    System.ComponentModel.Description("Pixel format description"),
    System.ComponentModel.Browsable(true)
    ]
    public uint PixelFormat
    {
      get { return _pixelFormat; }
      set
      {
        _pixelFormat = value;
        this._deleteRenderingContext();
      }
    }


    /// <summary>
    /// Pixel format property.
    /// </summary>
    [
    System.ComponentModel.Category("OpenGL"),
    System.ComponentModel.Description("Pixel format descriptions"),
    System.ComponentModel.Browsable(true)
    ]
    public System.Data.DataTable PixelFormats
    {
      get
      {
        if (null == _pixelFormats)
          _pixelFormats = this.availablePixelFormats();
        return _pixelFormats;
      }
    }


    /// <summary>
    /// Size of the control.
    /// </summary>
    public new System.Drawing.Size Size
    {
      get { return base.Size; }
      set
      {
        System.Windows.Forms.Form parent = this.FindForm();
        if (null != parent && null != parent.Size)
        {
          System.Drawing.Size diff = parent.Size - base.Size;
          parent.Size = value + diff;
        }
        else
        {
          base.Size = value;
        }
      }
    }


    /// <summary>
    /// Get the render context.
    /// </summary>
    protected CadKit.OpenGL.Glue.RenderContext RenderContext
    {
      get { return _renderContext; }
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    protected CadKit.Threads.Tools.Lock Lock
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
    /// Return the appropriate object for the property grid.
    /// </summary>
    object CadKit.Interfaces.IPropertyGridObject.PropertyGridObject
    {
      get { return this.PropertyGridObject; }
    }


    /// <summary>
    /// Return the appropriate object for the property grid.
    /// </summary>
    public virtual object PropertyGridObject
    {
      get { return new CadKit.OpenGL.Canvas.PropertyProxy(this); }
    }


    /// <summary>
    /// Return inner panel.
    /// </summary>
    private InnerPanel InnerControl
    {
      get { using (this.Lock.read()) { return _innerPanel; } }
      set { using (this.Lock.write()) { _innerPanel = value; } }
    }
  }
}
