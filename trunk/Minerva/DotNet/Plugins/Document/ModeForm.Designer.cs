namespace DT.Minerva.Plugins.Document
{
  partial class ModeForm
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
      this._dll = new System.Windows.Forms.Button();
      this._distributed = new System.Windows.Forms.Button();
      this._both = new System.Windows.Forms.Button();
      this._decision = new System.Windows.Forms.CheckBox();
      this.SuspendLayout();
      // 
      // _dll
      // 
      this._dll.Location = new System.Drawing.Point(12, 39);
      this._dll.Name = "_dll";
      this._dll.Size = new System.Drawing.Size(75, 23);
      this._dll.TabIndex = 0;
      this._dll.Text = "Dll";
      this._dll.UseVisualStyleBackColor = true;
      this._dll.Click += new System.EventHandler(this._dll_Click);
      // 
      // _distributed
      // 
      this._distributed.Location = new System.Drawing.Point(128, 39);
      this._distributed.Name = "_distributed";
      this._distributed.Size = new System.Drawing.Size(75, 23);
      this._distributed.TabIndex = 1;
      this._distributed.Text = "Distrubuted";
      this._distributed.UseVisualStyleBackColor = true;
      this._distributed.Click += new System.EventHandler(this._distributed_Click);
      // 
      // _both
      // 
      this._both.Location = new System.Drawing.Point(244, 39);
      this._both.Name = "_both";
      this._both.Size = new System.Drawing.Size(75, 23);
      this._both.TabIndex = 2;
      this._both.Text = "Both";
      this._both.UseVisualStyleBackColor = true;
      this._both.Click += new System.EventHandler(this._both_Click);
      // 
      // _decision
      // 
      this._decision.AutoSize = true;
      this._decision.Location = new System.Drawing.Point(96, 12);
      this._decision.Name = "_decision";
      this._decision.Size = new System.Drawing.Size(138, 17);
      this._decision.TabIndex = 3;
      this._decision.Text = "Remember My Decision";
      this._decision.UseVisualStyleBackColor = true;
      // 
      // ModeForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(352, 74);
      this.Controls.Add(this._decision);
      this.Controls.Add(this._both);
      this.Controls.Add(this._distributed);
      this.Controls.Add(this._dll);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ModeForm";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "Choose Mode";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button _dll;
    private System.Windows.Forms.Button _distributed;
    private System.Windows.Forms.Button _both;
    private System.Windows.Forms.CheckBox _decision;
  }
}