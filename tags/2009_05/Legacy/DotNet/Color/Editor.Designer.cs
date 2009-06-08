namespace CadKit.Color
{
  partial class Editor
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Editor));
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this._wheel = new CadKit.Color.Wheel();
      this._value = new System.Windows.Forms.TrackBar();
      this.tableLayoutPanel1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this._value)).BeginInit();
      this.SuspendLayout();
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 2;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 80.19802F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 19.80198F));
      this.tableLayoutPanel1.Controls.Add(this._wheel, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this._value, 1, 0);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 1;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(303, 262);
      this.tableLayoutPanel1.TabIndex = 0;
      // 
      // _wheel
      // 
      this._wheel.Color = System.Drawing.Color.Black;
      this._wheel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._wheel.Location = new System.Drawing.Point(3, 3);
      this._wheel.Name = "_wheel";
      this._wheel.PersistentName = null;
      this._wheel.Point = new System.Drawing.Point(118, 128);
      this._wheel.PointF = ((System.Drawing.PointF)(resources.GetObject("_wheel.PointF")));
      this._wheel.Size = new System.Drawing.Size(237, 256);
      this._wheel.TabIndex = 0;
      // 
      // _value
      // 
      this._value.Dock = System.Windows.Forms.DockStyle.Right;
      this._value.Location = new System.Drawing.Point(255, 3);
      this._value.Name = "_value";
      this._value.Orientation = System.Windows.Forms.Orientation.Vertical;
      this._value.Size = new System.Drawing.Size(45, 256);
      this._value.TabIndex = 1;
      // 
      // Editor
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.tableLayoutPanel1);
      this.Name = "Editor";
      this.Size = new System.Drawing.Size(303, 262);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this._value)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private Wheel _wheel;
    private System.Windows.Forms.TrackBar _value;
  }
}
