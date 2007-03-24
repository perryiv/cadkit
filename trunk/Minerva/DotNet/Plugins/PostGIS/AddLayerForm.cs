
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Layers.PostGIS
{
  public partial class AddLayerForm : System.Windows.Forms.UserControl
  {
    private CadKit.Interfaces.IOptionsPage _page;
    private static int _lastIndexUsed = -1;


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

      CadKit.Interfaces.ILayerList layerList = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList;
      if (null != layerList)
      {
        _favorites.DataSource = layerList.Favorites;
      }

      _connections.SelectedValueChanged += new System.EventHandler(_connections_SelectedValueChanged);

      _datatables.SelectedIndexChanged += new System.EventHandler(_listBox_SelectedIndexChanged);
      _favorites.SelectedIndexChanged += new System.EventHandler(_favorites_SelectedIndexChanged);

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

      if (null != layerList)
      {
        if (null != _favorites.SelectedItem)
          layerList.addLayer(layerList.createFavorite(_favorites.SelectedItem as string), this);
        else if (null != _datatables.SelectedItem)
        {
          DT.Minerva.Interfaces.IDatabaseConnection dataSource = DT.Minerva.DB.Connections.Instance.OpenConnections[_lastIndexUsed] as DT.Minerva.Interfaces.IDatabaseConnection;

          string tablename = _datatables.SelectedItem as string;

          CadKit.Interfaces.ILayer layer = DT.Minerva.Glue.Factory.create(tablename,dataSource);

          if (null != layer)
          {
            DT.Minerva.Interfaces.IDataSource iDataSource = layer as DT.Minerva.Interfaces.IDataSource;

            if (null != iDataSource)
              iDataSource.DataSource = dataSource;

            Properties properties = new Properties(layer);
            if (properties.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
              layerList.addLayer(layer, this);
            }
          }
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void _listBox_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      _favorites.SelectedIndex = -1;
      _favorites.SelectedItem = null;
    }


    /// <summary>
    /// 
    /// </summary>
    void _favorites_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      _datatables.SelectedIndex = -1;
      _datatables.SelectedItem = null;
    }


    /// <summary>
    /// 
    /// </summary>
    void _connections_SelectedValueChanged(object sender, System.EventArgs e)
    {
      _lastIndexUsed = _connections.SelectedIndex;
      _datatables.Enabled = true;

      _datatables.DataSource = DT.Minerva.Glue.Info.geometryTables(DT.Minerva.DB.Connections.Instance.OpenConnections[_lastIndexUsed]);
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
          DT.Minerva.Glue.Connection connection = new DT.Minerva.Glue.Connection();
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