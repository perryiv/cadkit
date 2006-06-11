
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace OpenGL
  {
    partial class DataGridForm : CadKit.Persistence.Form
    {
      /// <summary>
      /// Required designer variable.
      /// </summary>
      private System.ComponentModel.IContainer components = null;

      /// <summary>
      /// Clean up any resources being used.
      /// </summary>
      /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
      protected override void Dispose( bool disposing )
      {
        if ( disposing && ( components != null ) )
        {
          components.Dispose();
        }
        base.Dispose( disposing );
      }

      #region Windows Form Designer generated code

      /// <summary>
      /// Required method for Designer support - do not modify
      /// the contents of this method with the code editor.
      /// </summary>
      private void InitializeComponent()
      {
        System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
        System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
        this._dataGrid = new System.Windows.Forms.DataGridView();
        ( ( System.ComponentModel.ISupportInitialize ) ( this._dataGrid ) ).BeginInit();
        this.SuspendLayout();
        // 
        // _dataGrid
        // 
        this._dataGrid.AllowUserToAddRows = false;
        this._dataGrid.AllowUserToDeleteRows = false;
        this._dataGrid.AllowUserToOrderColumns = true;
        this._dataGrid.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
        this._dataGrid.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
        dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
        dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
        dataGridViewCellStyle1.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 0 ) ) );
        dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
        dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
        dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
        dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
        this._dataGrid.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
        this._dataGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
        dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
        dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
        dataGridViewCellStyle2.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 0 ) ) );
        dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
        dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
        dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
        dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
        this._dataGrid.DefaultCellStyle = dataGridViewCellStyle2;
        this._dataGrid.Dock = System.Windows.Forms.DockStyle.Fill;
        this._dataGrid.Location = new System.Drawing.Point( 0, 0 );
        this._dataGrid.Name = "_dataGrid";
        this._dataGrid.ReadOnly = true;
        this._dataGrid.Size = new System.Drawing.Size( 292, 266 );
        this._dataGrid.TabIndex = 0;
        // 
        // DataGridForm
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size( 292, 266 );
        this.Controls.Add( this._dataGrid );
        this.Name = "DataGridForm";
        this.Text = "DataGrid";
        ( ( System.ComponentModel.ISupportInitialize ) ( this._dataGrid ) ).EndInit();
        this.ResumeLayout( false );

      }

      #endregion

      private System.Windows.Forms.DataGridView _dataGrid;

    }
  }
}
