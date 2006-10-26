
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.JobWindow
{
  public partial class Jobs : WeifenLuo.WinFormsUI.DockContent
  {
    public Jobs()
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