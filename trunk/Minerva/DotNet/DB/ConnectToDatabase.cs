
namespace DT.Minerva.DB
{
  public partial class ConnectToDatabase : System.Windows.Forms.Form
  {
    class Saved
    {
      public string hostname = "";
      public string database = "";
      public string username = "";
    }

    private System.Collections.Generic.List<Saved> _saved = new System.Collections.Generic.List<Saved>();


    /// <summary>
    /// 
    /// </summary>
    public ConnectToDatabase()
    {
      InitializeComponent();

      string value = CadKit.Persistence.Registry.Instance.getString("Database", "Saved", "");

      string [] saved = value.Split(new string[] { ":" }, System.StringSplitOptions.RemoveEmptyEntries);

      foreach (string s in saved)
      {
        string[] entries = s.Split(new string[] { "|" }, System.StringSplitOptions.RemoveEmptyEntries);

        Saved savedEntry = new Saved();
        savedEntry.hostname = entries[0];
        savedEntry.database = entries[1];
        savedEntry.username = entries[2];

        _savedEntries.Items.Add(savedEntry.database + " on " + savedEntry.hostname);
        _saved.Add(savedEntry);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _okButton_Click(object sender, System.EventArgs e)
    {
      // Create local connection to check for errors when connecting.
      DT.Minerva.Glue.Connection connection = new DT.Minerva.Glue.Connection();
      connection.Hostname = this.Hostname;
      connection.Database = this.Database;
      connection.Username = this.Username;
      connection.Password = this.Password;
      try
      {
        connection.connect();
      }
      //catch (Npgsql.NpgsqlException exception)
      //{
      //  // Authentication error.  See http://www.linuxshare.ru/postgresql/manual/errcodes-appendix.html for error codes.
      //  if (exception.Code == "28000")
      //  {
      //    string message = "Authentication for user " + this.Username + " failed.";
      //    System.Windows.Forms.MessageBox.Show(message, "Authentication Failed", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
      //    return;
      //  }
      //  else
      //  {
      //    System.Windows.Forms.MessageBox.Show(System.String.Format("Error when connecting to database: {0}", exception.Message), "Connection Failed!", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
      //    return;
      //  }
      //}
      catch (System.Exception exception)
      {
        System.Windows.Forms.MessageBox.Show(System.String.Format ( "Error when connecting to database: {0}", exception.Message ), "Connection Failed!", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
        return;
      }
      finally
      {
        connection.disconnect();
      }


      this.DialogResult = System.Windows.Forms.DialogResult.OK;

      if ( false == _hasValue() )
        this._save();

      this.Close();
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    private bool _hasValue()
    {
      foreach (Saved s in _saved)
      {
        if (this.Hostname == s.hostname && this.Database == s.database && this.Username == s.username)
          return true;
      }

      return false;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _save()
    {
      string save = this.Hostname + "|" + this.Database + "|" + this.Username + ":";

      string value = CadKit.Persistence.Registry.Instance.getString("Database", "Saved", "");

      value += save;

      CadKit.Persistence.Registry.Instance.setString("Database", "Saved", value);
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _cancelButton_Click(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.Close();
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public string Hostname
    {
      get
      {
        return _host.Text;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public string Database
    {
      get
      {
        return _database.Text;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public string Username
    {
      get
      {
        return _user.Text;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public string Password
    {
      get
      {
        return _password.Text;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void _savedEntries_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      int index = _savedEntries.SelectedIndex;

      Saved entry = _saved[index];

      _host.Text = entry.hostname;
      _database.Text = entry.database;
      _user.Text = entry.username;
    }
  }
}