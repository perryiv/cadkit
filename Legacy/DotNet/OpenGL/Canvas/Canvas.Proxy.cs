
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
      System.ComponentModel.Category("Graphics"),
      System.ComponentModel.Description("Background color for window"),
      System.ComponentModel.Browsable(true),
      ]
      public virtual System.Drawing.Color ClearColor
      {
        get { return this._canvas.ClearColor; }
        set
        {
          this._canvas.ClearColor = value;
          this._update();
        }
      }


      /// <summary>
      /// Pixel format property.
      /// </summary>
      [
      System.ComponentModel.Category("Graphics"),
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
      System.ComponentModel.Category("Graphics"),
      System.ComponentModel.Description("Size of the window"),
      System.ComponentModel.Browsable(true)
      ]
      public System.Drawing.Size Size
      {
        get { return this._canvas.Size; }
        set
        {
          this._canvas.Size = value;
          this._update();
        }
      }


      /// <summary>
      /// Update the panel.
      /// </summary>
      private void _update()
      {
        _canvas.Invalidate(true);
        _canvas.Update();
      }
    }
  }
}
