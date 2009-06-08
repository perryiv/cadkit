///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VaporIntrusionDotNet
{
  public partial class XYWindow : Form
  {
    //*************************************************************************************
    //
    // Structs
    //
    //*************************************************************************************

    public struct GridPoint
    {
      public Point point;
      public Color color;
    };

    //*************************************************************************************
    //
    // Member variables
    //
    //*************************************************************************************

    private List<GridPoint> _grid;
    private Helper.Vector2 _gridSize;
    private Rectangle _selectionRect;
    private Color _currentPointColor;
    private bool _selecting;
    private System.Drawing.Brush _brush;
    private int _currentGridDepth;

    //*************************************************************************************
    //
    // Constructor
    //
    //*************************************************************************************
    
    public XYWindow()
    {
      InitializeComponent();

      // initialize the brush
      _brush = new SolidBrush(Color.FromArgb(128,Color.Gray));

      // grid resolution
      _gridSize.x = 100;
      _gridSize.y = 100;

      _selecting = false;

      // initialize the grid
      _grid = new List<GridPoint>(_gridSize.x * _gridSize.y);

      // Initialize the current point color
      _currentPointColor = Color.Red;

      // Set the current depth of the grid
      _currentGridDepth = 0;

    }


    //*************************************************************************************
    //
    // Initialize
    //
    //*************************************************************************************
    
    public void initialize()
    {
      // resize the grid
      _resizeGrid();
    }


    //*************************************************************************************
    //
    // Handle mouse down events
    //
    //*************************************************************************************
    
    protected override void OnMouseDown(System.Windows.Forms.MouseEventArgs e)
    {
      Point mousePt = new Point(e.X, e.Y);

      switch (e.Button)
      {
        case (MouseButtons.Left):
          {

            Size size = new Size(0, 0);
            _selectionRect = new Rectangle(mousePt, size);
            _currentPointColor = Color.Blue;
            _selecting = true;
            // initialize the brush
            _brush = new SolidBrush(Color.FromArgb(128, Color.Blue));

            break;
          }

        case (MouseButtons.Right):
          {
            Size size = new Size(0, 0);
            _selectionRect = new Rectangle(mousePt, size);
            _currentPointColor = Color.Red;
            _selecting = true;
            // initialize the brush
            _brush = new SolidBrush(Color.FromArgb(128, Color.Red));
            break;
          }

      }

      Invalidate();
    }




    //*************************************************************************************
    //
    // Handle mouse up events
    //
    //*************************************************************************************

    protected override void OnMouseUp(System.Windows.Forms.MouseEventArgs e)
    {
      Point mousePt = new Point(e.X, e.Y);

      if (true == _selecting)
      {
        // get the original point
        Point origin = _selectionRect.Location;
        Size size = new Size( e.X - origin.X, e.Y - origin.Y );

        // make a new rect with the final information
        _selectionRect = new Rectangle(origin, size);

        // color the grid
        this._colorGrid();

        // clear out the rectangle
        _selectionRect = new Rectangle();

        // we are no longer selecting
        _selecting = false;
      }

      Invalidate();
    }



    //*************************************************************************************
    //
    // Handle mouse move events
    //
    //*************************************************************************************

    protected override void OnMouseMove(System.Windows.Forms.MouseEventArgs e)
    {
      if ((e.Button == MouseButtons.Left || e.Button == MouseButtons.Right) && true == _selecting)
      {
        // get the original point
        Point origin = _selectionRect.Location;
        Size size = new Size(e.X - origin.X, e.Y - origin.Y);

        // make a new rect with the final information
        _selectionRect = new Rectangle(origin, size);

        // Invalidate
        this.Invalidate();

      }
    }


    //*************************************************************************************
    //
    // Load
    //
    //*************************************************************************************
    
    private void XYWindow_Load(object sender, EventArgs e)
    {

    }


    //*************************************************************************************
    //
    // resize the grid
    //
    //*************************************************************************************

    private void _colorGrid()
    {
      // get the current size of the screen
      int width = this.Width;
      int height = this.Height;

      // find out how much space between rects
      int widthStep = width / _gridSize.x;
      int heightStep = height / _gridSize.y;

      // bitmap to hold the points
      Bitmap b = new Bitmap(width, height);

      // initialize the points
      for (int i = 0; i < _grid.Count; ++i)
      {
        Color color = _grid[i].color;

        if (_selectionRect.Contains(_grid[i].point))
        {
          // set the current color
          color = this._currentPointColor;

          // y value for this point
          int y = 0;

          // x value for this point
          int x = Math.DivRem( i, _gridSize.x, out y);

          // add the current component to the grid point
          GridSpace.setComponentAt( x, y, _currentGridDepth, ComponentManager.currentComponent() );
        }
        
        // Make a temp point
        Point p = _grid[i].point;

        // set the color of p          
        b.SetPixel(p.X, p.Y, color);
        b.SetPixel(p.X + 1, p.Y, color);
        b.SetPixel(p.X, p.Y + 1, color);
        b.SetPixel(p.X + 1, p.Y + 1, color);

        // Make a component to store the point
        GridPoint c = new GridPoint();

        c.point = p;
        c.color = color;

        // set the grid point
        _grid[i] = c;

        
        
        
        

      }

      // set the image on the canvas
      //_gridCanvas.Image = b;
      this.BackgroundImage = b;

    }


    //*************************************************************************************
    //
    // resize the grid
    //
    //*************************************************************************************

    private void _resizeGrid()
    {
      // get the current size of the screen
      int width = this.Width;
      int height = this.Height;    

      // find out how much space between rects
      int widthStep = width / _gridSize.x;
      int heightStep = height / _gridSize.y;

      // bitmap to hold the points
      Bitmap b = new Bitmap(width, height);

      // initialize the points
      for (int i = 0; i < _gridSize.x; ++i)
      {
        for (int j = 0; j < _gridSize.y; ++j)
        {
          // get the sub dimensions of this triangle
          int w0 = i * widthStep;
          int h0 = j * heightStep;
          int w1 = (i + 1) * widthStep;
          int h1 = (j + 1) * heightStep;

          // make a temp point
          Point p = new Point((w0 + (w1 - w0) / 2), (h0 + (h1 - h0) / 2));

          // set the color of p          
          b.SetPixel(p.X    , p.Y    , Color.Red);
          b.SetPixel(p.X + 1, p.Y    , Color.Red);
          b.SetPixel(p.X    , p.Y + 1, Color.Red);
          b.SetPixel(p.X + 1, p.Y + 1, Color.Red);

          // Make a component to store the point
          GridPoint c = new GridPoint();

          c.point = p;
          c.color = Color.Red;


          // set the grid point
          _grid.Add(c);

        }
      }

      // set the image on the canvas
      this.BackgroundImage = b;
    }


    //*************************************************************************************
    //
    // Get/Set the grid size
    //
    //*************************************************************************************

    public Helper.Vector2 gridSize
    {
      get
      {
        return _gridSize;
      }
      set
      {
        // Set the dimensions
        _gridSize = value;

        // (re)initialize the grid
        _grid = new List<GridPoint>(_gridSize.x * _gridSize.y);

      }
    }


    //*************************************************************************************
    //
    // 
    //
    //*************************************************************************************

    private void _gridCanvas_Click(object sender, EventArgs e)
    {

    }

    //*************************************************************************************
    //
    // Paint the canvas
    //
    //*************************************************************************************

    private void _gridCanvas_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
    {
      if (true == _selecting)
      {
        e.Graphics.FillRectangle(_brush, _selectionRect);
      }
    }

    //*************************************************************************************
    //
    // Paint
    //
    //*************************************************************************************

    protected override void  OnPaint(PaintEventArgs e)
    {
      if (true == _selecting)
      {
        e.Graphics.FillRectangle(_brush, _selectionRect);
      }

 	     //base.OnPaint(e);
    }
    

  }
}