
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
    public class PropertyProxy
    {
      /// <summary>
      /// Data members.
      /// </summary>
      private CadKit.OpenGL.Canvas _canvas = null;

      /// <summary>
      /// Constructor
      /// </summary>
      public PropertyProxy(CadKit.OpenGL.Canvas canvas)
      {
        if (null == canvas)
          throw new System.ArgumentException("Error 3538949322: Null canvas given to proxy");
        this._canvas = canvas;
      }

      /// <summary>
      /// OpenGL clear color property.
      /// </summary>
      [
      System.ComponentModel.Category("OpenGL"),
      System.ComponentModel.Description("Background color"),
      System.ComponentModel.Browsable(true),
      ]
      public System.Drawing.Color BackColor
      {
        get { return this._canvas.BackColor; }
        set
        {
          this._canvas.BackColor = value;
          this._canvas.Invalidate();
          this._canvas.Update();
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
        get { return this._canvas.PixelFormat; }
        set
        {
          this._canvas.PixelFormat = value;
          this._canvas.render(); // Have to force it.
        }
      }

      /// <summary>
      /// Size of the control.
      /// </summary>
      [
      System.ComponentModel.Category("OpenGL"),
      System.ComponentModel.Description("Size of the OpenGL viewport"),
      System.ComponentModel.Browsable(true)
      ]
      public System.Drawing.Size Size
      {
        get { return this._canvas.Size; }
        set
        {
          this._canvas.Size = value;
          this._canvas.Invalidate();
          this._canvas.Update();
        }
      }
    }
  }
}
