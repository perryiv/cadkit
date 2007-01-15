
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Animate
{
  partial class Animate
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
      this._animate = new System.Windows.Forms.Button();
      this._speed = new System.Windows.Forms.NumericUpDown();
      this._accumulate = new System.Windows.Forms.CheckBox();
      this._timeWindow = new System.Windows.Forms.CheckBox();
      this._pastDaysToShow = new System.Windows.Forms.NumericUpDown();
      this._dateTimeStep = new System.Windows.Forms.CheckBox();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      ((System.ComponentModel.ISupportInitialize)(this._speed)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this._pastDaysToShow)).BeginInit();
      this.flowLayoutPanel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // _animate
      // 
      this._animate.Location = new System.Drawing.Point(3, 3);
      this._animate.Name = "_animate";
      this._animate.Size = new System.Drawing.Size(75, 23);
      this._animate.TabIndex = 8;
      this._animate.Text = "Animate";
      this._animate.UseVisualStyleBackColor = true;
      this._animate.Click += new System.EventHandler(this._animate_Click);
      // 
      // _speed
      // 
      this._speed.DecimalPlaces = 1;
      this._speed.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
      this._speed.Location = new System.Drawing.Point(84, 3);
      this._speed.Name = "_speed";
      this._speed.Size = new System.Drawing.Size(120, 20);
      this._speed.TabIndex = 9;
      this._speed.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
      // 
      // _accumulate
      // 
      this._accumulate.AutoSize = true;
      this._accumulate.Checked = true;
      this._accumulate.CheckState = System.Windows.Forms.CheckState.Checked;
      this._accumulate.Location = new System.Drawing.Point(210, 3);
      this._accumulate.Name = "_accumulate";
      this._accumulate.Size = new System.Drawing.Size(107, 17);
      this._accumulate.TabIndex = 10;
      this._accumulate.Text = "Show past cases";
      this._accumulate.UseVisualStyleBackColor = true;
      // 
      // _timeWindow
      // 
      this._timeWindow.AutoSize = true;
      this._timeWindow.Location = new System.Drawing.Point(437, 3);
      this._timeWindow.Name = "_timeWindow";
      this._timeWindow.Size = new System.Drawing.Size(91, 17);
      this._timeWindow.TabIndex = 13;
      this._timeWindow.Text = "Time Window";
      this._timeWindow.UseVisualStyleBackColor = true;
      // 
      // _pastDaysToShow
      // 
      this._pastDaysToShow.Location = new System.Drawing.Point(534, 3);
      this._pastDaysToShow.Name = "_pastDaysToShow";
      this._pastDaysToShow.Size = new System.Drawing.Size(120, 20);
      this._pastDaysToShow.TabIndex = 12;
      // 
      // _dateTimeStep
      // 
      this._dateTimeStep.AutoSize = true;
      this._dateTimeStep.Checked = true;
      this._dateTimeStep.CheckState = System.Windows.Forms.CheckState.Checked;
      this._dateTimeStep.Location = new System.Drawing.Point(323, 3);
      this._dateTimeStep.Name = "_dateTimeStep";
      this._dateTimeStep.Size = new System.Drawing.Size(108, 17);
      this._dateTimeStep.TabIndex = 11;
      this._dateTimeStep.Text = "Date as time step";
      this._dateTimeStep.UseVisualStyleBackColor = true;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Controls.Add(this._animate);
      this.flowLayoutPanel1.Controls.Add(this._speed);
      this.flowLayoutPanel1.Controls.Add(this._accumulate);
      this.flowLayoutPanel1.Controls.Add(this._dateTimeStep);
      this.flowLayoutPanel1.Controls.Add(this._timeWindow);
      this.flowLayoutPanel1.Controls.Add(this._pastDaysToShow);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(801, 152);
      this.flowLayoutPanel1.TabIndex = 14;
      // 
      // Animate
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(801, 152);
      this.Controls.Add(this.flowLayoutPanel1);
      this.Name = "Animate";
      this.TabText = "Animate";
      this.Text = "Animate";
      ((System.ComponentModel.ISupportInitialize)(this._speed)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this._pastDaysToShow)).EndInit();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.flowLayoutPanel1.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button _animate;
    private System.Windows.Forms.NumericUpDown _speed;
    private System.Windows.Forms.CheckBox _accumulate;
    private System.Windows.Forms.CheckBox _timeWindow;
    private System.Windows.Forms.NumericUpDown _pastDaysToShow;
    private System.Windows.Forms.CheckBox _dateTimeStep;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
  }
}