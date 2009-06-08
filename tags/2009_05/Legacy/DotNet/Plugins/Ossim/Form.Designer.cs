
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Ossim
{
  partial class Form
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

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
      this._toolStrip = new System.Windows.Forms.ToolStrip();
      this._listView = new System.Windows.Forms.ListView();
      this.SuspendLayout();
      // 
      // _toolStrip
      // 
      this._toolStrip.Location = new System.Drawing.Point(0, 0);
      this._toolStrip.Name = "_toolStrip";
      this._toolStrip.Size = new System.Drawing.Size(292, 25);
      this._toolStrip.TabIndex = 0;
      this._toolStrip.Text = "toolStrip1";
      // 
      // _listView
      // 
      this._listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this._listView.Location = new System.Drawing.Point(0, 25);
      this._listView.Name = "_listView";
      this._listView.Size = new System.Drawing.Size(292, 354);
      this._listView.TabIndex = 1;
      this._listView.UseCompatibleStateImageBehavior = false;
      // 
      // Form
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(292, 379);
      this.Controls.Add(this._listView);
      this.Controls.Add(this._toolStrip);
      this.Name = "Form";
      this.TabText = "Ossim Layers";
      this.Text = "Ossim Layers";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip _toolStrip;
    private System.Windows.Forms.ListView _listView;
  }
}