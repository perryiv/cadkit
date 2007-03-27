
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Test
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
      this._outerTable = new System.Windows.Forms.TableLayoutPanel();
      this._innerTable = new System.Windows.Forms.TableLayoutPanel();
      this._addJobButton = new System.Windows.Forms.Button();
      this._clearJobsButton = new System.Windows.Forms.Button();
      this._jobs = new CadKit.Threads.GUI.Jobs();
      this._outerTable.SuspendLayout();
      this._innerTable.SuspendLayout();
      this.SuspendLayout();
      // 
      // _outerTable
      // 
      this._outerTable.ColumnCount = 1;
      this._outerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._outerTable.Controls.Add(this._innerTable, 0, 0);
      this._outerTable.Controls.Add(this._jobs, 0, 1);
      this._outerTable.Dock = System.Windows.Forms.DockStyle.Fill;
      this._outerTable.Location = new System.Drawing.Point(0, 0);
      this._outerTable.Name = "_outerTable";
      this._outerTable.RowCount = 2;
      this._outerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
      this._outerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._outerTable.Size = new System.Drawing.Size(509, 294);
      this._outerTable.TabIndex = 0;
      // 
      // _innerTable
      // 
      this._innerTable.ColumnCount = 3;
      this._innerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 81F));
      this._innerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._innerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 81F));
      this._innerTable.Controls.Add(this._addJobButton, 0, 0);
      this._innerTable.Controls.Add(this._clearJobsButton, 2, 0);
      this._innerTable.Dock = System.Windows.Forms.DockStyle.Fill;
      this._innerTable.Location = new System.Drawing.Point(3, 3);
      this._innerTable.Name = "_innerTable";
      this._innerTable.RowCount = 1;
      this._innerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
      this._innerTable.Size = new System.Drawing.Size(503, 34);
      this._innerTable.TabIndex = 0;
      // 
      // _addJobButton
      // 
      this._addJobButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._addJobButton.Location = new System.Drawing.Point(3, 5);
      this._addJobButton.Name = "_addJobButton";
      this._addJobButton.Size = new System.Drawing.Size(75, 23);
      this._addJobButton.TabIndex = 0;
      this._addJobButton.Text = "Add";
      this._addJobButton.UseVisualStyleBackColor = true;
      this._addJobButton.Click += new System.EventHandler(this._onAddJob);
      // 
      // _clearJobsButton
      // 
      this._clearJobsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._clearJobsButton.Location = new System.Drawing.Point(425, 5);
      this._clearJobsButton.Name = "_clearJobsButton";
      this._clearJobsButton.Size = new System.Drawing.Size(75, 23);
      this._clearJobsButton.TabIndex = 1;
      this._clearJobsButton.Text = "Clear";
      this._clearJobsButton.UseVisualStyleBackColor = true;
      this._clearJobsButton.Click += new System.EventHandler(this._onClearJobs);
      // 
      // _jobs
      // 
      this._jobs.Dock = System.Windows.Forms.DockStyle.Fill;
      this._jobs.Location = new System.Drawing.Point(3, 43);
      this._jobs.Name = "_jobs";
      this._jobs.Padding = new System.Windows.Forms.Padding(2);
      this._jobs.Size = new System.Drawing.Size(503, 248);
      this._jobs.TabIndex = 1;
      // 
      // Form
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(509, 294);
      this.Controls.Add(this._outerTable);
      this.Name = "Form";
      this.Text = "Form";
      this._outerTable.ResumeLayout(false);
      this._innerTable.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TableLayoutPanel _outerTable;
    private System.Windows.Forms.TableLayoutPanel _innerTable;
    private System.Windows.Forms.Button _addJobButton;
    private System.Windows.Forms.Button _clearJobsButton;
    private CadKit.Threads.GUI.Jobs _jobs;

  }
}