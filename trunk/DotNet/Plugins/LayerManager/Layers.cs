
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
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
      this.opacityopacitySlider.SetRange(0, 100);
      this._setUpListView();

      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += this._activeDocumentChanged;

      this._buildToolbar();
      this._buildComboBox();
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

    protected void _buildComboBox()
    {
      this.operationType.Items.Add("Top");
      this.operationType.Items.Add("Reference");
      this.operationType.Items.Add("Opacity");
      this.operationType.Items.Add("AbsoluteDifference");
      this.operationType.Items.Add("FalseColorReplacement");
      this.operationType.Items.Add("HorizontalSwipe");
      this.operationType.Items.Add("VerticalSwipe");
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
      _treeView.Nodes.Clear();
      _treeView.CheckBoxes = true;
      _treeView.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(_treeView_NodeMouseDoubleClick);
      _treeView.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(_treeView_AfterCheck);
      _treeView.ContextMenuStrip = this._buildContextMenu();
      _treeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(_treeView_NodeMouseClick);
    }


    /// <summary>
    /// Make this the active view.
    /// </summary>
    void _treeView_NodeMouseClick(object sender, System.Windows.Forms.TreeNodeMouseClickEventArgs e)
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
    void _treeView_AfterCheck(object sender, System.Windows.Forms.TreeViewEventArgs e)
    {
      try
      {
        int index = _treeView.Nodes.IndexOf(e.Node);

        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
          CadKit.Interfaces.ILayer layer = layerList.Layers[index] as CadKit.Interfaces.ILayer;

          if (null != layerList && null != layer)
          {

            if (e.Node.Checked)
            {
              e.Node.ForeColor = System.Drawing.Color.Black;
              layerList.showLayer(layer, this);
            }
            else
            {
              e.Node.ForeColor = System.Drawing.Color.Gray;
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
    void _treeView_NodeMouseDoubleClick(object sender, System.Windows.Forms.TreeNodeMouseClickEventArgs e)
    {
      if (_document is CadKit.Interfaces.ILayerList)
      {
        CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
        layerList.viewLayerExtents(layerList.Layers[_treeView.Nodes.IndexOf(_treeView.SelectedNode)]);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void _activeDocumentChanged(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc)
    {
      if (oldDoc == newDoc)
        return;

      if (null != oldDoc)
        oldDoc.remove(this);

      _document = newDoc;

      if (null != _document)
        _document.add(this);

      this.addLayers();

      _treeView.Invalidate();
    }


    /// <summary>
    /// Clear the list view and re-add layers from the document.
    /// </summary>
    public void addLayers()
    {
      try
      {
        // Clear what we have.
        _treeView.Nodes.Clear();

        // If the document is a layer list.
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;
          CadKit.Interfaces.ILayer[] layers = layerList.Layers;

          _treeView.AfterCheck -= this._treeView_AfterCheck;
          foreach (CadKit.Interfaces.ILayer layer in layers)
          {
            this._addLayer(layer);
          }
          _treeView.AfterCheck += this._treeView_AfterCheck;
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
          System.Windows.Forms.TreeNode item = new System.Windows.Forms.TreeNode(layer.Name);

          item.Checked = layer.Shown;
          _treeView.Nodes.Add(item);

          if (layer is CadKit.Interfaces.IGroup)
          {
            CadKit.Interfaces.IGroup group = (CadKit.Interfaces.IGroup)layer;
            foreach (object o in group.Children)
            {
              CadKit.Interfaces.ILayer sublayer = o as CadKit.Interfaces.ILayer;
              if (null != sublayer)
              {
                System.Windows.Forms.TreeNode subitem = new System.Windows.Forms.TreeNode(layer.Name);
                subitem.Checked = layer.Shown;
                item.Nodes.Add(subitem);
              }
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
    /// 
    /// </summary>
    public void removeSelectedLayers()
    {
      try
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          CadKit.Interfaces.ILayerList layerList = (CadKit.Interfaces.ILayerList)_document;

          int index = _treeView.Nodes.IndexOf(_treeView.SelectedNode);
          this._removeLayer(index, layerList);
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
        _treeView.Nodes.RemoveAt(index);
        layerList.removeLayer(layer, this);
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
              //_listBox.Items[i].Text = layer.Name;
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
        CadKit.Interfaces.ILayer layer = layers[_treeView.Nodes.IndexOf(_treeView.SelectedNode)];

        if (null != layer)
        {
          layer.addToFavorites();
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

          CadKit.Interfaces.ILayer layer = layers[_treeView.Nodes.IndexOf(_treeView.SelectedNode)];
          if (null != layer)
          {
            layerList.modifyLayer(layer, this);
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
          this._removeLayer(_treeView.Nodes.IndexOf(_treeView.SelectedNode), layerList);
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
      _treeView.Nodes.Clear();
    }


    /// <summary>
    /// Get the property grid.
    /// </summary>
    object CadKit.Interfaces.IPropertyGridObject.PropertyGridObject
    {
      get
      {
        if (_document is CadKit.Interfaces.ILayerList)
        {
          object layer = ((CadKit.Interfaces.ILayerList)_document).Layers[_treeView.Nodes.IndexOf(_treeView.SelectedNode)];
          if (layer is CadKit.Interfaces.IPropertyGridObject)
            return ((CadKit.Interfaces.IPropertyGridObject)layer).PropertyGridObject;
        }

        return null;
      }
    }

    private void opacitySlider_Scroll(object sender, System.EventArgs e)
    {
      if (_document is CadKit.Interfaces.ILayerOperation)
      {
        CadKit.Interfaces.ILayerOperation layerOp = (CadKit.Interfaces.ILayerOperation)_document;

        string optype = (string)this.operationType.SelectedItem;
        int layer = _treeView.Nodes.IndexOf(_treeView.SelectedNode);
        layerOp.setLayerOperation(optype, this.opacityopacitySlider.Value, layer);
      }
    }

    private void operationType_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      string optype = (string)this.operationType.SelectedItem;      
      CadKit.Interfaces.ILayerOperation layerOp = (CadKit.Interfaces.ILayerOperation)_document;
      int layer = _treeView.Nodes.IndexOf(_treeView.SelectedNode);
      layerOp.setLayerOperation(optype, this.opacityopacitySlider.Value, layer);
    }
  }
}
