namespace DT.Minerva.Plugins.Layers.PostGIS
{
  partial class AddLayerForm
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
      this._connections = new System.Windows.Forms.ComboBox();
      this._datatables = new System.Windows.Forms.ListBox();
      this._addDataBase = new System.Windows.Forms.Button();
      this._favorites = new System.Windows.Forms.ListBox();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.panel1 = new System.Windows.Forms.Panel();
      this.tableLayoutPanel1.SuspendLayout();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // _connections
      // 
      this._connections.FormattingEnabled = true;
      this._connections.Location = new System.Drawing.Point(3, 3);
      this._connections.Name = "_connections";
      this._connections.Size = new System.Drawing.Size(485, 21);
      this._connections.TabIndex = 0;
      // 
      // _datatables
      // 
      this._datatables.Dock = System.Windows.Forms.DockStyle.Fill;
      this._datatables.FormattingEnabled = true;
      this._datatables.Location = new System.Drawing.Point(3, 63);
      this._datatables.Name = "_datatables";
      this._datatables.Size = new System.Drawing.Size(755, 264);
      this._datatables.TabIndex = 1;
      // 
      // _addDataBase
      // 
      this._addDataBase.Location = new System.Drawing.Point(494, 1);
      this._addDataBase.Name = "_addDataBase";
      this._addDataBase.Size = new System.Drawing.Size(75, 23);
      this._addDataBase.TabIndex = 5;
      this._addDataBase.Text = "...";
      this._addDataBase.UseVisualStyleBackColor = true;
      this._addDataBase.Click += new System.EventHandler(this._addDataBase_Click);
      // 
      // _favorites
      // 
      this._favorites.Dock = System.Windows.Forms.DockStyle.Fill;
      this._favorites.FormattingEnabled = true;
      this._favorites.Location = new System.Drawing.Point(3, 333);
      this._favorites.Name = "_favorites";
      this._favorites.Size = new System.Drawing.Size(755, 264);
      this._favorites.TabIndex = 6;
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 1;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.Controls.Add(this._datatables, 0, 1);
      this.tableLayoutPanel1.Controls.Add(this._favorites, 0, 2);
      this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 0);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 3;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 10F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 45F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 45F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(761, 600);
      this.tableLayoutPanel1.TabIndex = 7;
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this._connections);
      this.panel1.Controls.Add(this._addDataBase);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(3, 3);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(755, 54);
      this.panel1.TabIndex = 7;
      // 
      // AddLayerForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.tableLayoutPanel1);
      this.Name = "AddLayerForm";
      this.Size = new System.Drawing.Size(761, 600);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.panel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.ComboBox _connections;
    private System.Windows.Forms.ListBox _datatables;
    private System.Windows.Forms.Button _addDataBase;
    private System.Windows.Forms.ListBox _favorites;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.Panel panel1;
  }
}