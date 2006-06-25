
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class SplashScreen : 
    System.Windows.Forms.Form, 
    CadKit.Interfaces.IProgressBar,
    CadKit.Interfaces.IUpdateDisplay,
    CadKit.Interfaces.IMenuBar
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public SplashScreen ( object caller )
    {
      _caller = caller;
      InitializeComponent();
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private System.ComponentModel.IContainer components = null;
    private System.Windows.Forms.Label _label = null;
    private object _caller = null;

    /// <summary>
    /// Get the menu bar.
    /// </summary>
    object CadKit.Interfaces.IMenuBar.MenuBar
    {
      get
      {
        CadKit.Interfaces.IMenuBar bar = _caller as CadKit.Interfaces.IMenuBar;
        return (null == bar) ? null : bar.MenuBar;
      }
    }

    /// <summary>
    /// Set/get the image.
    /// </summary>
    public System.Drawing.Image Image
    {
      get { return _label.Image; }
      set
      {
        _label.Image = value;
        _label.Size = value.Size;
      }
    }

    /// <summary>
    /// Set/get the label.
    /// </summary>
    public System.Windows.Forms.Label Label
    {
      get { return _label; }
      set { _label = value; }
    }

    /// <summary>
    /// Set/get the progress bar.
    /// </summary>
    public System.Windows.Forms.ProgressBar ProgressBar
    {
      get { return _progressBar; }
      set { _progressBar = value; }
    }

    /// <summary>
    /// Implement progress bar interface.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Value
    {
      set
      {
        _progressBar.Value = value;
        _progressBar.Invalidate(true);
      }
      get { return _progressBar.Value; }
    }

    /// <summary>
    /// Implement progress bar interface.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Range
    {
      get { return _progressBar.Maximum - _progressBar.Minimum; }
    }

    /// <summary>
    /// Implement progress bar interface.
    /// </summary>
    string CadKit.Interfaces.IProgressBar.Text
    {
      set
      {
        _label.Text = value;
        _label.Invalidate(true);
      }
      get { return _label.Text; }
    }

    /// <summary>
    /// Implement interface.
    /// </summary>
    void CadKit.Interfaces.IUpdateDisplay.updateDisplay()
    {
      _label.Update();
      _progressBar.Update();
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this._progressBar = new System.Windows.Forms.ProgressBar();
      this._label = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // _progressBar
      // 
      this._progressBar.Dock = System.Windows.Forms.DockStyle.Bottom;
      this._progressBar.Location = new System.Drawing.Point(0, 331);
      this._progressBar.Name = "_progressBar";
      this._progressBar.Size = new System.Drawing.Size(460, 20);
      this._progressBar.TabIndex = 0;
      // 
      // _label
      // 
      this._label.Dock = System.Windows.Forms.DockStyle.Fill;
      this._label.ForeColor = System.Drawing.Color.White;
      this._label.Location = new System.Drawing.Point(0, 0);
      this._label.Name = "_label";
      this._label.Size = new System.Drawing.Size(460, 331);
      this._label.TabIndex = 1;
      this._label.Text = "Default Text";
      this._label.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
      // 
      // SplashScreen
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(460, 351);
      this.Controls.Add(this._label);
      this.Controls.Add(this._progressBar);
      this.Name = "SplashScreen";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "SplashScreen";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.ProgressBar _progressBar;
  }
}
