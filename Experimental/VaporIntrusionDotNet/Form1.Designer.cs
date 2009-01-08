namespace VaporIntrusionDotNet
{
  partial class Form1
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
      this.menuBar = new System.Windows.Forms.MenuStrip();
      this.fileMenuBarItem = new System.Windows.Forms.ToolStripMenuItem();
      this.editMenuBarItem = new System.Windows.Forms.ToolStripMenuItem();
      this.menuBar.SuspendLayout();
      this.SuspendLayout();
      // 
      // menuBar
      // 
      this.menuBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileMenuBarItem,
            this.editMenuBarItem});
      this.menuBar.Location = new System.Drawing.Point(0, 0);
      this.menuBar.Name = "menuBar";
      this.menuBar.Size = new System.Drawing.Size(952, 26);
      this.menuBar.TabIndex = 0;
      this.menuBar.Text = "MenuBar";
      // 
      // fileMenuBarItem
      // 
      this.fileMenuBarItem.Name = "fileMenuBarItem";
      this.fileMenuBarItem.Size = new System.Drawing.Size(40, 22);
      this.fileMenuBarItem.Text = "File";
      // 
      // editMenuBarItem
      // 
      this.editMenuBarItem.Name = "editMenuBarItem";
      this.editMenuBarItem.Size = new System.Drawing.Size(43, 22);
      this.editMenuBarItem.Text = "Edit";
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(952, 678);
      this.Controls.Add(this.menuBar);
      this.IsMdiContainer = true;
      this.MainMenuStrip = this.menuBar;
      this.Name = "Form1";
      this.Text = "Vapor Intrusion Simulation Setup";
      this.Load += new System.EventHandler(this.Form1_Load);
      this.menuBar.ResumeLayout(false);
      this.menuBar.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.MenuStrip menuBar;
    private System.Windows.Forms.ToolStripMenuItem fileMenuBarItem;
    private System.Windows.Forms.ToolStripMenuItem editMenuBarItem;
  }
}

