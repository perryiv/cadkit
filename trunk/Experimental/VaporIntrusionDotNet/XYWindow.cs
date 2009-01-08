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
    // Constructor
    //
    //*************************************************************************************
    
    public XYWindow()
    {
      InitializeComponent();
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
            break;
          }

        case (MouseButtons.Right):
          {
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
    // Load
    //
    //*************************************************************************************
    
    private void XYWindow_Load(object sender, EventArgs e)
    {

    }
  }
}