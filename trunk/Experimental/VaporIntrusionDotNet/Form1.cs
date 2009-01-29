using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;


namespace VaporIntrusionDotNet
{
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();

      // Initialize the component manager
      ComponentManager.initialize();

      // initialize the grid space
      GridSpace.initialize();

      // get the dimensions of the main form window
      int width = this.Width;
      int height = this.Height;

      // Create the 2D draw windows
      XYWindow xy = new XYWindow();

      // Add the 2D draw windows to the main form
      xy.MdiParent = this;

      // set the position of the xy 2D draw window
      xy.Left = 0; xy.Width  = width  / 2;
      xy.Top = 0;  xy.Height = height / 2;

      // resize the canvas
      xy.initialize();

      // Show the forms
      xy.Show();
    }

    private void Form1_Load(object sender, EventArgs e)
    {

    }

    private void Form1_Load_1(object sender, EventArgs e)
    {

    }

    private void MenuBar_ItemClicked_1(object sender, ToolStripItemClickedEventArgs e)
    {

    }
  }
}