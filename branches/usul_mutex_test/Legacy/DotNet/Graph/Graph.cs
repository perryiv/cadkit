
namespace CadKit.Graph
{
  public partial class Graph : System.Windows.Forms.Form
  {
    public Graph()
    {
      InitializeComponent();
    }

    public Plot Plot
    {
      get { return this.panel1.Plot; }
      set { this.panel1.Plot = value; }
    }
  }
}