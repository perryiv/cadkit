namespace DT.Minerva.Layers.Colors
{
  partial class ColorChooser
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
      this._okButton.Click -= this._okButton_Click;
      this._cancel.Click -= this._cancel_Click;

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
      this._singleColor = new System.Windows.Forms.RadioButton();
      this._gradientColor = new System.Windows.Forms.RadioButton();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this._okButton = new System.Windows.Forms.Button();
      this._cancel = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // _singleColor
      // 
      this._singleColor.AutoSize = true;
      this._singleColor.Location = new System.Drawing.Point(3, 19);
      this._singleColor.Name = "_singleColor";
      this._singleColor.Size = new System.Drawing.Size(81, 17);
      this._singleColor.TabIndex = 0;
      this._singleColor.TabStop = true;
      this._singleColor.Text = "Single Color";
      this._singleColor.UseVisualStyleBackColor = true;
      this._singleColor.CheckedChanged += new System.EventHandler(this._singleColor_CheckedChanged);
      // 
      // _gradientColor
      // 
      this._gradientColor.AutoSize = true;
      this._gradientColor.Location = new System.Drawing.Point(115, 19);
      this._gradientColor.Name = "_gradientColor";
      this._gradientColor.Size = new System.Drawing.Size(92, 17);
      this._gradientColor.TabIndex = 1;
      this._gradientColor.TabStop = true;
      this._gradientColor.Text = "Gradient Color";
      this._gradientColor.UseVisualStyleBackColor = true;
      this._gradientColor.CheckedChanged += new System.EventHandler(this._gradientColor_CheckedChanged);
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Location = new System.Drawing.Point(3, 43);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(198, 49);
      this.flowLayoutPanel1.TabIndex = 2;
      // 
      // _okButton
      // 
      this._okButton.Location = new System.Drawing.Point(3, 108);
      this._okButton.Name = "_okButton";
      this._okButton.Size = new System.Drawing.Size(75, 23);
      this._okButton.TabIndex = 3;
      this._okButton.Text = "OK";
      this._okButton.UseVisualStyleBackColor = true;
      this._okButton.Click += new System.EventHandler(this._okButton_Click);
      // 
      // _cancel
      // 
      this._cancel.Location = new System.Drawing.Point(126, 108);
      this._cancel.Name = "_cancel";
      this._cancel.Size = new System.Drawing.Size(75, 23);
      this._cancel.TabIndex = 4;
      this._cancel.Text = "Cancel";
      this._cancel.UseVisualStyleBackColor = true;
      this._cancel.Click += new System.EventHandler(this._cancel_Click);
      // 
      // ColorChooser
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this._cancel);
      this.Controls.Add(this._okButton);
      this.Controls.Add(this.flowLayoutPanel1);
      this.Controls.Add(this._gradientColor);
      this.Controls.Add(this._singleColor);
      this.Name = "ColorChooser";
      this.Size = new System.Drawing.Size(207, 145);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.RadioButton _singleColor;
    private System.Windows.Forms.RadioButton _gradientColor;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
    private System.Windows.Forms.Button _okButton;
    private System.Windows.Forms.Button _cancel;
  }
}
