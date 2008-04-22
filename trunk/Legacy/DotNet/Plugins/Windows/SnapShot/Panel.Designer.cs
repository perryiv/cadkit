namespace CadKit.Plugins.Windows.SnapShot
{
  partial class SnapShotWindow
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
      this.components = new System.ComponentModel.Container();
      this._panel = new System.Windows.Forms.Panel();
      this._tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
      this._snapPictureButton = new System.Windows.Forms.Button();
      this._propertiesPanel = new System.Windows.Forms.Panel();
      this._propertiesGroupBox = new System.Windows.Forms.GroupBox();
      this._renderPassesComboBox = new System.Windows.Forms.ComboBox();
      this._scatterScaleTextBox = new System.Windows.Forms.TextBox();
      this._scatterScaleLabel = new System.Windows.Forms.Label();
      this._renderPassesLabel = new System.Windows.Forms.Label();
      this._frameScaleTextBox = new System.Windows.Forms.TextBox();
      this._scaleLabel = new System.Windows.Forms.Label();
      this._errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
      this._panel.SuspendLayout();
      this._tableLayoutPanel.SuspendLayout();
      this._propertiesPanel.SuspendLayout();
      this._propertiesGroupBox.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this._errorProvider)).BeginInit();
      this.SuspendLayout();
      // 
      // _panel
      // 
      this._panel.BackColor = System.Drawing.SystemColors.Control;
      this._panel.Controls.Add(this._tableLayoutPanel);
      this._panel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._panel.Location = new System.Drawing.Point(0, 0);
      this._panel.Name = "_panel";
      this._panel.Size = new System.Drawing.Size(202, 177);
      this._panel.TabIndex = 0;
      this._panel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      // 
      // _tableLayoutPanel
      // 
      this._tableLayoutPanel.ColumnCount = 1;
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayoutPanel.Controls.Add(this._snapPictureButton, 0, 1);
      this._tableLayoutPanel.Controls.Add(this._propertiesPanel, 0, 0);
      this._tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._tableLayoutPanel.Location = new System.Drawing.Point(0, 0);
      this._tableLayoutPanel.Name = "_tableLayoutPanel";
      this._tableLayoutPanel.RowCount = 2;
      this._tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
      this._tableLayoutPanel.Size = new System.Drawing.Size(202, 177);
      this._tableLayoutPanel.TabIndex = 1;
      // 
      // _snapPictureButton
      // 
      this._snapPictureButton.Anchor = System.Windows.Forms.AnchorStyles.None;
      this._snapPictureButton.Location = new System.Drawing.Point(55, 145);
      this._snapPictureButton.Name = "_snapPictureButton";
      this._snapPictureButton.Size = new System.Drawing.Size(91, 23);
      this._snapPictureButton.TabIndex = 1;
      this._snapPictureButton.Text = "Take Picture";
      this._snapPictureButton.UseVisualStyleBackColor = true;
      this._snapPictureButton.Click += new System.EventHandler(this._snapPictureButtonClick);
      this._snapPictureButton.Paint += new System.Windows.Forms.PaintEventHandler(this._snapPictureButtonPaint);
      // 
      // _propertiesPanel
      // 
      this._propertiesPanel.Anchor = System.Windows.Forms.AnchorStyles.None;
      this._propertiesPanel.BackColor = System.Drawing.SystemColors.ControlDark;
      this._propertiesPanel.Controls.Add(this._propertiesGroupBox);
      this._propertiesPanel.Location = new System.Drawing.Point(14, 12);
      this._propertiesPanel.Name = "_propertiesPanel";
      this._propertiesPanel.Size = new System.Drawing.Size(173, 113);
      this._propertiesPanel.TabIndex = 2;
      // 
      // _propertiesGroupBox
      // 
      this._propertiesGroupBox.BackColor = System.Drawing.SystemColors.Control;
      this._propertiesGroupBox.Controls.Add(this._renderPassesComboBox);
      this._propertiesGroupBox.Controls.Add(this._scatterScaleTextBox);
      this._propertiesGroupBox.Controls.Add(this._scatterScaleLabel);
      this._propertiesGroupBox.Controls.Add(this._renderPassesLabel);
      this._propertiesGroupBox.Controls.Add(this._frameScaleTextBox);
      this._propertiesGroupBox.Controls.Add(this._scaleLabel);
      this._propertiesGroupBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this._propertiesGroupBox.Location = new System.Drawing.Point(0, 0);
      this._propertiesGroupBox.Name = "_propertiesGroupBox";
      this._propertiesGroupBox.Size = new System.Drawing.Size(173, 113);
      this._propertiesGroupBox.TabIndex = 6;
      this._propertiesGroupBox.TabStop = false;
      this._propertiesGroupBox.Text = "Picture Properties";
      // 
      // _renderPassesComboBox
      // 
      this._renderPassesComboBox.FormattingEnabled = true;
      this._renderPassesComboBox.Location = new System.Drawing.Point(98, 50);
      this._renderPassesComboBox.Name = "_renderPassesComboBox";
      this._renderPassesComboBox.Size = new System.Drawing.Size(60, 21);
      this._renderPassesComboBox.TabIndex = 3;
      // 
      // _scatterScaleTextBox
      // 
      this._scatterScaleTextBox.Location = new System.Drawing.Point(98, 77);
      this._scatterScaleTextBox.Name = "_scatterScaleTextBox";
      this._scatterScaleTextBox.Size = new System.Drawing.Size(60, 20);
      this._scatterScaleTextBox.TabIndex = 4;
      // 
      // _scatterScaleLabel
      // 
      this._scatterScaleLabel.AutoSize = true;
      this._scatterScaleLabel.Location = new System.Drawing.Point(21, 80);
      this._scatterScaleLabel.Name = "_scatterScaleLabel";
      this._scatterScaleLabel.Size = new System.Drawing.Size(71, 13);
      this._scatterScaleLabel.TabIndex = 10;
      this._scatterScaleLabel.Text = "Scatter Scale";
      // 
      // _renderPassesLabel
      // 
      this._renderPassesLabel.AutoSize = true;
      this._renderPassesLabel.Location = new System.Drawing.Point(13, 53);
      this._renderPassesLabel.Name = "_renderPassesLabel";
      this._renderPassesLabel.Size = new System.Drawing.Size(79, 13);
      this._renderPassesLabel.TabIndex = 9;
      this._renderPassesLabel.Text = "Render Passes";
      // 
      // _frameScaleTextBox
      // 
      this._frameScaleTextBox.Location = new System.Drawing.Point(98, 24);
      this._frameScaleTextBox.Name = "_frameScaleTextBox";
      this._frameScaleTextBox.Size = new System.Drawing.Size(60, 20);
      this._frameScaleTextBox.TabIndex = 2;
      // 
      // _scaleLabel
      // 
      this._scaleLabel.AutoSize = true;
      this._scaleLabel.Location = new System.Drawing.Point(26, 27);
      this._scaleLabel.Name = "_scaleLabel";
      this._scaleLabel.Size = new System.Drawing.Size(66, 13);
      this._scaleLabel.TabIndex = 6;
      this._scaleLabel.Text = "Frame Scale";
      // 
      // _errorProvider
      // 
      this._errorProvider.ContainerControl = this;
      // 
      // SnapShotWindow
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(202, 177);
      this.Controls.Add(this._panel);
      this.Name = "SnapShotWindow";
      this.TabText = "SnapShot Window";
      this.Text = "SnapShot Window";
      this._panel.ResumeLayout(false);
      this._tableLayoutPanel.ResumeLayout(false);
      this._propertiesPanel.ResumeLayout(false);
      this._propertiesGroupBox.ResumeLayout(false);
      this._propertiesGroupBox.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this._errorProvider)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Panel _panel;
    private System.Windows.Forms.TableLayoutPanel _tableLayoutPanel;
    private System.Windows.Forms.Button _snapPictureButton;
    private System.Windows.Forms.Panel _propertiesPanel;
    private System.Windows.Forms.GroupBox _propertiesGroupBox;
    private System.Windows.Forms.TextBox _scatterScaleTextBox;
    private System.Windows.Forms.Label _scatterScaleLabel;
    private System.Windows.Forms.Label _renderPassesLabel;
    private System.Windows.Forms.TextBox _frameScaleTextBox;
    private System.Windows.Forms.Label _scaleLabel;
    private System.Windows.Forms.ErrorProvider _errorProvider;
    private System.Windows.Forms.ComboBox _renderPassesComboBox;
  }
}