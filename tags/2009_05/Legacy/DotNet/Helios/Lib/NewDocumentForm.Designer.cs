namespace CadKit.Helios.Lib
{
  partial class NewDocumentForm
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
      this._flowLayoutPanel = new System.Windows.Forms.FlowLayoutPanel();
      this.SuspendLayout();
      // 
      // _flowLayoutPanel
      // 
      this._flowLayoutPanel.AutoScroll = true;
      this._flowLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._flowLayoutPanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this._flowLayoutPanel.Location = new System.Drawing.Point(0, 0);
      this._flowLayoutPanel.Name = "_flowLayoutPanel";
      this._flowLayoutPanel.Size = new System.Drawing.Size(292, 266);
      this._flowLayoutPanel.TabIndex = 0;
      // 
      // NewDocumentForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(292, 266);
      this.Controls.Add(this._flowLayoutPanel);
      this.Name = "NewDocumentForm";
      this.TabText = "New Document";
      this.Text = "New Document";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.FlowLayoutPanel _flowLayoutPanel;

  }
}