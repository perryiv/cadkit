namespace DT.Minerva.Plugins.Layers.PostGIS
{
  partial class Properties
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
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this._propertyGrid = new System.Windows.Forms.PropertyGrid();
      this.panel1 = new System.Windows.Forms.Panel();
      this._cancel = new System.Windows.Forms.Button();
      this._ok = new System.Windows.Forms.Button();
      this.tableLayoutPanel1.SuspendLayout();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 1;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.Controls.Add(this._propertyGrid, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 1);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 2;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 87.27679F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 12.72321F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(333, 448);
      this.tableLayoutPanel1.TabIndex = 0;
      // 
      // _propertyGrid
      // 
      this._propertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
      this._propertyGrid.Location = new System.Drawing.Point(3, 3);
      this._propertyGrid.Name = "_propertyGrid";
      this._propertyGrid.Size = new System.Drawing.Size(327, 385);
      this._propertyGrid.TabIndex = 0;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this._cancel);
      this.panel1.Controls.Add(this._ok);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(3, 394);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(327, 51);
      this.panel1.TabIndex = 1;
      // 
      // _cancel
      // 
      this._cancel.Location = new System.Drawing.Point(243, 14);
      this._cancel.Name = "_cancel";
      this._cancel.Size = new System.Drawing.Size(75, 23);
      this._cancel.TabIndex = 1;
      this._cancel.Text = "Cancel";
      this._cancel.UseVisualStyleBackColor = true;
      this._cancel.Click += new System.EventHandler(this._cancel_Click);
      // 
      // _ok
      // 
      this._ok.Location = new System.Drawing.Point(9, 14);
      this._ok.Name = "_ok";
      this._ok.Size = new System.Drawing.Size(75, 23);
      this._ok.TabIndex = 0;
      this._ok.Text = "OK";
      this._ok.UseVisualStyleBackColor = true;
      this._ok.Click += new System.EventHandler(this._ok_Click);
      // 
      // Properties
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(333, 448);
      this.Controls.Add(this.tableLayoutPanel1);
      this.Name = "Properties";
      this.Text = "Properties";
      this.tableLayoutPanel1.ResumeLayout(false);
      this.panel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.PropertyGrid _propertyGrid;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Button _cancel;
    private System.Windows.Forms.Button _ok;
  }
}