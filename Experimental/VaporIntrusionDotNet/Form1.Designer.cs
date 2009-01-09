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
      this.MenuBar = new System.Windows.Forms.MenuStrip();
      this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.windowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.MenuBar.SuspendLayout();
      this.SuspendLayout();
      // 
      // MenuBar
      // 
      this.MenuBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem,
            this.windowToolStripMenuItem});
      this.MenuBar.Location = new System.Drawing.Point(0, 0);
      this.MenuBar.Name = "MenuBar";
      this.MenuBar.Size = new System.Drawing.Size(816, 26);
      this.MenuBar.TabIndex = 1;
      this.MenuBar.Text = "menuStrip1";
      this.MenuBar.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.MenuBar_ItemClicked_1);
      // 
      // fileToolStripMenuItem
      // 
      this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      this.fileToolStripMenuItem.Size = new System.Drawing.Size(40, 22);
      this.fileToolStripMenuItem.Text = "File";
      // 
      // editToolStripMenuItem
      // 
      this.editToolStripMenuItem.Name = "editToolStripMenuItem";
      this.editToolStripMenuItem.Size = new System.Drawing.Size(43, 22);
      this.editToolStripMenuItem.Text = "Edit";
      // 
      // windowToolStripMenuItem
      // 
      this.windowToolStripMenuItem.Name = "windowToolStripMenuItem";
      this.windowToolStripMenuItem.Size = new System.Drawing.Size(70, 22);
      this.windowToolStripMenuItem.Text = "Window";
      // 
      // viewToolStripMenuItem
      // 
      this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
      this.viewToolStripMenuItem.Size = new System.Drawing.Size(49, 22);
      this.viewToolStripMenuItem.Text = "View";
      // 
      // Form1
      // 
      this.ClientSize = new System.Drawing.Size(816, 613);
      this.Controls.Add(this.MenuBar);
      this.IsMdiContainer = true;
      this.MainMenuStrip = this.MenuBar;
      this.Name = "Form1";
      this.Text = "Vapor Intrusion Simulation Interface";
      this.Load += new System.EventHandler(this.Form1_Load_1);
      this.MenuBar.ResumeLayout(false);
      this.MenuBar.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.MenuStrip MenuBar;
    private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem windowToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
  }
}

