
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Allows rotation of column names.
// See http://unboxedsolutions.com/sean/examples/CustomDataGrid.cs.txt
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace OpenGL
  {
    public partial class DataGridView : System.Windows.Forms.DataGridView
    {
      private double _rotationAngle;

      public DataGridView()
      {
        _rotationAngle = 0d;
      }

      [System.ComponentModel.Description( "Rotation Angle" ), System.ComponentModel.Category( "Appearance" )]
      public double RotationAngle
      {
        get { return _rotationAngle; }
        set
        {
          _rotationAngle = value;
          this.Invalidate();
        }
      }

      protected override void OnCellPainting( System.Windows.Forms.DataGridViewCellPaintingEventArgs e )
      {
        if ( e.ColumnIndex >= 0 && e.RowIndex == -1 )
        {
          string text = this.Columns[e.ColumnIndex].HeaderText;

          //Getting the width and height of the text, which we are going to write
          double columnTextWidth = e.Graphics.MeasureString( text, e.CellStyle.Font ).Width;
          double columnTextHeight = e.Graphics.MeasureString( text, e.CellStyle.Font ).Height;

          // Erase the cell.
          using ( System.Drawing.Brush backColorBrush = new System.Drawing.SolidBrush( e.CellStyle.BackColor ) )
          {
            e.Graphics.FillRectangle( backColorBrush, e.CellBounds );
          }

          // only need 1 bottom line...
          e.Graphics.DrawLine( System.Drawing.Pens.DarkGray, e.CellBounds.Left, e.CellBounds.Bottom - 1, e.CellBounds.Right, e.CellBounds.Bottom - 1 );

          // two top lines...
          e.Graphics.DrawLine( System.Drawing.Pens.DarkGray, e.CellBounds.Left, e.CellBounds.Top, e.CellBounds.Right, e.CellBounds.Top );
          e.Graphics.DrawLine( System.Drawing.Pens.White, e.CellBounds.Left, e.CellBounds.Top + 1, e.CellBounds.Right, e.CellBounds.Top + 1 );

          // right line...
          e.Graphics.DrawLine( System.Drawing.Pens.DarkGray, e.CellBounds.Right - 1, e.CellBounds.Top, e.CellBounds.Right - 1, e.CellBounds.Bottom );

          // left line...
          e.Graphics.DrawLine( System.Drawing.Pens.White, e.CellBounds.Left, e.CellBounds.Top, e.CellBounds.Left, e.CellBounds.Bottom );

          //For rotation
          double angle = ( _rotationAngle / 180 ) * System.Math.PI;

          double hSin = columnTextHeight * System.Math.Sin( angle );
          double wCos = columnTextWidth * System.Math.Cos( angle );
          double hCos = columnTextHeight * System.Math.Cos( angle );
          double wSin = columnTextWidth * System.Math.Sin( angle );

          double rotatedWidth = hSin - wCos;
          double rotatedHeight = hCos - wSin;

          double dx = ( e.CellBounds.Width + hSin - wCos ) / 2;
          double dy = ( e.CellBounds.Height - hCos - wSin ) / 2;

          this.Columns[e.ColumnIndex].Width = ( int ) System.Math.Abs( rotatedWidth ) + 10;
          int newColHeight = ( int ) System.Math.Abs( rotatedHeight ) + 10;
          if ( this.ColumnHeadersHeight < newColHeight )
          {
            this.ColumnHeadersHeight = newColHeight;
          }

          System.Drawing.Drawing2D.Matrix mx = new System.Drawing.Drawing2D.Matrix();
          mx.Rotate( ( float ) _rotationAngle, System.Drawing.Drawing2D.MatrixOrder.Append );
          float heightOffset = e.CellBounds.Y + 10;
          mx.Translate( ( float ) ( dx + e.CellBounds.X ), heightOffset, System.Drawing.Drawing2D.MatrixOrder.Append );
          e.Graphics.Transform = mx;

          e.Graphics.DrawString( text, e.CellStyle.Font, System.Drawing.Brushes.Black, 0, 0, System.Drawing.StringFormat.GenericTypographic );

          e.Graphics.ResetTransform();
          e.Handled = true;
        }
        else
        {
          base.OnCellPainting( e );
        }
      }
    }
  }
}
