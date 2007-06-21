
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
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this._stopButton = new System.Windows.Forms.Button();
      this._pauseButton = new System.Windows.Forms.Button();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.trackBar2 = new System.Windows.Forms.TrackBar();
      this.trackBar1 = new System.Windows.Forms.TrackBar();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this._stepAmount = new System.Windows.Forms.NumericUpDown();
      this._days = new System.Windows.Forms.RadioButton();
      this._years = new System.Windows.Forms.RadioButton();
      this._months = new System.Windows.Forms.RadioButton();
      ((System.ComponentModel.ISupportInitialize)(this._speed)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this._pastDaysToShow)).BeginInit();
      this.flowLayoutPanel1.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.groupBox1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar2)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
      this.groupBox2.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this._stepAmount)).BeginInit();
      this.SuspendLayout();
      // 
      // _animate
      // 
      this._animate.Location = new System.Drawing.Point(9, 15);
      this._animate.Name = "_animate";
      this._animate.Size = new System.Drawing.Size(75, 23);
      this._animate.TabIndex = 8;
      this._animate.Text = "Animate";
      this._animate.UseVisualStyleBackColor = true;
      this._animate.Click += new System.EventHandler(this._animate_Click);
      // 
      // _speed
      // 
      this._speed.DecimalPlaces = 3;
      this._speed.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
      this._speed.Location = new System.Drawing.Point(166, 44);
      this._speed.Name = "_speed";
      this._speed.Size = new System.Drawing.Size(75, 20);
      this._speed.TabIndex = 9;
      this._speed.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
      this._speed.ValueChanged += new System.EventHandler(this._speed_ValueChanged);
      // 
      // _accumulate
      // 
      this._accumulate.AutoSize = true;
      this._accumulate.Checked = true;
      this._accumulate.CheckState = System.Windows.Forms.CheckState.Checked;
      this._accumulate.Location = new System.Drawing.Point(9, 47);
      this._accumulate.Name = "_accumulate";
      this._accumulate.Size = new System.Drawing.Size(107, 17);
      this._accumulate.TabIndex = 10;
      this._accumulate.Text = "Show past cases";
      this._accumulate.UseVisualStyleBackColor = true;
      // 
      // _timeWindow
      // 
      this._timeWindow.AutoSize = true;
      this._timeWindow.Location = new System.Drawing.Point(6, 19);
      this._timeWindow.Name = "_timeWindow";
      this._timeWindow.Size = new System.Drawing.Size(91, 17);
      this._timeWindow.TabIndex = 13;
      this._timeWindow.Text = "Time Window";
      this._timeWindow.UseVisualStyleBackColor = true;
      // 
      // _pastDaysToShow
      // 
      this._pastDaysToShow.Location = new System.Drawing.Point(103, 18);
      this._pastDaysToShow.Name = "_pastDaysToShow";
      this._pastDaysToShow.Size = new System.Drawing.Size(91, 20);
      this._pastDaysToShow.TabIndex = 12;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Controls.Add(this.groupBox3);
      this.flowLayoutPanel1.Controls.Add(this.groupBox1);
      this.flowLayoutPanel1.Controls.Add(this.groupBox2);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(832, 200);
      this.flowLayoutPanel1.TabIndex = 14;
      // 
      // groupBox3
      // 
      this.groupBox3.Controls.Add(this._stopButton);
      this.groupBox3.Controls.Add(this._pauseButton);
      this.groupBox3.Controls.Add(this._animate);
      this.groupBox3.Controls.Add(this._accumulate);
      this.groupBox3.Controls.Add(this._speed);
      this.groupBox3.Location = new System.Drawing.Point(3, 3);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(247, 156);
      this.groupBox3.TabIndex = 19;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Controls";
      // 
      // _stopButton
      // 
      this._stopButton.Location = new System.Drawing.Point(87, 15);
      this._stopButton.Name = "_stopButton";
      this._stopButton.Size = new System.Drawing.Size(75, 23);
      this._stopButton.TabIndex = 12;
      this._stopButton.Text = "Stop";
      this._stopButton.UseVisualStyleBackColor = true;
      this._stopButton.Click += new System.EventHandler(this._stopButton_Click);
      // 
      // _pauseButton
      // 
      this._pauseButton.Location = new System.Drawing.Point(166, 15);
      this._pauseButton.Name = "_pauseButton";
      this._pauseButton.Size = new System.Drawing.Size(75, 23);
      this._pauseButton.TabIndex = 11;
      this._pauseButton.Text = "Pause";
      this._pauseButton.UseVisualStyleBackColor = true;
      this._pauseButton.Click += new System.EventHandler(this._pauseButton_Click);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.trackBar2);
      this.groupBox1.Controls.Add(this.trackBar1);
      this.groupBox1.Controls.Add(this._pastDaysToShow);
      this.groupBox1.Controls.Add(this._timeWindow);
      this.groupBox1.Location = new System.Drawing.Point(256, 3);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(385, 156);
      this.groupBox1.TabIndex = 17;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Window";
      // 
      // trackBar2
      // 
      this.trackBar2.Location = new System.Drawing.Point(16, 94);
      this.trackBar2.Name = "trackBar2";
      this.trackBar2.Size = new System.Drawing.Size(363, 45);
      this.trackBar2.TabIndex = 15;
      // 
      // trackBar1
      // 
      this.trackBar1.Location = new System.Drawing.Point(16, 42);
      this.trackBar1.Name = "trackBar1";
      this.trackBar1.Size = new System.Drawing.Size(363, 45);
      this.trackBar1.TabIndex = 14;
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this._stepAmount);
      this.groupBox2.Controls.Add(this._days);
      this.groupBox2.Controls.Add(this._years);
      this.groupBox2.Controls.Add(this._months);
      this.groupBox2.Location = new System.Drawing.Point(647, 3);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(115, 156);
      this.groupBox2.TabIndex = 18;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Timestep";
      // 
      // _stepAmount
      // 
      this._stepAmount.Location = new System.Drawing.Point(9, 94);
      this._stepAmount.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
      this._stepAmount.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
      this._stepAmount.Name = "_stepAmount";
      this._stepAmount.Size = new System.Drawing.Size(91, 20);
      this._stepAmount.TabIndex = 17;
      this._stepAmount.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
      // 
      // _days
      // 
      this._days.AutoSize = true;
      this._days.Checked = true;
      this._days.Location = new System.Drawing.Point(9, 22);
      this._days.Name = "_days";
      this._days.Size = new System.Drawing.Size(49, 17);
      this._days.TabIndex = 14;
      this._days.TabStop = true;
      this._days.Text = "Days";
      this._days.UseVisualStyleBackColor = true;
      // 
      // _years
      // 
      this._years.AutoSize = true;
      this._years.Location = new System.Drawing.Point(9, 70);
      this._years.Name = "_years";
      this._years.Size = new System.Drawing.Size(52, 17);
      this._years.TabIndex = 16;
      this._years.Text = "Years";
      this._years.UseVisualStyleBackColor = true;
      // 
      // _months
      // 
      this._months.AutoSize = true;
      this._months.Location = new System.Drawing.Point(9, 47);
      this._months.Name = "_months";
      this._months.Size = new System.Drawing.Size(60, 17);
      this._months.TabIndex = 15;
      this._months.Text = "Months";
      this._months.UseVisualStyleBackColor = true;
      // 
      // Animate
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(832, 200);
      this.Controls.Add(this.flowLayoutPanel1);
      this.Name = "Animate";
      this.TabText = "Animate";
      this.Text = "Animate";
      ((System.ComponentModel.ISupportInitialize)(this._speed)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this._pastDaysToShow)).EndInit();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.groupBox3.ResumeLayout(false);
      this.groupBox3.PerformLayout();
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar2)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this._stepAmount)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button _animate;
    private System.Windows.Forms.NumericUpDown _speed;
    private System.Windows.Forms.CheckBox _accumulate;
    private System.Windows.Forms.CheckBox _timeWindow;
    private System.Windows.Forms.NumericUpDown _pastDaysToShow;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
    private System.Windows.Forms.RadioButton _days;
    private System.Windows.Forms.RadioButton _months;
    private System.Windows.Forms.RadioButton _years;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.GroupBox groupBox3;
    private System.Windows.Forms.Button _pauseButton;
    private System.Windows.Forms.TrackBar trackBar1;
    private System.Windows.Forms.Button _stopButton;
    private System.Windows.Forms.TrackBar trackBar2;
    private System.Windows.Forms.NumericUpDown _stepAmount;
  }
}