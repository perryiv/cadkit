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
    // Member variables
    //
    //*************************************************************************************

    private List<Component> _grid;
    private Helper.Vector2 _gridSize;
    private Rectangle _selectionRect;
    private Color _currentPointColor;
    private bool _selecting;

    //*************************************************************************************
    //
    // Constructor
    //
    //*************************************************************************************
    
    public XYWindow()
    {
      InitializeComponent();

      // grid resolution
      _gridSize.x = 5;
      _gridSize.y = 3;

      _selecting = false;

      // hide the picture box
      _gridCanvas.Hide();

      // initialize the grid
      _grid = new List<Component>(_gridSize.x * _gridSize.y);

      // Initialize the current point color
      _currentPointColor = Color.Red;

      // resize the canvas
      resizeCanvas();
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

            break;
          }

        case (MouseButtons.Right):
          {
            Size size = new Size(0, 0);
            _selectionRect = new Rectangle(mousePt, size);
            _currentPointColor = Color.Red;
            _selecting = true;
            break;
          }

      }

      //if (rectA.Contains(mousePt))
      //{
      //  isImageClicked = true;
      //  imageClicked = 0;
      //  this.Text = "You clicked image A";
      //}

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


      //if (rectA.Contains(mousePt))
      //{
      //  isImageClicked = true;
      //  imageClicked = 0;
      //  this.Text = "You clicked image A";
      //}

      //_selecting = false;

      Invalidate();
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
    // resize grid canvas
    //
    //*************************************************************************************

    public void resizeCanvas()
    {
      _gridCanvas.Left = (int)((float)this.Width * 0.0);
      _gridCanvas.Top = (int)((float)this.Height * 0.0);

      _gridCanvas.Width = (int)((float)this.Width * 1.0);
      _gridCanvas.Height = (int)((float)this.Height * 1.0);
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
      int height = (int)((float)this.Height * 0.75);

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
          color = this._currentPointColor;
        }
        
        // Make a temp point
        Point p = _grid[i].point;

        // set the color of p          
        b.SetPixel(p.X, p.Y, color);
        b.SetPixel(p.X + 1, p.Y, color);
        b.SetPixel(p.X, p.Y + 1, color);
        b.SetPixel(p.X + 1, p.Y + 1, color);

        // Make a component to store the point
        Component c = new Component();

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
      int height = (int)((float)this.Height * 0.75);      

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
          Component c = new Component();

          c.point = p;
          c.color = Color.Red;


          // set the grid point
          _grid.Add(c);

        }
      }

      // set the image on the canvas
      //_gridCanvas.Image = b;      
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
        _grid = new List<Component>(_gridSize.x * _gridSize.y);

      }
    } 
  }
}