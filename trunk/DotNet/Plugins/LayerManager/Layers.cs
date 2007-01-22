
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager
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

      this._setUpListView();

      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += this._activeDocumentChanged;

      this._buildToolbar();
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _buildToolbar()
    {
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.AddLayerCommand(this, this));
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.RemoveLayerCommand(this, this));
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.UpdateLayersCommand(this, this));
      _toolstrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.MoveLayerUpCommand(this, this));
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.MoveLayerDownCommand(this, this));
      _toolstrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      this._addToolbarButton(_toolstrip, new CadKit.Plugins.LayerManager.Commands.AddLayerToFavoritesCommand(this, this));
    }


    /// <summary>
    /// Add the tool-bar button.
    /// </summary>
    protected void _addToolbarButton(System.Windows.Forms.ToolStrip strip, CadKit.Commands.Command command)
    {
      if (null != strip && null != command)
      {
        strip.Items.Add(command.ToolButton);
      }
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

      this.addLayers();

      _listView.Invalidate();
    }


    /// <summary>
    /// Clear the list view and re-add layers from the document.
    /// </summary>
    public void addLayers()
    {
      try
      {
        // Clear what we have.
        _listView.Items.Clear();

        // If the document is a layer list.
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
          CadKit.Interfaces.ILayer[] layers = layerList.Layers;

          _listView.ItemCheck -= this._itemCheckChanged;
          foreach (CadKit.Interfaces.ILayer layer in layers)
          {
            this._addLayer(layer);
          }
          _listView.ItemCheck += this._itemCheckChanged;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2179058038: {0}", e.Message);
      }
    }


    /// <summary>
    /// Add a layer.
    /// </summary>
    protected void _addLayer(CadKit.Interfaces.ILayer layer)
    {
      try
      {
        if (null != layer)
        {
          System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(layer.Name);

          item.Checked = true;
          _listView.Items.Add(item);

        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 646331520: Exception caught while trying to add a layer: {0}", exception.Message);
      }
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
    public void removeSelectedLayers()
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
    public void updateAllLayers()
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
      //{
      //  System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
      //  item.Text = "Save";
      //  item.Click += new System.EventHandler(_save);
      //  menu.Items.Add(item);
      //}
      //menu.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      //{
      //  System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
      //  item.Text = "Remove";
      //  item.Click += new System.EventHandler(_removeClick);
      //  menu.Items.Add(item);
      //}
      //menu.Items.Add(new System.Windows.Forms.ToolStripSeparator());
      //{
      //  System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
      //  item.Text = "Update";
      //  item.Click += new System.EventHandler(_updateSelectedItems);
      //  menu.Items.Add(item);
      //}
      return menu;
    }


    /// <summary>
    /// Save the selected layers.
    /// </summary>
    public void saveSelectedLayers()
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
            layer.addToFavorites();
          }
        }
      }
    }


    /// <summary>
    /// Update only layers that are selected.
    /// </summary>
    public void updateSelectedLayers()
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
    /// Remove selected layers
    /// </summary>
    public void removeSeletecedLayers()
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
