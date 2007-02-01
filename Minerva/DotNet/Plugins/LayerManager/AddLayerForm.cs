
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.LayerManager
{
  public partial class AddLayerForm : System.Windows.Forms.UserControl
  {
    private CadKit.Interfaces.IOptionsPage _page;
    private CadKit.Interfaces.ILayer _currentLayer = null;
    private static int _lastIndexUsed = -1;


    /// <summary>
    ///  Get the layer.
    /// </summary>
    public CadKit.Interfaces.ILayer Layer
    {
      get
      {
        return _currentLayer;
      }
    }


    /// <summary>
    /// Constructor.
    /// </summary>
    public AddLayerForm(CadKit.Interfaces.IOptionsPage page)
    {
      InitializeComponent();

      this.Load += this._load;

      _page = page;
      if (null != _page)
        _page.Apply += this._apply;

      foreach (DT.Minerva.Interfaces.IDatabaseConnection dataConnection in DT.Minerva.DB.Connections.Instance.OpenConnections)
      {
        _connections.Items.Add(dataConnection.Database + " on " + dataConnection.Hostname);
      }

      _listBox.Enabled = false;
      string[] layers = DT.Minerva.Layers.Controls.Factory.Instance.Names;
      foreach (string s in layers)
      {
        _listBox.Items.Add(s);
      }

      _connections.SelectedValueChanged += new System.EventHandler(_connections_SelectedValueChanged);

      _listBox.SelectedIndexChanged += new System.EventHandler(_listBox_SelectedIndexChanged);

      if (_lastIndexUsed >= 0 && _lastIndexUsed < _connections.Items.Count)
        _connections.SelectedIndex = _lastIndexUsed;
    }


    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    void _load(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing += this._formClosing;
    }


    /// <summary>
    /// Called when the parent form is closing.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing -= this._formClosing;

      if (null != _page)
        _page.Apply -= this._apply;
    }


    /// <summary>
    /// Called when the apply (or ok) button is pressed.
    /// </summary>
    private void _apply(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPage page)
    {
      CadKit.Interfaces.ILayerList layerList = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList;

      if (null != layerList && null != this.Layer)
      {
        layerList.addLayer(this.Layer, this);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void _listBox_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      string layerType = _listBox.SelectedItem as string;
      _lastIndexUsed = _connections.SelectedIndex;

      DT.Minerva.Interfaces.IDataSource dataSource = DT.Minerva.DB.Connections.Instance.OpenConnections[_lastIndexUsed] as DT.Minerva.Interfaces.IDataSource;
      _currentLayer = DT.Minerva.Layers.Controls.Factory.Instance.create(layerType, dataSource);

      if(_currentLayer is CadKit.Interfaces.IPropertyGridObject)
        _propertyGrid.SelectedObject = ((CadKit.Interfaces.IPropertyGridObject)_currentLayer).PropertyGridObject;

      _listBox.Enabled = true;
    }


    /// <summary>
    /// 
    /// </summary>
    void _connections_SelectedValueChanged(object sender, System.EventArgs e)
    {
      _listBox.Enabled = true;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _addDataBase_Click(object sender, System.EventArgs e)
    {
      try
      {
        DT.Minerva.DB.ConnectToDatabase connect = new DT.Minerva.DB.ConnectToDatabase();
        connect.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;

        if (connect.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        {
          DT.Minerva.DB.Info connection = new DT.Minerva.DB.Info();
          connection.Hostname = connect.Hostname;
          connection.Database = connect.Database;
          connection.Username = connect.Username;
          connection.Password = connect.Password;
          connection.connect();

          DT.Minerva.DB.Connections.Instance.addConnection(connection);

          int index = _connections.Items.Add(connection.Database + " on " + connection.Hostname);
          _connections.SelectedIndex = index;
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 3244187004: Exception caught while trying to connect to database.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }
  }
}