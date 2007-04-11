namespace DT.Minerva.Plugins.Options
{
  partial class PageContent
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this._ossimPlanetGroupBox = new System.Windows.Forms.GroupBox();
      this._legend = new System.Windows.Forms.CheckBox();
      this._percentScreenWidthSpinner = new System.Windows.Forms.NumericUpDown();
      this._percentScreenWidthLabel = new System.Windows.Forms.Label();
      this.flowLayoutPanel1.SuspendLayout();
      this._ossimPlanetGroupBox.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this._percentScreenWidthSpinner)).BeginInit();
      this.SuspendLayout();
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Controls.Add(this.groupBox1);
      this.flowLayoutPanel1.Controls.Add(this._ossimPlanetGroupBox);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(452, 265);
      this.flowLayoutPanel1.TabIndex = 0;
      // 
      // groupBox1
      // 
      this.groupBox1.Location = new System.Drawing.Point(3, 3);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(0, 158);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "groupBox1";
      // 
      // _ossimPlanetGroupBox
      // 
      this._ossimPlanetGroupBox.Controls.Add(this._percentScreenWidthLabel);
      this._ossimPlanetGroupBox.Controls.Add(this._percentScreenWidthSpinner);
      this._ossimPlanetGroupBox.Controls.Add(this._legend);
      this._ossimPlanetGroupBox.Location = new System.Drawing.Point(9, 3);
      this._ossimPlanetGroupBox.Name = "_ossimPlanetGroupBox";
      this._ossimPlanetGroupBox.Size = new System.Drawing.Size(440, 221);
      this._ossimPlanetGroupBox.TabIndex = 1;
      this._ossimPlanetGroupBox.TabStop = false;
      this._ossimPlanetGroupBox.Text = "Minerva";
      // 
      // _legend
      // 
      this._legend.AutoSize = true;
      this._legend.Location = new System.Drawing.Point(17, 19);
      this._legend.Name = "_legend";
      this._legend.Size = new System.Drawing.Size(62, 17);
      this._legend.TabIndex = 2;
      this._legend.Text = "Legend";
      this._legend.UseVisualStyleBackColor = true;
      // 
      // _percentScreenWidthSpinner
      // 
      this._percentScreenWidthSpinner.DecimalPlaces = 2;
      this._percentScreenWidthSpinner.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
      this._percentScreenWidthSpinner.Location = new System.Drawing.Point(132, 60);
      this._percentScreenWidthSpinner.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
      this._percentScreenWidthSpinner.Name = "_percentScreenWidthSpinner";
      this._percentScreenWidthSpinner.Size = new System.Drawing.Size(62, 20);
      this._percentScreenWidthSpinner.TabIndex = 3;
      // 
      // _percentScreenWidthLabel
      // 
      this._percentScreenWidthLabel.AutoSize = true;
      this._percentScreenWidthLabel.Location = new System.Drawing.Point(14, 62);
      this._percentScreenWidthLabel.Name = "_percentScreenWidthLabel";
      this._percentScreenWidthLabel.Size = new System.Drawing.Size(112, 13);
      this._percentScreenWidthLabel.TabIndex = 4;
      this._percentScreenWidthLabel.Text = "Percent Screen Width";
      // 
      // PageContent
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.flowLayoutPanel1);
      this.Name = "PageContent";
      this.Size = new System.Drawing.Size(452, 265);
      this.flowLayoutPanel1.ResumeLayout(false);
      this._ossimPlanetGroupBox.ResumeLayout(false);
      this._ossimPlanetGroupBox.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this._percentScreenWidthSpinner)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox _ossimPlanetGroupBox;
    private System.Windows.Forms.CheckBox _legend;
    private System.Windows.Forms.Label _percentScreenWidthLabel;
    private System.Windows.Forms.NumericUpDown _percentScreenWidthSpinner;
  }
}
