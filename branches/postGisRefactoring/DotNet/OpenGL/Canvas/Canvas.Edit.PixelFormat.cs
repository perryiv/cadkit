
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
    /// Called when the context menu button is selected.
    /// </summary>
    public virtual void editPixelFormat()
    {
      try
      {
        if (null != _pixelFormatEditor)
        {
          _pixelFormatEditor.Show();
          _pixelFormatEditor.Activate();
        }
        else
        {
          _pixelFormatEditor = new CadKit.OpenGL.DataGridForm(this);
          _pixelFormatEditor.PersistentName = "CanvasPixelFormatEditor";
          CadKit.Tools.ToolWindow.configure(_pixelFormatEditor, this.FindForm(), "Edit Pixel Format", true);
          _pixelFormatEditor.FormClosed += this._pixelFormatEditorClosed;
          _pixelFormatEditor.Show();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2473789126: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// This is called when the pixel format editor is closed.
    /// </summary>
    private void _pixelFormatEditorClosed(object sender, System.EventArgs e)
    {
      _pixelFormatEditor = null;
      this.Invalidate();
    }
  }
}
