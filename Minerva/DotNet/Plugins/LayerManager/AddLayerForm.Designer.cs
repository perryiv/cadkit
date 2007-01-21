namespace DT.Minerva.Plugins.LayerManager
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
      this._listBox = new System.Windows.Forms.ListBox();
      this._propertyGrid = new System.Windows.Forms.PropertyGrid();
      this._addDataBase = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // _connections
      // 
      this._connections.FormattingEnabled = true;
      this._connections.Location = new System.Drawing.Point(121, 12);
      this._connections.Name = "_connections";
      this._connections.Size = new System.Drawing.Size(328, 21);
      this._connections.TabIndex = 0;
      // 
      // _listBox
      // 
      this._listBox.FormattingEnabled = true;
      this._listBox.Location = new System.Drawing.Point(12, 54);
      this._listBox.Name = "_listBox";
      this._listBox.Size = new System.Drawing.Size(310, 368);
      this._listBox.TabIndex = 1;
      // 
      // _propertyGrid
      // 
      this._propertyGrid.Location = new System.Drawing.Point(338, 54);
      this._propertyGrid.Name = "_propertyGrid";
      this._propertyGrid.Size = new System.Drawing.Size(370, 368);
      this._propertyGrid.TabIndex = 2;
      // 
      // _addDataBase
      // 
      this._addDataBase.Location = new System.Drawing.Point(485, 10);
      this._addDataBase.Name = "_addDataBase";
      this._addDataBase.Size = new System.Drawing.Size(75, 23);
      this._addDataBase.TabIndex = 5;
      this._addDataBase.Text = "...";
      this._addDataBase.UseVisualStyleBackColor = true;
      this._addDataBase.Click += new System.EventHandler(this._addDataBase_Click);
      // 
      // AddLayerForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this._connections);
      this.Controls.Add(this._addDataBase);
      this.Controls.Add(this._propertyGrid);
      this.Controls.Add(this._listBox);
      this.Name = "AddLayerForm";
      this.Size = new System.Drawing.Size(720, 482);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.ComboBox _connections;
    private System.Windows.Forms.ListBox _listBox;
    private System.Windows.Forms.PropertyGrid _propertyGrid;
    private System.Windows.Forms.Button _addDataBase;
  }
}