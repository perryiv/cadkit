
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.LayerManager
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Layers));
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this._addLayerButton = new System.Windows.Forms.ToolStripButton();
      this._removeButton = new System.Windows.Forms.ToolStripButton();
      this._updateButton = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this._upButton = new System.Windows.Forms.ToolStripButton();
      this._downButton = new System.Windows.Forms.ToolStripButton();
      this._listView = new System.Windows.Forms.ListView();
      this.toolStrip1.SuspendLayout();
      this.SuspendLayout();
      // 
      // toolStrip1
      // 
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this._addLayerButton,
            this._removeButton,
            this._updateButton,
            this.toolStripSeparator1,
            this._upButton,
            this._downButton});
      this.toolStrip1.Location = new System.Drawing.Point(0, 0);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(304, 25);
      this.toolStrip1.TabIndex = 0;
      this.toolStrip1.Text = "toolStrip1";
      // 
      // _addLayerButton
      // 
      this._addLayerButton.BackColor = System.Drawing.SystemColors.Control;
      this._addLayerButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this._addLayerButton.Image = ((System.Drawing.Image)(resources.GetObject("_addLayerButton.Image")));
      this._addLayerButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this._addLayerButton.Name = "_addLayerButton";
      this._addLayerButton.Size = new System.Drawing.Size(23, 22);
      this._addLayerButton.Text = "toolStripButton1";
      this._addLayerButton.Click += new System.EventHandler(this._addLayerButton_Click);
      // 
      // _removeButton
      // 
      this._removeButton.BackColor = System.Drawing.SystemColors.Control;
      this._removeButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this._removeButton.Image = ((System.Drawing.Image)(resources.GetObject("_removeButton.Image")));
      this._removeButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this._removeButton.Name = "_removeButton";
      this._removeButton.Size = new System.Drawing.Size(23, 22);
      this._removeButton.Text = "toolStripButton1";
      this._removeButton.Click += new System.EventHandler(this._removeButton_Click);
      // 
      // _updateButton
      // 
      this._updateButton.BackColor = System.Drawing.SystemColors.Control;
      this._updateButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this._updateButton.Image = ((System.Drawing.Image)(resources.GetObject("_updateButton.Image")));
      this._updateButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this._updateButton.Name = "_updateButton";
      this._updateButton.Size = new System.Drawing.Size(23, 22);
      this._updateButton.Text = "toolStripButton2";
      this._updateButton.Click += new System.EventHandler(this._updateButton_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
      // 
      // _upButton
      // 
      this._upButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this._upButton.Image = ((System.Drawing.Image)(resources.GetObject("_upButton.Image")));
      this._upButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this._upButton.Name = "_upButton";
      this._upButton.Size = new System.Drawing.Size(23, 22);
      this._upButton.Text = "toolStripButton1";
      this._upButton.Click += new System.EventHandler(this._upButton_Click);
      // 
      // _downButton
      // 
      this._downButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this._downButton.Image = ((System.Drawing.Image)(resources.GetObject("_downButton.Image")));
      this._downButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this._downButton.Name = "_downButton";
      this._downButton.Size = new System.Drawing.Size(23, 22);
      this._downButton.Text = "toolStripButton2";
      this._downButton.Click += new System.EventHandler(this._downButton_Click);
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
      this.Controls.Add(this.toolStrip1);
      this.Name = "Layers";
      this.TabText = "Layers";
      this.Text = "Layers";
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip toolStrip1;
    private System.Windows.Forms.ToolStripButton _addLayerButton;
    private System.Windows.Forms.ToolStripButton _removeButton;
    private System.Windows.Forms.ToolStripButton _updateButton;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private System.Windows.Forms.ToolStripButton _upButton;
    private System.Windows.Forms.ToolStripButton _downButton;
    private System.Windows.Forms.ListView _listView;
  }
}