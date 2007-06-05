
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.JobWindow
{
  public partial class Jobs : WeifenLuo.WinFormsUI.Docking.DockContent
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public Jobs()
    {
      try
      {
        this.InitializeComponent();

        this.DockAreas =
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.Docking.DockAreas.Float;
        this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
        this.HideOnClose = true;

        CadKit.Threads.GUI.Jobs jobs = new CadKit.Threads.GUI.Jobs();
        jobs.Dock = System.Windows.Forms.DockStyle.Fill;
        this.Controls.Add(jobs);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 6248991210: {0}", e.Message);
        this.Controls.Clear();
      }
    }
  }
}
