
namespace DT.Minerva.Plugins.Document
{
  public partial class SessionControl : System.Windows.Forms.Form
  {
    public SessionControl( string[] sessions )
    {
      InitializeComponent();

      _availableSessions.DataSource = sessions;
      _availableSessions.SelectedItem = null;
    }


    /// <summary>
    /// 
    /// </summary>
    public string SessionName
    {
      get { return _sessionName.Text; }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _okButton_Click(object sender, System.EventArgs e)
    {
      if (_sessionName.Text != "")
      {
        this.DialogResult = System.Windows.Forms.DialogResult.OK;
        this.Close();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _availableSessions_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      _sessionName.Text = (System.String)_availableSessions.SelectedItem;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _onListDoubleClick(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.Close();
    }
  }
}
