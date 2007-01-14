
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager
{
  partial class Layers
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
      this._toolstrip = new System.Windows.Forms.ToolStrip();
      this._listView = new System.Windows.Forms.ListView();
      this.SuspendLayout();
      // 
      // _toolstrip
      // 
      this._toolstrip.Location = new System.Drawing.Point(0, 0);
      this._toolstrip.Name = "_toolstrip";
      this._toolstrip.Size = new System.Drawing.Size(304, 25);
      this._toolstrip.TabIndex = 0;
      this._toolstrip.Text = "toolStrip1";
      // 
      // _listView
      // 
      this._listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this._listView.Location = new System.Drawing.Point(0, 25);
      this._listView.Name = "_listView";
      this._listView.Size = new System.Drawing.Size(304, 430);
      this._listView.TabIndex = 1;
      this._listView.UseCompatibleStateImageBehavior = false;
      // 
      // Layers
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(304, 455);
      this.Controls.Add(this._listView);
      this.Controls.Add(this._toolstrip);
      this.Name = "Layers";
      this.TabText = "Layers";
      this.Text = "Layers";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip _toolstrip;
    private System.Windows.Forms.ListView _listView;
  }
}