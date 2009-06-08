
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Ossim
{
  public partial class Form : WeifenLuo.WinFormsUI.DockContent
  {
    private object _mutex = new object();

    /// <summary>
    /// 
    /// </summary>
    public Form()
    {
      InitializeComponent();

      this.DockableAreas =
          WeifenLuo.WinFormsUI.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockBottom;
      this.HideOnClose = true;

      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += this._activeDocumentChanged;

      _listView.AllowColumnReorder = true;
      _listView.View = System.Windows.Forms.View.Details;
      _listView.Columns.Add("File");
      _listView.CheckBoxes = true;
      _listView.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(_listView_ItemCheck);
      _listView.DoubleClick += new System.EventHandler(_listView_DoubleClick);

      this._buildToolbar();
    }


    /// <summary>
    /// 
    /// </summary>
    void _listView_DoubleClick(object sender, System.EventArgs e)
    {
    }


    /// <summary>
    /// 
    /// </summary>
    void _listView_ItemCheck(object sender, System.Windows.Forms.ItemCheckEventArgs e)
    {
      System.Windows.Forms.ListViewItem item = _listView.Items[e.Index];
      CadKit.Interfaces.IImageLayer addImageLayer = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IImageLayer;
      if (addImageLayer != null)
      {
        if (e.NewValue == System.Windows.Forms.CheckState.Checked)
          addImageLayer.showImageLayer(item.Text);
        else if (e.NewValue == System.Windows.Forms.CheckState.Unchecked)
          addImageLayer.hideImageLayer(item.Text);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _activeDocumentChanged(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc)
    {
      CadKit.Interfaces.IImageLayer imageLayer = newDoc as CadKit.Interfaces.IImageLayer;
      _listView.Enabled = (imageLayer != null);
    }


    /// <summary>
    /// 
    /// </summary>
    private void _buildToolbar()
    {
      lock (_mutex)
      {
        this._addToolbarButton(_toolStrip, new Commands.AddLayerCommand(this, this));
        this._addToolbarButton(_toolStrip, new Commands.RemoveLayerCommand(this, this));
      }
    }


    /// <summary>
    /// Add the tool-bar button.
    /// </summary>
    protected void _addToolbarButton(System.Windows.Forms.ToolStrip strip, CadKit.Commands.Command command)
    {
      lock (_mutex)
      {
        if (null != strip && null != command)
        {
          strip.Items.Add(command.ToolButton);
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public void addImageLayer(string fileName)
    {
      CadKit.Interfaces.IImageLayer addImageLayer = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IImageLayer;
      if (addImageLayer != null)
      {
        addImageLayer.addImageLayer(fileName);
        System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(fileName);
        item.Checked = true;
        _listView.Items.Add(item);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public void removeSelectedLayers()
    {
      CadKit.Interfaces.IImageLayer imageLayer = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IImageLayer;
      if (imageLayer != null)
      {
        foreach(System.Windows.Forms.ListViewItem item in _listView.Items)
          imageLayer.removeImageLayer(item.Text);
      }
    }
  }
}
