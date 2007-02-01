
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
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

      this._setEnabled(false);
      CadKit.Documents.Manager.Instance.ActiveDocumentChanged += new CadKit.Documents.Manager.ActiveDocumentChangedDelegate(_activeDocumentChanged);
    }


    /// <summary>
    /// 
    /// </summary>
    void _activeDocumentChanged(CadKit.Interfaces.IDocument oldDoc, CadKit.Interfaces.IDocument newDoc)
    {
      if (oldDoc == newDoc)
        return;

      if (newDoc is CadKit.Interfaces.IAnimateTemporal)
      {
        this._setEnabled(true);
      }
      else
      {
        this._setEnabled(false);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _setEnabled(bool b)
    {
      this._animate.Enabled = b;
      this._speed.Enabled = b;
      this._accumulate.Enabled = b;
      this._dateTimeStep.Enabled = b;
      this._timeWindow.Enabled = b;
      this._pastDaysToShow.Enabled = b;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _animate_Click(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;
      CadKit.Interfaces.IAnimateTemporal animate = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IAnimateTemporal;

      if(null != button && null != animate)
      {
        if (button.Text == "Animate")
        {
          animate.startAnimation(float.Parse(_speed.Value.ToString()), _accumulate.Checked, _dateTimeStep.Checked, _timeWindow.Checked, int.Parse(_pastDaysToShow.Value.ToString()));
          button.Text = "Stop";
        }
        else
        {
          animate.stopAnimation();
          button.Text = "Animate";
        }
      }
    }
  }
}
