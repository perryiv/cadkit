
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.LayerManager
{
  public partial class AddLayerForm : System.Windows.Forms.Form
  {
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
    public AddLayerForm()
    {
      InitializeComponent();

      foreach (DT.Minerva.Interfaces.IDatabaseConnection dataConnection in DT.Minerva.Lib.Controller.Instance.DataSources)
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

      _okButton.Click += new System.EventHandler(_okButton_Click);
      _cancelButton.Click += new System.EventHandler(_cancelButton_Click);

      if (_lastIndexUsed >= 0 && _lastIndexUsed < _connections.Items.Count)
        _connections.SelectedIndex = _lastIndexUsed;
    }


    /// <summary>
    /// 
    /// </summary>
    void _listBox_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      string layerType = _listBox.SelectedItem as string;
      _lastIndexUsed = _connections.SelectedIndex;

      DT.Minerva.Interfaces.IDataSource dataSource = DT.Minerva.Lib.Controller.Instance.DataSources[_lastIndexUsed] as DT.Minerva.Interfaces.IDataSource;
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
    void _cancelButton_Click(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.Close();
    }


    /// <summary>
    /// 
    /// </summary>
    void _okButton_Click(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.Close();
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

          DT.Minerva.Lib.Controller.Instance.addDataSource(connection);

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