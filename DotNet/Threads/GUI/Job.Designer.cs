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
      this._tableLayout = new System.Windows.Forms.TableLayoutPanel();
      this._label = new System.Windows.Forms.Label();
      this._progressBar = new System.Windows.Forms.ProgressBar();
      this._buttonRemove = new System.Windows.Forms.Button();
      this._tableLayout.SuspendLayout();
      this.SuspendLayout();
      // 
      // _tableLayout
      // 
      this._tableLayout.BackColor = System.Drawing.SystemColors.Control;
      this._tableLayout.ColumnCount = 3;
      this._tableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 120F));
      this._tableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 36F));
      this._tableLayout.Controls.Add(this._label, 0, 0);
      this._tableLayout.Controls.Add(this._progressBar, 1, 0);
      this._tableLayout.Controls.Add(this._buttonRemove, 2, 0);
      this._tableLayout.Dock = System.Windows.Forms.DockStyle.Fill;
      this._tableLayout.Location = new System.Drawing.Point(2, 2);
      this._tableLayout.Name = "_tableLayout";
      this._tableLayout.RowCount = 1;
      this._tableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this._tableLayout.Size = new System.Drawing.Size(396, 36);
      this._tableLayout.TabIndex = 0;
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
      this._progressBar.Size = new System.Drawing.Size(234, 23);
      this._progressBar.TabIndex = 0;
      // 
      // _buttonRemove
      // 
      this._buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this._buttonRemove.FlatAppearance.BorderSize = 0;
      this._buttonRemove.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
      this._buttonRemove.Image = global::CadKit.Threads.GUI.Properties.Resources.close;
      this._buttonRemove.Location = new System.Drawing.Point(363, 8);
      this._buttonRemove.Name = "_buttonRemove";
      this._buttonRemove.Size = new System.Drawing.Size(30, 20);
      this._buttonRemove.TabIndex = 0;
      this._buttonRemove.UseVisualStyleBackColor = false;
      this._buttonRemove.Click += new System.EventHandler(this._buttonRemoveClick);
      // 
      // Job
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.BackColor = System.Drawing.SystemColors.Control;
      this.Controls.Add(this._tableLayout);
      this.MinimumSize = new System.Drawing.Size(400, 40);
      this.Name = "Job";
      this.Padding = new System.Windows.Forms.Padding(2);
      this.Size = new System.Drawing.Size(400, 40);
      this._tableLayout.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TableLayoutPanel _tableLayout;
    private System.Windows.Forms.Label _label;
    private System.Windows.Forms.ProgressBar _progressBar;
    private System.Windows.Forms.Button _buttonRemove;
  }
}
