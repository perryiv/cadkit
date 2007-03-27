using System;
using System.Collections.Generic;
using System.Text;

namespace DT.Minerva.Plugins.MoviePlayer
{
  class MoviePlayer : WeifenLuo.WinFormsUI.DockContent
  {
    private System.Windows.Forms.Button button1;
  
    private void InitializeComponent()
    {
      this.button1 = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(12, 3);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(96, 37);
      this.button1.TabIndex = 0;
      this.button1.Text = "Movie Mode On";
      this.button1.UseVisualStyleBackColor = true;
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // MoviePlayer
      // 
      this.ClientSize = new System.Drawing.Size(123, 44);
      this.Controls.Add(this.button1);
      this.Name = "MoviePlayer";
      this.ResumeLayout(false);

    }

    public MoviePlayer()
      : base()
    {
      InitializeComponent();

      this.DockableAreas =
        WeifenLuo.WinFormsUI.DockAreas.DockBottom |
        WeifenLuo.WinFormsUI.DockAreas.DockLeft |
        WeifenLuo.WinFormsUI.DockAreas.DockRight |
        WeifenLuo.WinFormsUI.DockAreas.DockTop |
        WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockBottom;
      this.HideOnClose = true;
    }

    /// <summary>
    ///  If the buttons get clicked then we need to do this.
    /// 1. Grab the handler to document ( C# ). 
    /// 2. Ask the document to initialize the ITool ( via Glue ). 
    /// 3. Once initialized set this tool as handleTool for the viewer ( via Glue ).     ///   
    /// 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void button1_Click(object sender, EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;      

      if (null != button)
      {
        if (button.Text == "Movie Mode On")
        {
          button.Text = "Movie Mode Off";
          CadKit.Interfaces.IMovieMode mm = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IMovieMode;
          if (null != mm)
          {
            mm.setMovieMode( true );
          }
        }
        else
        {
          button.Text = "Movie Mode On";
          //....
        }
      }
    }

    private void button2_Click(object sender, EventArgs e)
    {

    }
  }
}
