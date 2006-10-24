
namespace CadKit.Helios.Lib
{
  public partial class JobsForm : WeifenLuo.WinFormsUI.DockContent
  {
    public JobsForm()
    {
      InitializeComponent();

      this.DockableAreas = 
        WeifenLuo.WinFormsUI.DockAreas.DockBottom | 
        WeifenLuo.WinFormsUI.DockAreas.DockLeft | 
        WeifenLuo.WinFormsUI.DockAreas.DockRight | 
        WeifenLuo.WinFormsUI.DockAreas.DockTop | 
        WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockBottom;
      this.HideOnClose = true;

      CadKit.Threads.GUI.Jobs jobs = new CadKit.Threads.GUI.Jobs();
      jobs.Dock = System.Windows.Forms.DockStyle.Fill;
      this.Controls.Add(jobs);
    }
  }
}