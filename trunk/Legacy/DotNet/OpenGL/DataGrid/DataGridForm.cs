
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class DataGridForm : CadKit.Persistence.Form
  {
    private CadKit.OpenGL.Canvas _canvas;

    public DataGridForm(CadKit.OpenGL.Canvas canvas)
      : base()
    {
      if (null == canvas)
        throw new System.ArgumentNullException("Error 1443852070: Canvas is null", null as System.Exception);
      _canvas = canvas;
      InitializeComponent();
      this._fillGrid();
    }

    void _fillGrid()
    {
      this.SuspendLayout();
      try
      {
        _dataGrid.DataSource = _canvas.PixelFormats;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2192974644: {0}\n{1}", e.Message, e.StackTrace);
      }
      finally
      {
        this.ResumeLayout(false);
      }
    }
  }
}
