
namespace DT.Minerva.Lib
{
  public partial class MergeData : System.Windows.Forms.Form
  {
    public MergeData()
    {
      InitializeComponent();

      _dataSources.SelectedIndexChanged += new System.EventHandler(_dataSources_SelectedIndexChanged);
      _dataSources.DataSource = Controller.Instance.DataSources;
    }

    void _dataSources_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      int index = _dataSources.SelectedIndex;
      DT.Minerva.Interfaces.IDataSource dataSource = Controller.Instance.DataSources[index] as DT.Minerva.Interfaces.IDataSource;

      _tablesOne.SelectedIndexChanged += new System.EventHandler(_tablesOne_SelectedIndexChanged);
      _tablesTwo.SelectedIndexChanged += new System.EventHandler(_tablesTwo_SelectedIndexChanged);
      _tablesOne.DataSource = dataSource.Tables;
      _tablesTwo.DataSource = dataSource.Tables;
    }

    void _tablesTwo_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      int index = _dataSources.SelectedIndex;
      DT.Minerva.Interfaces.IDataSource dataSource = Controller.Instance.DataSources[index] as DT.Minerva.Interfaces.IDataSource;

      string tablename = (string)_tablesTwo.SelectedItem;
      System.Collections.Generic.List<string> columns = dataSource.getColumnNames(tablename);

      groupBox2.Text = tablename;

      flowLayoutPanel2.Controls.Clear();

      foreach (string column in columns)
      {
        System.Windows.Forms.CheckBox check = new System.Windows.Forms.CheckBox();
        check.Text = column;
        check.CheckedChanged += new System.EventHandler(check_CheckedChanged2);
        flowLayoutPanel2.Controls.Add(check);
      }
    }

    void check_CheckedChanged2(object sender, System.EventArgs e)
    {
      System.Windows.Forms.CheckBox check = sender as System.Windows.Forms.CheckBox;

      if (check.Checked)
      {
        comboBox2.Items.Add(check.Text);
      }
      else
      {
        comboBox2.Items.Remove(check.Text);
      }
    }

    void _tablesOne_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      int index = _dataSources.SelectedIndex;
      DT.Minerva.Interfaces.IDataSource dataSource = Controller.Instance.DataSources[index] as DT.Minerva.Interfaces.IDataSource;

      string tablename = (string)_tablesOne.SelectedItem;
      System.Collections.Generic.List<string> columns = dataSource.getColumnNames(tablename);

      groupBox1.Text = tablename;

      flowLayoutPanel1.Controls.Clear();

      foreach (string column in columns)
      {
        System.Windows.Forms.CheckBox check = new System.Windows.Forms.CheckBox();
        check.Text = column;
        check.CheckedChanged += new System.EventHandler(check_CheckedChanged);
        flowLayoutPanel1.Controls.Add(check);
      }
    }

    void check_CheckedChanged(object sender, System.EventArgs e)
    {
      System.Windows.Forms.CheckBox check = sender as System.Windows.Forms.CheckBox;

      if (check.Checked)
      {
        comboBox1.Items.Add(check.Text);
      }
      else
      {
        comboBox1.Items.Remove(check.Text);
      }
    }

    private void _okButton_Click(object sender, System.EventArgs e)
    {
      string tableName = textBox1.Text;
      DT.Minerva.Interfaces.IDatabaseConnection connection = Controller.Instance.DataSources[_dataSources.SelectedIndex];
      DT.Minerva.Interfaces.IQuery dataQuery = connection as DT.Minerva.Interfaces.IQuery;

      string tableOne = _tablesOne.SelectedItem as string;

      string queryOne = "SELECT * FROM " + tableOne;

      System.Data.IDataReader dr = dataQuery.execteQueryIDataReader(queryOne);
      //Npgsql.NpgsqlDataReader ndr = dr as Npgsql.NpgsqlDataReader;

      //System.Windows.Forms.Form test = new System.Windows.Forms.Form();
      //System.Windows.Forms.DataGridView grid = new System.Windows.Forms.DataGridView();
      //grid.DataSource = ndr;
      //grid.Dock = System.Windows.Forms.DockStyle.Fill;
      //test.Controls.Add(grid);
      //test.Show();

      foreach (string s in comboBox1.Items)
      {
      }

      string tableTwo = _tablesTwo.SelectedItem as string;

      string queryTwo = "SELECT * FROM " + tableTwo;

      foreach (string s in comboBox2.Items)
      {
      }

      // Create table
      /*
       * CREATE TABLE tableName { id, int4 }
       * */
      this.Close();
    }

    private void _cancelButton_Click(object sender, System.EventArgs e)
    {
      this.Close();
    }

  }
}