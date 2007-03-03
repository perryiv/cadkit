
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
      public InnerPanel()
        : base()
      {
        // Getting a flicker when using the color editor...
        // this.SetStyle(System.Windows.Forms.ControlStyles.UserPaint, true);
        // this.SetStyle(System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer, true);
        // this.SetStyle(System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);
      }
    }


    /// <summary>
    /// Local types for color map.
    /// </summary>
    class Colors : System.Collections.Generic.Dictionary<string, System.Drawing.Color> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();
    private Flags _flags = new Flags();
    private uint _pixelFormat = 0;
    private CadKit.OpenGL.Glue.RenderContext _renderContext = null;
    private InnerPanel _innerPanel = null;
    private CadKit.Persistence.Form _propertyGridForm = null;
    private CadKit.OpenGL.DataGridForm _pixelFormatEditor = null;
    private CadKit.Persistence.Form _colorEditorForm = null;
    private System.Data.DataTable _pixelFormats = null;
    private System.Windows.Forms.Timer _timer = null;
  }
}
