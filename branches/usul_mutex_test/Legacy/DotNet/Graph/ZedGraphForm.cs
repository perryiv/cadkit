
namespace CadKit.Graph
{
  public class ZedGraphForm : System.Windows.Forms.Form
  {
    private ZedGraph.ZedGraphControl _control = new ZedGraph.ZedGraphControl();

    public ZedGraphForm()
    {
      _control.Dock = System.Windows.Forms.DockStyle.Fill;
      this.Controls.Add(_control);
    }

    public ZedGraph.PaneBase Pane { get { return _control.GraphPane; } }
    public ZedGraph.GraphPane GraphPane { get { return _control.GraphPane; } }
    public ZedGraph.MasterPane MasterPane { get { return _control.MasterPane; } }
    public ZedGraph.ZedGraphControl ZedGraphControl { get { return _control; } }
  }
}
