
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Animate
{
  public partial class Animate : WeifenLuo.WinFormsUI.DockContent
  {
    public Animate()
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
    }

    private void _animate_Click(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (button.Text == "Animate")
      {
        DT.Minerva.Lib.Controller.Instance.startAnimation(float.Parse(_speed.Value.ToString()), _accumulate.Checked, _dateTimeStep.Checked, _timeWindow.Checked, int.Parse(_pastDaysToShow.Value.ToString()));
        button.Text = "Stop";
      }
      else
      {
        DT.Minerva.Lib.Controller.Instance.stopAnimation();
        button.Text = "Animate";
      }
    }
  }
}