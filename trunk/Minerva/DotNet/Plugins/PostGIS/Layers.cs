
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.LayerManager
{
  public partial class Layers : 
    WeifenLuo.WinFormsUI.DockContent,
    CadKit.Interfaces.IDocumentView,
    CadKit.Interfaces.IPropertyGridObject
  {
    private const float _offset = 0.0f;
    private CadKit.Interfaces.IDocument _document = null;


    /// <summary>
    /// Constructor.
    /// </summary>
    public Layers()
    {
      InitializeComponent();

      this.DockableAreas = 
        WeifenLuo.WinFormsUI.DockAreas.DockBottom | 
        WeifenLuo.WinFormsUI.DockAreas.DockLeft | 
        WeifenLuo.WinFormsUI.DockAreas.DockRight | 
        WeifenLuo.WinFormsUI.DockAreas.DockTop | 
        WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockRight;
      this.HideOnClose = true;

      _addLayerButton.ToolTipText = "Add Layer";
      _removeButton.ToolTipText = "Remove Layer";
      _updateButton.ToolTipText = "Update Layers";
      _upButton.ToolTipText = "Move Layer Up";
      _downButton.ToolTipText = "Move Layer Down";

      this._setUpListView();

      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += this._activeDocumentChanged;

      this._setEnabled(true);
    }


    /// <summary>
    /// 
    /// </summary>
    private void _setUpListView()
    {
      _listView.Clear();
      _listView.AllowColumnReorder = true;
      _listView.CheckBoxes = true;
      _listView.Columns.Add("Name");
      _listView.Columns.Add("Number");
      _listView.Columns.Add("Min");
      _listView.Columns.Add("Max");
      _listView.View = System.Windows.Forms.View.Details;
      _listView.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(_itemCheckChanged);
      _listView.SelectedIndexChanged += new System.EventHandler(_listBox_SelectedIndexChanged);
      _listView.ContextMenuStrip = this._buildContextMenu();
    }


    /// <summary>
    /// 
    /// </summary>
    void _activeDocumentChanged(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc)
    {
      if (oldDoc == newDoc)
        return;

      _document = newDoc;

      //if (false == _listView.IsHandleCreated)
      {
        _listView.Items.Clear();

        if (newDoc is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)newDoc;

          CadKit.Interfaces.ILayer[] layers = layerList.Layers;

          _listView.ItemCheck -= this._itemCheckChanged;
          foreach (CadKit.Interfaces.ILayer layer in layers)
          {
            System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(layer.Name);
            this._addSubItems(layer, item);

            item.Checked = layer.Shown;
            _listView.Items.Add(item);
          }
          _listView.ItemCheck += this._itemCheckChanged;

          this._setEnabled(true);
        }
        else
        {
          this._setEnabled(false);
        }

        _listView.Invalidate();
      }
    }

    void _setEnabled(bool b)
    {
      //_listView.Enabled = b;
      _addLayerButton.Enabled = b;
      _removeButton.Enabled = b;
      _upButton.Enabled = b;
      _downButton.Enabled = b;
      _updateButton.Enabled = b;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _addLayerButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        AddLayerForm addLayer = new AddLayerForm();

        if (addLayer.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        {
          CadKit.Interfaces.ILayer layer = addLayer.Layer;

          if (null != layer)
          {
            if (_document is CadKit.Interfaces.ILayerList)
            {
              // Don't need to call layer.show because it's called int _listBox_ItemCheck.

              CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

              // Add the layer.
              //layer.RenderBin = 100 + _layers.Count;
              layerList.addLayer(layer, this);
              
              System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(layer.Name);
              this._addSubItems(layer, item);

              item.Checked = true;
              _listView.Items.Add(item);
              _listView.Invalidate();

            }
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 646331520: Exception caught while trying to add a layer: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Add sub-items for list view item..
    /// </summary>
    private void _addSubItems(CadKit.Interfaces.ILayer layer, System.Windows.Forms.ListViewItem item)
    {
      item.SubItems.Add("");
      item.SubItems.Add("");
      item.SubItems.Add("");
    }


    /// <summary>
    /// Set sub-item's text with current values from the layer.
    /// </summary>
    private void _setSubItems(int index)
    {
      //DT.Minerva.Layers.Controls.BaseLayer layer = _layers[index];
      //System.Windows.Forms.ListViewItem item = _listView.Items[index];
      
      //item.SubItems[1].Text = layer.Number;
      //item.SubItems[2].Text = layer.Min;
      //item.SubItems[3].Text = layer.Max;
    }


    /// <summary>
    /// 
    /// </summary>
    void _listBox_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      try
      {
        CadKit.Documents.Manager.Instance.ActiveView = this;
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 2270625540: Exception caught while trying to change selected index.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _itemCheckChanged(object sender, System.Windows.Forms.ItemCheckEventArgs e)
    {
      try
      {
        int index = e.Index;

        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
          CadKit.Interfaces.ILayer layer = layerList.Layers[index] as CadKit.Interfaces.ILayer;

          if (null != layerList && null != layer)
          {
            if (e.NewValue == System.Windows.Forms.CheckState.Checked)
            {
              _listView.Items[index].ForeColor = System.Drawing.Color.Black;
              layerList.showLayer(layer, this);
            }
            else if (e.NewValue == System.Windows.Forms.CheckState.Unchecked)
            {
              _listView.Items[index].ForeColor = System.Drawing.Color.Gray;
              layerList.hideLayer(layer, this);
            }
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 3400305770: Exception caught while trying to change visibility of a layer.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _removeButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

          System.Windows.Forms.ListView.SelectedIndexCollection indexCollection = _listView.SelectedIndices;

          foreach (int index in indexCollection)
          {
            this._removeLayer(index, layerList);
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 2183838408: Exception thrown while trying to remove layer." +
          System.Environment.NewLine + "Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Remove layer at given index.
    /// </summary>
    private void _removeLayer(int index, CadKit.Interfaces.ILayerList layerList)
    {
      CadKit.Interfaces.ILayer layer = layerList.Layers[index] as CadKit.Interfaces.ILayer;

      if (null != layer)
      {
        _listView.Items.RemoveAt(index);
        layerList.removeLayer(layer,this);
      }
    }


    /// <summary>
    /// Update all layers.
    /// </summary>
    private void _updateButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

          object[] layers = layerList.Layers;
          for (int i = 0; i < layers.Length; ++i)
          {
            CadKit.Interfaces.ILayer layer = layers[i] as CadKit.Interfaces.ILayer;

            if (null != layer)
            {
              layerList.modifyLayer(layer, this);
              _listView.Items[i].Text = layer.Name;
              this._setSubItems(i);
            }
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 394702580: Exception caught while trying to update layers.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Build the context menu.
    /// </summary>
    private System.Windows.Forms.ContextMenuStrip _buildContextMenu()
    {
      System.Windows.Forms.ContextMenuStrip menu = new System.Windows.Forms.ContextMenuStrip();
      menu.ShowImageMargin = false;
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Save";
        item.Click += new System.EventHandler(_save);
        menu.Items.Add(item);
      }
      menu.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Remove";
        item.Click += new System.EventHandler(_removeClick);
        menu.Items.Add(item);
      }
      menu.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Update";
        item.Click += new System.EventHandler(_updateSelectedItems);
        menu.Items.Add(item);
      }
      return menu;
    }


    /// <summary>
    /// 
    /// </summary>
    void _save(object sender, System.EventArgs e)
    {
      if (_document is CadKit.Interfaces.ILayerList)
      {
        CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

        CadKit.Interfaces.ILayer[] layers = layerList.Layers;
        foreach (int i in _listView.SelectedIndices)
        {
          CadKit.Interfaces.ILayer layer = layers[i];

          if (null != layer)
          {
            DT.Minerva.Layers.Controls.Factory.Instance.addType(layer.Name, layer);
          }
        }
      }
    }


    /// <summary>
    /// Update only layers that are selected.
    /// </summary>
    void _updateSelectedItems(object sender, System.EventArgs e)
    {
      try
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

          CadKit.Interfaces.ILayer[] layers = layerList.Layers;
          foreach (int index in _listView.SelectedIndices)
          {
            CadKit.Interfaces.ILayer layer = layers[index];

            if (null != layer)
            {
              layerList.modifyLayer(layer, this);
              this._setSubItems(index);
            }
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 2700916252: Exception caught while trying to update layer(s).");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void _removeClick(object sender, System.EventArgs e)
    {
      try
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
          foreach (int index in _listView.SelectedIndices)
          {
            this._removeLayer(index,layerList);
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 1974321048: Exception caught while trying to remove layer(s).");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Move selected layer up.
    /// </summary>
    private void _upButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        // Disable check changed handling while resorting items.
        // This prevents redundant adds.
        //_listView.ItemCheck -= _listBox_ItemCheck;

        //foreach (int index in _listView.SelectedIndices)
        //{
        //  if (index > 0)
        //  {
        //    System.Windows.Forms.ListViewItem item = _listView.Items[index];
        //    _listView.Items.RemoveAt(index);
        //    _listView.Items.Insert(index - 1, item);

        //    DT.Minerva.Layers.Controls.BaseLayer layer = _layers[index];
        //    _layers.RemoveAt(index);
        //    _layers.Insert(index - 1, layer);
        //  }
        //}

        //_updateRenderBins();

        //_listView.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(_listBox_ItemCheck);
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 4018616504: Exception caught while trying to move an item." +
          System.Environment.NewLine + "Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Update all the render bins.
    /// </summary>
    private void _updateRenderBins()
    {
      //for (int i = 0; i < _layers.Count; ++i)
      //{
        //DT.Minerva.Layers.Controls.BaseLayer layer = _layers[i];
        //layer.RenderBin = 100 + i;
        //layer.Offset = _offset * _layers.Count;
        //layer.update();
      //}
    }


    /// <summary>
    /// Move selected layer down.
    /// </summary>
    private void _downButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        // Disable check changed handling while resorting items.
        // This prevents redundant adds.
        //_listView.ItemCheck -= _listBox_ItemCheck;

        //foreach (int index in _listView.SelectedIndices)
        //{
        //  if (index < _listView.Items.Count)
        //  {
        //    System.Windows.Forms.ListViewItem item = _listView.Items[index];
        //    _listView.Items.RemoveAt(index);
        //    _listView.Items.Insert(index + 1, item);

        //    DT.Minerva.Layers.Controls.BaseLayer layer = _layers[index];
        //    _layers.RemoveAt(index);
        //    _layers.Insert(index + 1, layer);
        //  }
        //}

        //_updateRenderBins();

        //_listView.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(_listBox_ItemCheck);
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 4054554004: Exception caught while trying to move an item." +
          System.Environment.NewLine + "Message: {0}", exception.Message);
      }
    }


    /// <summary>
    /// Get/Set the document.
    /// </summary>
    CadKit.Interfaces.IDocument CadKit.Interfaces.IDocumentView.Document
    {
      get { return _document; }
      set { _document = value; }
    }


    /// <summary>
    /// The document is closing.
    /// </summary>
    void CadKit.Interfaces.IDocumentView.close()
    {
      _listView.Clear();
    }


    /// <summary>
    /// Get the property grid.
    /// </summary>
    object CadKit.Interfaces.IPropertyGridObject.PropertyGridObject
    {
      get
      {
        System.Windows.Forms.ListView.SelectedIndexCollection indexCollection = _listView.SelectedIndices;
        if (indexCollection.Count > 0)
        {
          if (_document is CadKit.Interfaces.ILayerList)
          {
            object layer = ((CadKit.Interfaces.ILayerList)_document).Layers[indexCollection[0]];
            if(layer is CadKit.Interfaces.IPropertyGridObject)
              return ((CadKit.Interfaces.IPropertyGridObject)layer).PropertyGridObject;
          }
        }

        return null;
      }
    }
  }
}