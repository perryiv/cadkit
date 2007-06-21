
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.ColorEditor
{
  public partial class Editor : WeifenLuo.WinFormsUI.DockContent
  {
    /// <summary>
    /// Data members.
    /// </summary>
    CadKit.Color.Editor _editor = new CadKit.Color.Editor();


    /// <summary>
    /// Constructor
    /// </summary>
    public Editor()
    {
      try
      {
        this.InitializeComponent();

        this.DockableAreas =
          WeifenLuo.WinFormsUI.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.DockAreas.Float;
        this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockBottom;
        this.HideOnClose = true;

        _editor.Dock = System.Windows.Forms.DockStyle.Fill;
        _editor.ColorChanged += this._editorColorChanged;
        _editor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        this.Controls.Add(_editor);

        // Listen for active view.
        CadKit.Documents.Manager.Instance.ActiveViewChanged += this._onActiveViewChanged;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1138863175: {0}", e.Message);
        this.Controls.Clear();
      }
    }


    /// <summary>
    /// Called when the color changed.
    /// </summary>
    void _editorColorChanged(object sender, CadKit.Color.ColorChangedEventArgs args)
    {
      try
      {
        CadKit.Interfaces.IClearColor color = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IClearColor;
        if (null != color)
        {
          color.ClearColor = args.Color;
          System.Windows.Forms.Control control = color as System.Windows.Forms.Control;
          if (null != control)
          {
            control.Invalidate(true);
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3571950654: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the active view changes.
    /// </summary>
    private void _onActiveViewChanged(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView)
    {
      try
      {
        CadKit.Interfaces.IClearColor color = newView as CadKit.Interfaces.IClearColor;
        if (null != color)
        {
          try
          {
            _editor.Color = (System.Drawing.Color)color.ClearColor;
          }
          catch (System.InvalidCastException)
          {
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 6361619200: {0}", e.Message);
      }
    }
  }
}
