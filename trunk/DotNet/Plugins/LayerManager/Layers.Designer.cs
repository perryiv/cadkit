
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
      this._treeView = new System.Windows.Forms.TreeView();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.slider = new System.Windows.Forms.TrackBar();
      this.operationType = new System.Windows.Forms.ComboBox();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.slider)).BeginInit();
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
      // _treeView
      // 
      this._treeView.Dock = System.Windows.Forms.DockStyle.Fill;
      this._treeView.Location = new System.Drawing.Point(0, 0);
      this._treeView.Name = "_treeView";
      this._treeView.Size = new System.Drawing.Size(304, 322);
      this._treeView.TabIndex = 1;
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 25);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this._treeView);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.slider);
      this.splitContainer1.Panel2.Controls.Add(this.operationType);
      this.splitContainer1.Size = new System.Drawing.Size(304, 430);
      this.splitContainer1.SplitterDistance = 322;
      this.splitContainer1.TabIndex = 2;
      // 
      // slider
      // 
      this.slider.Location = new System.Drawing.Point(12, 44);
      this.slider.Name = "slider";
      this.slider.Size = new System.Drawing.Size(280, 48);
      this.slider.TabIndex = 1;
      this.slider.Scroll += new System.EventHandler(this.slider_Scroll);
      // 
      // operationType
      // 
      this.operationType.FormattingEnabled = true;
      this.operationType.Location = new System.Drawing.Point(91, 14);
      this.operationType.Name = "operationType";
      this.operationType.Size = new System.Drawing.Size(121, 21);
      this.operationType.TabIndex = 0;
      this.operationType.SelectedIndexChanged += new System.EventHandler(this.operationType_SelectedIndexChanged);
      // 
      // Layers
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(304, 455);
      this.Controls.Add(this.splitContainer1);
      this.Controls.Add(this._toolstrip);
      this.Name = "Layers";
      this.TabText = "Layers";
      this.Text = "Layers";
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.Panel2.PerformLayout();
      this.splitContainer1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.slider)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip _toolstrip;
    private System.Windows.Forms.TreeView _treeView;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.TrackBar slider;
    private System.Windows.Forms.ComboBox operationType;
  }
}