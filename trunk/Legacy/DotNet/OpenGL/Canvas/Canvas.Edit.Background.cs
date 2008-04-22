
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
    public virtual void editBackgroundColor()
    {
      try
      {
        if (null != _colorEditorForm)
        {
          _colorEditorForm.Show();
          _colorEditorForm.Activate();
          return;
        }

        else
        {
          string persistentName = "CanvasColorWheel";

          // Make the color editor.
          CadKit.Color.Editor editor = new CadKit.Color.Editor();
          editor.PersistentName = persistentName;
          editor.Dock = System.Windows.Forms.DockStyle.Fill;
          editor.Color = this.BackColor;
          editor.ColorChanged += this._colorChanged;
          editor.ColorEditDone += this._colorDoneChanging;

          // Make new container form.
          _colorEditorForm = new CadKit.Persistence.Form();
          _colorEditorForm.PersistentName = persistentName;
          _colorEditorForm.Controls.Add(editor);
          CadKit.Tools.ToolWindow.configure(_colorEditorForm, this.FindForm(), "Edit Background", true);
          _colorEditorForm.FormClosed += this._colorEditorClosed;
          _colorEditorForm.Show();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 6369872130: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Called when the color is done changing.
    /// </summary>
    void _colorDoneChanging(object sender, CadKit.Color.ColorChangedEventArgs e)
    {
      this._updatePropertyGrid();
    }

    /// <summary>
    /// Called when the color changes.
    /// </summary>
    private void _colorChanged(object sender, CadKit.Color.ColorChangedEventArgs e)
    {
      this.BackColor = e.Color;
      this.Invalidate();
    }

    /// <summary>
    /// This is called when the color editor is closed.
    /// </summary>
    private void _colorEditorClosed(object sender, System.EventArgs e)
    {
      _colorEditorForm = null;
      this.Invalidate();
    }

    /// <summary>
    /// Called to update the color editor.
    /// </summary>
    private void _updateColorEditor()
    {
      if (null != _colorEditorForm)
      {
        foreach (System.Windows.Forms.Control control in _colorEditorForm.Controls)
        {
          CadKit.Color.Editor editor = control as CadKit.Color.Editor;
          if (null != editor)
          {
            editor.Color = this.BackColor;
            editor.Invalidate();
          }
        }
      }
    }
  }
}
