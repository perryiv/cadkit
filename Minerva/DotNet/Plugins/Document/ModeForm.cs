
namespace DT.Minerva.Plugins.Document
{
  public partial class ModeForm : System.Windows.Forms.Form
  {
    public enum Mode
    {
      DLL,
      DISTRIBUTED,
      BOTH
    }

    Mode _mode = Mode.BOTH;


    public Mode DrawMode
    {
      get { return _mode; }
      set { _mode = value; }
    }


    public ModeForm()
    {
      InitializeComponent();
    }

    private void _dll_Click(object sender, System.EventArgs e)
    {
      this.DrawMode = Mode.DLL;
      this.Close();
    }

    private void _distributed_Click(object sender, System.EventArgs e)
    {
      this.DrawMode = Mode.DISTRIBUTED;
      this.Close();
    }

    private void _both_Click(object sender, System.EventArgs e)
    {
      this.DrawMode = Mode.BOTH;
      this.Close();
    }
  }
}