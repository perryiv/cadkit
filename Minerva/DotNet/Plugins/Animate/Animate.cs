
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
      this._timeWindow.Enabled = b;
      this._pastDaysToShow.Enabled = b;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _animate_Click(object sender, System.EventArgs e)
    {
      CadKit.Interfaces.IAnimateTemporal animate = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IAnimateTemporal;

      if (null != animate)
      {
        if (_days.Checked)
          animate.TimestepType = CadKit.Interfaces.AnimateTimestep.DAY;
        if (_months.Checked)
          animate.TimestepType = CadKit.Interfaces.AnimateTimestep.MONTH;
        if (_years.Checked)
          animate.TimestepType = CadKit.Interfaces.AnimateTimestep.YEAR;

        animate.AnimationSpeed = System.Convert.ToDouble(_speed.Value);

        animate.startAnimation(float.Parse(_speed.Value.ToString()), _accumulate.Checked, _timeWindow.Checked, int.Parse(_pastDaysToShow.Value.ToString()));
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _pauseButton_Click(object sender, System.EventArgs e)
    {
      CadKit.Interfaces.IAnimateTemporal animate = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IAnimateTemporal;

      if (null != animate)
      {
        animate.pauseAnimation();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _stopButton_Click(object sender, System.EventArgs e)
    {
      CadKit.Interfaces.IAnimateTemporal animate = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IAnimateTemporal;

      if (null != animate)
      {
        animate.stopAnimation();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _speed_ValueChanged(object sender, System.EventArgs e)
    {
      CadKit.Interfaces.IAnimateTemporal animate = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IAnimateTemporal;

      if (null != animate)
      {
        double value = System.Convert.ToDouble(_speed.Value);
        animate.AnimationSpeed = value;
      }
    }
  }
}
