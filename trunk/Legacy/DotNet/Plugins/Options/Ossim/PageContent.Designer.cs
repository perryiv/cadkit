namespace CadKit.Plugins.Options.Ossim
{
  partial class PageContent
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
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this._ossimPlanetGroupBox = new System.Windows.Forms.GroupBox();
      this.label1 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this._elevationPatch = new System.Windows.Forms.NumericUpDown();
      this._maxLevelDetail = new System.Windows.Forms.NumericUpDown();
      this._heightExag = new System.Windows.Forms.NumericUpDown();
      this._browse = new System.Windows.Forms.Button();
      this._cacheDir = new System.Windows.Forms.TextBox();
      this._hud = new System.Windows.Forms.CheckBox();
      this._ephemeris = new System.Windows.Forms.CheckBox();
      this._elevation = new System.Windows.Forms.CheckBox();
      this._latLongGrid = new System.Windows.Forms.CheckBox();
      this.flowLayoutPanel1.SuspendLayout();
      this._ossimPlanetGroupBox.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this._elevationPatch)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this._maxLevelDetail)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this._heightExag)).BeginInit();
      this.SuspendLayout();
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.Controls.Add(this.groupBox1);
      this.flowLayoutPanel1.Controls.Add(this._ossimPlanetGroupBox);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(452, 265);
      this.flowLayoutPanel1.TabIndex = 0;
      // 
      // groupBox1
      // 
      this.groupBox1.Location = new System.Drawing.Point(3, 3);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(0, 158);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "groupBox1";
      // 
      // _ossimPlanetGroupBox
      // 
      this._ossimPlanetGroupBox.Controls.Add(this._latLongGrid);
      this._ossimPlanetGroupBox.Controls.Add(this.label1);
      this._ossimPlanetGroupBox.Controls.Add(this.label4);
      this._ossimPlanetGroupBox.Controls.Add(this.label3);
      this._ossimPlanetGroupBox.Controls.Add(this.label2);
      this._ossimPlanetGroupBox.Controls.Add(this._elevationPatch);
      this._ossimPlanetGroupBox.Controls.Add(this._maxLevelDetail);
      this._ossimPlanetGroupBox.Controls.Add(this._heightExag);
      this._ossimPlanetGroupBox.Controls.Add(this._browse);
      this._ossimPlanetGroupBox.Controls.Add(this._cacheDir);
      this._ossimPlanetGroupBox.Controls.Add(this._hud);
      this._ossimPlanetGroupBox.Controls.Add(this._ephemeris);
      this._ossimPlanetGroupBox.Controls.Add(this._elevation);
      this._ossimPlanetGroupBox.Location = new System.Drawing.Point(9, 3);
      this._ossimPlanetGroupBox.Name = "_ossimPlanetGroupBox";
      this._ossimPlanetGroupBox.Size = new System.Drawing.Size(440, 221);
      this._ossimPlanetGroupBox.TabIndex = 1;
      this._ossimPlanetGroupBox.TabStop = false;
      this._ossimPlanetGroupBox.Text = "OssimPlanet";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 118);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(133, 13);
      this.label1.TabIndex = 11;
      this.label1.Text = "Elevation Cache Directory:";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(236, 69);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(86, 13);
      this.label4.TabIndex = 10;
      this.label4.Text = "Max Level Detail";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(236, 49);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(105, 13);
      this.label3.TabIndex = 9;
      this.label3.Text = "Elevation Patch Size";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(236, 26);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(97, 13);
      this.label2.TabIndex = 8;
      this.label2.Text = "Height Exageration";
      // 
      // _elevationPatch
      // 
      this._elevationPatch.Location = new System.Drawing.Point(351, 42);
      this._elevationPatch.Name = "_elevationPatch";
      this._elevationPatch.Size = new System.Drawing.Size(51, 20);
      this._elevationPatch.TabIndex = 7;
      // 
      // _maxLevelDetail
      // 
      this._maxLevelDetail.Location = new System.Drawing.Point(351, 65);
      this._maxLevelDetail.Name = "_maxLevelDetail";
      this._maxLevelDetail.Size = new System.Drawing.Size(51, 20);
      this._maxLevelDetail.TabIndex = 6;
      // 
      // _heightExag
      // 
      this._heightExag.DecimalPlaces = 1;
      this._heightExag.Location = new System.Drawing.Point(351, 19);
      this._heightExag.Name = "_heightExag";
      this._heightExag.Size = new System.Drawing.Size(51, 20);
      this._heightExag.TabIndex = 5;
      // 
      // _browse
      // 
      this._browse.Location = new System.Drawing.Point(347, 140);
      this._browse.Name = "_browse";
      this._browse.Size = new System.Drawing.Size(75, 23);
      this._browse.TabIndex = 4;
      this._browse.Text = "Browse...";
      this._browse.UseVisualStyleBackColor = true;
      // 
      // _cacheDir
      // 
      this._cacheDir.Location = new System.Drawing.Point(13, 143);
      this._cacheDir.Name = "_cacheDir";
      this._cacheDir.Size = new System.Drawing.Size(328, 20);
      this._cacheDir.TabIndex = 3;
      // 
      // _hud
      // 
      this._hud.AutoSize = true;
      this._hud.Location = new System.Drawing.Point(16, 65);
      this._hud.Name = "_hud";
      this._hud.Size = new System.Drawing.Size(50, 17);
      this._hud.TabIndex = 2;
      this._hud.Text = "HUD";
      this._hud.UseVisualStyleBackColor = true;
      // 
      // _ephemeris
      // 
      this._ephemeris.AutoSize = true;
      this._ephemeris.Location = new System.Drawing.Point(16, 42);
      this._ephemeris.Name = "_ephemeris";
      this._ephemeris.Size = new System.Drawing.Size(75, 17);
      this._ephemeris.TabIndex = 1;
      this._ephemeris.Text = "Ephemeris";
      this._ephemeris.UseVisualStyleBackColor = true;
      // 
      // _elevation
      // 
      this._elevation.AutoSize = true;
      this._elevation.Location = new System.Drawing.Point(16, 19);
      this._elevation.Name = "_elevation";
      this._elevation.Size = new System.Drawing.Size(70, 17);
      this._elevation.TabIndex = 0;
      this._elevation.Text = "Elevation";
      this._elevation.UseVisualStyleBackColor = true;
      // 
      // _latLongGrid
      // 
      this._latLongGrid.AutoSize = true;
      this._latLongGrid.Location = new System.Drawing.Point(16, 89);
      this._latLongGrid.Name = "_latLongGrid";
      this._latLongGrid.Size = new System.Drawing.Size(90, 17);
      this._latLongGrid.TabIndex = 12;
      this._latLongGrid.Text = "Lat Long Grid";
      this._latLongGrid.UseVisualStyleBackColor = true;
      // 
      // PageContent
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.flowLayoutPanel1);
      this.Name = "PageContent";
      this.Size = new System.Drawing.Size(452, 265);
      this.flowLayoutPanel1.ResumeLayout(false);
      this._ossimPlanetGroupBox.ResumeLayout(false);
      this._ossimPlanetGroupBox.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this._elevationPatch)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this._maxLevelDetail)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this._heightExag)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox _ossimPlanetGroupBox;
    private System.Windows.Forms.CheckBox _hud;
    private System.Windows.Forms.CheckBox _ephemeris;
    private System.Windows.Forms.CheckBox _elevation;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.NumericUpDown _elevationPatch;
    private System.Windows.Forms.NumericUpDown _maxLevelDetail;
    private System.Windows.Forms.NumericUpDown _heightExag;
    private System.Windows.Forms.Button _browse;
    private System.Windows.Forms.TextBox _cacheDir;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.CheckBox _latLongGrid;
  }
}
