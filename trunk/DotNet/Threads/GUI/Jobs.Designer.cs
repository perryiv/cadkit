namespace CadKit.Threads.GUI
{
  partial class Jobs
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
      this._panel = new System.Windows.Forms.Panel();
      this._layout = new System.Windows.Forms.FlowLayoutPanel();
      this._panel.SuspendLayout();
      this.SuspendLayout();
      // 
      // _panel
      // 
      this._panel.Controls.Add(this._layout);
      this._panel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._panel.Location = new System.Drawing.Point(0, 0);
      this._panel.Name = "_panel";
      this._panel.Size = new System.Drawing.Size(572, 249);
      this._panel.TabIndex = 0;
      // 
      // _layout
      // 
      this._layout.AutoScroll = true;
      this._layout.Dock = System.Windows.Forms.DockStyle.Fill;
      this._layout.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this._layout.Location = new System.Drawing.Point(0, 0);
      this._layout.Name = "_layout";
      this._layout.Size = new System.Drawing.Size(572, 249);
      this._layout.TabIndex = 0;
      this._layout.WrapContents = false;
      // 
      // Jobs
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.Controls.Add(this._panel);
      this.Name = "Jobs";
      this.Size = new System.Drawing.Size(572, 249);
      this._panel.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Panel _panel;
    private System.Windows.Forms.FlowLayoutPanel _layout;
  }
}
