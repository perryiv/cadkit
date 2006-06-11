
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace OpenGL
  {
    public partial class Canvas : System.Windows.Forms.Panel
    {
      /// <summary>
      /// Background color
      /// </summary>
      public override System.Drawing.Color BackColor
      {
        get { return base.BackColor; }
        set
        {
          base.BackColor = value;
          CadKit.Persistence.Registry.instance().setColor( this.GetType().ToString(), "BackgroundColor", value );
        }
      }

      /// <summary>
      /// Pixel format property.
      /// </summary>
      [
      System.ComponentModel.Category( "OpenGL" ),
      System.ComponentModel.Description( "Pixel format description" ),
      System.ComponentModel.Browsable( true )
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
      System.ComponentModel.Category( "OpenGL" ),
      System.ComponentModel.Description( "Pixel format descriptions" ),
      System.ComponentModel.Browsable( true )
      ]
      public System.Data.DataTable PixelFormats
      {
        get
        {
          if ( null == _pixelFormats )
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
          if ( null != parent && null != parent.Size )
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
    }
  }
}
