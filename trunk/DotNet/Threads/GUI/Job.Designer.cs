namespace CadKit.Threads.GUI
{
  partial class Job
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
      this._tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
      this._label = new System.Windows.Forms.Label();
      this._progressBar = new System.Windows.Forms.ProgressBar();
      this._buttonRun = new System.Windows.Forms.Button();
      this._buttonRemove = new System.Windows.Forms.Button();
      this._tableLayoutPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // _tableLayoutPanel
      // 
      this._tableLayoutPanel.BackColor = System.Drawing.SystemColors.Control;
      this._tableLayoutPanel.ColumnCount = 4;
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 120F));
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 81F));
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 81F));
      this._tableLayoutPanel.Controls.Add(this._label, 0, 0);
      this._tableLayoutPanel.Controls.Add(this._progressBar, 1, 0);
      this._tableLayoutPanel.Controls.Add(this._buttonRun, 2, 0);
      this._tableLayoutPanel.Controls.Add(this._buttonRemove, 3, 0);
      this._tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._tableLayoutPanel.Location = new System.Drawing.Point(2, 2);
      this._tableLayoutPanel.Name = "_tableLayoutPanel";
      this._tableLayoutPanel.RowCount = 1;
      this._tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this._tableLayoutPanel.Size = new System.Drawing.Size(396, 36);
      this._tableLayoutPanel.TabIndex = 0;
      // 
      // _label
      // 
      this._label.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._label.Location = new System.Drawing.Point(3, 6);
      this._label.Name = "_label";
      this._label.Size = new System.Drawing.Size(114, 23);
      this._label.TabIndex = 0;
      this._label.Text = "Running...";
      this._label.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // _progressBar
      // 
      this._progressBar.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._progressBar.Location = new System.Drawing.Point(123, 6);
      this._progressBar.Name = "_progressBar";
      this._progressBar.Size = new System.Drawing.Size(108, 23);
      this._progressBar.TabIndex = 0;
      // 
      // _buttonRun
      // 
      this._buttonRun.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._buttonRun.Location = new System.Drawing.Point(237, 6);
      this._buttonRun.Name = "_buttonRun";
      this._buttonRun.Size = new System.Drawing.Size(75, 23);
      this._buttonRun.TabIndex = 1;
      this._buttonRun.Text = "Pause";
      this._buttonRun.UseVisualStyleBackColor = false;
      // 
      // _buttonRemove
      // 
      this._buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._buttonRemove.Location = new System.Drawing.Point(318, 6);
      this._buttonRemove.Name = "_buttonRemove";
      this._buttonRemove.Size = new System.Drawing.Size(75, 23);
      this._buttonRemove.TabIndex = 0;
      this._buttonRemove.Text = "Remove";
      this._buttonRemove.UseVisualStyleBackColor = false;
      // 
      // Job
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.SystemColors.Control;
      this.Controls.Add(this._tableLayoutPanel);
      this.MinimumSize = new System.Drawing.Size(400, 40);
      this.Name = "Job";
      this.Padding = new System.Windows.Forms.Padding(2);
      this.Size = new System.Drawing.Size(400, 40);
      this._tableLayoutPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TableLayoutPanel _tableLayoutPanel;
    private System.Windows.Forms.Label _label;
    private System.Windows.Forms.ProgressBar _progressBar;
    private System.Windows.Forms.Button _buttonRun;
    private System.Windows.Forms.Button _buttonRemove;
  }
}
