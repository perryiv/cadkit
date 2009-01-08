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

      // initialize the grid space
      GridSpace.initialize();

      // Create the 2D draw windows
      XYWindow xy = new XYWindow();

      // Add the 2D draw windows to the main form
      xy.MdiParent = this;

      // Show the forms
      xy.Show();
    }

    private void Form1_Load(object sender, EventArgs e)
    {

    }
  }
}