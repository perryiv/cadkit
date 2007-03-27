
namespace CadKit.Plugins.GUI
{
  public class Form : System.Windows.Forms.Form
  {
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
  
    public Form()
    {
      this.InitializeComponent();

      System.Windows.Forms.Label label = new System.Windows.Forms.Label();
      label.Text = "Loaded Plugins:";
      this.flowLayoutPanel1.Controls.Add(label);

      System.Windows.Forms.ListView listview = new System.Windows.Forms.ListView();
      listview.View = System.Windows.Forms.View.List;
      listview.CheckBoxes = true;
      listview.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(listview_ItemCheck);
      listview.ShowItemToolTips = true;
      //listview.Dock = System.Windows.Forms.DockStyle.Fill;
      listview.Size = new System.Drawing.Size(this.Width, listview.Height * 2);
      this.flowLayoutPanel1.Controls.Add(listview);

      string[] names = CadKit.Plugins.Manager.Instance.FileNames;
      //CadKit.Interfaces.IPlugin[] plugins = CadKit.Plugins.Manager.Instance.Plugins;

      for (int i = 0; i < names.Length; ++i)
      {
        string filename = names[i];
        //CadKit.Interfaces.IPlugin plugin = CadKit.Plugins.Manager.Instance.pluginInterface(filename);
        System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(filename);
        item.Checked = CadKit.Plugins.Manager.Instance.isLoaded(filename);
        //item.ToolTipText = plugin.Description;
        listview.Items.Add(item);
      }
    }

    void listview_ItemCheck(object sender, System.Windows.Forms.ItemCheckEventArgs e)
    {
    }

    private void InitializeComponent()
    {
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.SuspendLayout();
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(616, 462);
      this.flowLayoutPanel1.TabIndex = 0;
      // 
      // Form
      // 
      this.ClientSize = new System.Drawing.Size(616, 462);
      this.Controls.Add(this.flowLayoutPanel1);
      this.Name = "Form";
      this.Text = "Plugins";
      this.ResumeLayout(false);

    }
  }
}
