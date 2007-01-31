namespace DT.Minerva.Lib
{
  partial class MergeData
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
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this._cancelButton = new System.Windows.Forms.Button();
      this._okButton = new System.Windows.Forms.Button();
      this._dataSources = new System.Windows.Forms.ListBox();
      this._tablesOne = new System.Windows.Forms.ListBox();
      this._tablesTwo = new System.Windows.Forms.ListBox();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
      this.textBox1 = new System.Windows.Forms.TextBox();
      this.comboBox1 = new System.Windows.Forms.ComboBox();
      this.comboBox2 = new System.Windows.Forms.ComboBox();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.flowLayoutPanel1);
      this.groupBox1.Location = new System.Drawing.Point(13, 296);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(750, 94);
      this.groupBox1.TabIndex = 3;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "groupBox1";
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.flowLayoutPanel2);
      this.groupBox2.Location = new System.Drawing.Point(13, 396);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(750, 101);
      this.groupBox2.TabIndex = 4;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "groupBox2";
      // 
      // _cancelButton
      // 
      this._cancelButton.Location = new System.Drawing.Point(688, 556);
      this._cancelButton.Name = "_cancelButton";
      this._cancelButton.Size = new System.Drawing.Size(75, 23);
      this._cancelButton.TabIndex = 5;
      this._cancelButton.Text = "Cancel";
      this._cancelButton.UseVisualStyleBackColor = true;
      this._cancelButton.Click += new System.EventHandler(this._cancelButton_Click);
      // 
      // _okButton
      // 
      this._okButton.Location = new System.Drawing.Point(607, 556);
      this._okButton.Name = "_okButton";
      this._okButton.Size = new System.Drawing.Size(75, 23);
      this._okButton.TabIndex = 6;
      this._okButton.Text = "OK";
      this._okButton.UseVisualStyleBackColor = true;
      this._okButton.Click += new System.EventHandler(this._okButton_Click);
      // 
      // _dataSources
      // 
      this._dataSources.FormattingEnabled = true;
      this._dataSources.Location = new System.Drawing.Point(13, 12);
      this._dataSources.Name = "_dataSources";
      this._dataSources.Size = new System.Drawing.Size(265, 277);
      this._dataSources.TabIndex = 7;
      // 
      // _tablesOne
      // 
      this._tablesOne.FormattingEnabled = true;
      this._tablesOne.Location = new System.Drawing.Point(310, 12);
      this._tablesOne.Name = "_tablesOne";
      this._tablesOne.Size = new System.Drawing.Size(212, 277);
      this._tablesOne.TabIndex = 8;
      // 
      // _tablesTwo
      // 
      this._tablesTwo.FormattingEnabled = true;
      this._tablesTwo.Location = new System.Drawing.Point(551, 12);
      this._tablesTwo.Name = "_tablesTwo";
      this._tablesTwo.Size = new System.Drawing.Size(212, 277);
      this._tablesTwo.TabIndex = 9;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(3, 16);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(744, 75);
      this.flowLayoutPanel1.TabIndex = 0;
      // 
      // flowLayoutPanel2
      // 
      this.flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel2.Location = new System.Drawing.Point(3, 16);
      this.flowLayoutPanel2.Name = "flowLayoutPanel2";
      this.flowLayoutPanel2.Size = new System.Drawing.Size(744, 82);
      this.flowLayoutPanel2.TabIndex = 0;
      // 
      // textBox1
      // 
      this.textBox1.Location = new System.Drawing.Point(16, 514);
      this.textBox1.Name = "textBox1";
      this.textBox1.Size = new System.Drawing.Size(262, 20);
      this.textBox1.TabIndex = 10;
      // 
      // comboBox1
      // 
      this.comboBox1.FormattingEnabled = true;
      this.comboBox1.Location = new System.Drawing.Point(310, 514);
      this.comboBox1.Name = "comboBox1";
      this.comboBox1.Size = new System.Drawing.Size(212, 21);
      this.comboBox1.TabIndex = 11;
      // 
      // comboBox2
      // 
      this.comboBox2.FormattingEnabled = true;
      this.comboBox2.Location = new System.Drawing.Point(551, 514);
      this.comboBox2.Name = "comboBox2";
      this.comboBox2.Size = new System.Drawing.Size(212, 21);
      this.comboBox2.TabIndex = 12;
      // 
      // MergeData
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(790, 601);
      this.Controls.Add(this.comboBox2);
      this.Controls.Add(this.comboBox1);
      this.Controls.Add(this.textBox1);
      this.Controls.Add(this._tablesTwo);
      this.Controls.Add(this._tablesOne);
      this.Controls.Add(this._dataSources);
      this.Controls.Add(this._okButton);
      this.Controls.Add(this._cancelButton);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Name = "MergeData";
      this.Text = "MergeData";
      this.groupBox1.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Button _cancelButton;
    private System.Windows.Forms.Button _okButton;
    private System.Windows.Forms.ListBox _dataSources;
    private System.Windows.Forms.ListBox _tablesOne;
    private System.Windows.Forms.ListBox _tablesTwo;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
    private System.Windows.Forms.TextBox textBox1;
    private System.Windows.Forms.ComboBox comboBox1;
    private System.Windows.Forms.ComboBox comboBox2;
  }
}