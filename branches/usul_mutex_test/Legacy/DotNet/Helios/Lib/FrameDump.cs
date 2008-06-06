
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Lib
{
  public partial class FrameDump : System.Windows.Forms.Form
  {
    public FrameDump()
    {
      InitializeComponent();

      this.comboBox1.Items.Add(".jpg");
      this.comboBox1.Items.Add(".bmp");
      this.comboBox1.Items.Add(".gif");

      this.textBox2.Text = "filename";
    }

    public string Directory
    {
      get
      {
        return textBox1.Text;
      }
      set
      {
        textBox1.Text = value;
      }
    }

    public string Filename
    {
      get
      {
        return textBox2.Text;
      }
      set
      {
        textBox2.Text = value;
      }
    }

    public string Extension
    {
      get
      {
        return (string)this.comboBox1.SelectedItem;
      }
    }

    private void _browseClick(object sender, System.EventArgs e)
    {
      System.Windows.Forms.FolderBrowserDialog browser = new System.Windows.Forms.FolderBrowserDialog();
      browser.SelectedPath = this.Directory;
      
      if (browser.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        this.Directory = browser.SelectedPath;
      }
      
    }

    private void _okClick(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.Close();
    }

    private void _cancelClick(object sender, System.EventArgs e)
    {
      this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.Close();
    }
  }
}