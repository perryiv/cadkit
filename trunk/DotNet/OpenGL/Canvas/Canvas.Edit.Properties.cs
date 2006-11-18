
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
    public virtual void editProperties()
    {
      try
      {
        // If it is already showing, bring it to the front.
        if (null != _propertyGridForm)
        {
          _propertyGridForm.Show();
          _propertyGridForm.Activate();
          return;
        }
        else
        {
          string persistentName = "CanvasPropertyGrid";

          // Make the property grid.
          System.Windows.Forms.PropertyGrid grid = new System.Windows.Forms.PropertyGrid();
          grid.Dock = System.Windows.Forms.DockStyle.Fill;
          grid.SelectedObject = this.PropertyGridObject;
          grid.PropertyValueChanged += this._propertyValueChanged;

          // Make new container form, add property grid, and show it.
          _propertyGridForm = new CadKit.Persistence.Form();
          _propertyGridForm.PersistentName = persistentName;
          _propertyGridForm.Controls.Add(grid);
          CadKit.Tools.ToolWindow.configure(_propertyGridForm, this.FindForm(), "Edit OpenGL Canvas", true);
          _propertyGridForm.FormClosed += this._propertyGridClosed;
          _propertyGridForm.Show();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1108970309: {0}\n{1}", e.Message, e.StackTrace);
      }
    }


    /// <summary>
    /// Called when a property is changed.
    /// </summary>
    private void _propertyValueChanged(object s, System.Windows.Forms.PropertyValueChangedEventArgs e)
    {
      this._updateColorEditor();
    }


    /// <summary>
    /// This is called when the property grid is closed.
    /// </summary>
    private void _propertyGridClosed(object sender, System.EventArgs e)
    {
      _propertyGridForm = null;
      this.Invalidate();
    }


    /// <summary>
    /// Called to update the property grid, if it is visible.
    /// </summary>
    private void _updatePropertyGrid()
    {
      if (null != _propertyGridForm)
      {
        foreach (System.Windows.Forms.Control control in _propertyGridForm.Controls)
        {
          System.Windows.Forms.PropertyGrid grid = control as System.Windows.Forms.PropertyGrid;
          if (null != grid)
          {
            object current = grid.SelectedObject;
            grid.SelectedObject = null;
            grid.SelectedObject = current;
            grid.Invalidate();
          }
        }
      }
    }
  }
}
