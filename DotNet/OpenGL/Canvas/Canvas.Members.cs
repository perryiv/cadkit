
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
    /// Local types.
    /// </summary>
    private class InnerPanel : System.Windows.Forms.Panel
    {
      public InnerPanel() : base()
      {
        // Getting a flicker when using the color editor...
        //this.SetStyle(System.Windows.Forms.ControlStyles.UserPaint, true);
        //this.SetStyle(System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer, true);
        //this.SetStyle(System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);
      }
    }


    /// <summary>
    /// Data members.
    /// </summary>
    object _mutex = new object();
    Flags _flags = new Flags();
    uint _pixelFormat = 0;
    CadKit.OpenGL.Glue.RenderContext _renderContext = null;
    InnerPanel _innerPanel = null;
    CadKit.Persistence.Form _propertyGridForm = null;
    CadKit.OpenGL.DataGridForm _pixelFormatEditor = null;
    CadKit.Persistence.Form _colorEditorForm = null;
    System.Data.DataTable _pixelFormats = null;
    System.Windows.Forms.Timer _timer = null;
    System.Drawing.Color _clearColor = System.Drawing.Color.FromArgb(1, 0, 0, 0);
  }
}
