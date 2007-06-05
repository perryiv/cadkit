
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.Properties
{
  public partial class Editor : WeifenLuo.WinFormsUI.Docking.DockContent
  {
    /// <summary>
    /// Data members.
    /// </summary>
    System.Windows.Forms.PropertyGrid _grid = new System.Windows.Forms.PropertyGrid();


    /// <summary>
    /// Constructor
    /// </summary>
    public Editor()
    {
      try
      {
        this.InitializeComponent();

        this.DockAreas =
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.Docking.DockAreas.Float;
        this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
        this.HideOnClose = true;

        _grid.Dock = System.Windows.Forms.DockStyle.Fill;
        _grid.PropertyValueChanged += this._propertyValueChanged;
        this.Controls.Add(_grid);

        // Listen for active view.
        CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 6126340020: {0}", e.Message);
        this.Controls.Clear();
      }
    }


    /// <summary>
    /// Called when the property changed.
    /// </summary>
    void _propertyValueChanged(object s, System.Windows.Forms.PropertyValueChangedEventArgs e)
    {
      System.Windows.Forms.Control control = CadKit.Documents.Manager.Instance.ActiveView as System.Windows.Forms.Control;
      if (null != control)
      {
        control.Invalidate(true);
      }
    }


    /// <summary>
    /// Called when the active view changes.
    /// </summary>
    private void _onActiveViewChanged(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView)
    {
      try
      {
        try
        {
          CadKit.Interfaces.IPropertyGridObject property = newView as CadKit.Interfaces.IPropertyGridObject;
          _grid.SelectedObject = (null == property) ? null : property.PropertyGridObject;
        }
        catch (System.InvalidCastException)
        {
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2589487393: {0}", e.Message);
      }
    }
  }
}
