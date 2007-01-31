
namespace Minerva.Plugins.App.Distributed
{
  public partial class SessionControl : System.Windows.Forms.Form
  {
    public SessionControl(string[] sessions)
    {
      InitializeComponent();

      _availableSessions.DataSource = sessions;
      _availableSessions.SelectedItem = null;
    }

    public System.String sessionName()
    {
      return _sessionName.Text;
    }

    private void _okButton_Click(object sender, System.EventArgs e)
    {
      if (_sessionName.Text != "")
      {
        this.DialogResult = System.Windows.Forms.DialogResult.OK;
        this.Close();
      }
    }

    private void _availableSessions_SelectedIndexChanged(object sender, System.EventArgs e)
    {
      _sessionName.Text = (System.String)_availableSessions.SelectedItem;
    }

    private void _onListDoubleClick(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.Close();
    }
  }
}
