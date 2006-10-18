
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
    /// Data members.
    /// </summary>
    Flags _flags = new Flags();
    uint _pixelFormat = 0;
    CadKit.OpenGL.Glue.RenderContext _renderContext = null;
    System.Windows.Forms.Panel _innerPanel = null;
    CadKit.Persistence.Form _propertyGridForm = null;
    CadKit.OpenGL.DataGridForm _pixelFormatEditor = null;
    CadKit.Persistence.Form _colorEditorForm = null;
    System.Data.DataTable _pixelFormats = null;
    System.Windows.Forms.Timer _timer = null;
  }
}
