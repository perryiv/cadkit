using System;
using System.Collections.Generic;
using System.Text;

namespace DT.Minerva.Plugins.MoviePlayer
{
  class MoviePlayer : WeifenLuo.WinFormsUI.DockContent
  {
    private System.Windows.Forms.Button playButton;
    private System.Windows.Forms.Button restartButton;
    private System.Windows.Forms.Panel ActionPanel;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.Button button4;
    private System.Windows.Forms.Button button3;
    private System.Windows.Forms.Button movieModeButton;

    private void InitializeComponent()
    {
      this.movieModeButton = new System.Windows.Forms.Button();
      this.playButton = new System.Windows.Forms.Button();
      this.restartButton = new System.Windows.Forms.Button();
      this.ActionPanel = new System.Windows.Forms.Panel();
      this.button4 = new System.Windows.Forms.Button();
      this.button3 = new System.Windows.Forms.Button();
      this.button1 = new System.Windows.Forms.Button();
      this.button2 = new System.Windows.Forms.Button();
      this.ActionPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // movieModeButton
      // 
      this.movieModeButton.Location = new System.Drawing.Point(12, 12);
      this.movieModeButton.Name = "movieModeButton";
      this.movieModeButton.Size = new System.Drawing.Size(96, 21);
      this.movieModeButton.TabIndex = 0;
      this.movieModeButton.Text = "Movie Mode On";
      this.movieModeButton.UseVisualStyleBackColor = true;
      this.movieModeButton.Click += new System.EventHandler(this.movieModeButton_Click);
      // 
      // playButton
      // 
      this.playButton.Location = new System.Drawing.Point(14, 20);
      this.playButton.Name = "playButton";
      this.playButton.Size = new System.Drawing.Size(96, 31);
      this.playButton.TabIndex = 1;
      this.playButton.Text = "Play";
      this.playButton.UseVisualStyleBackColor = true;
      this.playButton.Click += new System.EventHandler(this.playButton_Click);
      // 
      // restartButton
      // 
      this.restartButton.Location = new System.Drawing.Point(116, 20);
      this.restartButton.Name = "restartButton";
      this.restartButton.Size = new System.Drawing.Size(96, 31);
      this.restartButton.TabIndex = 3;
      this.restartButton.Text = "Restart";
      this.restartButton.UseVisualStyleBackColor = true;
      this.restartButton.Click += new System.EventHandler(this.restartButton_Click);
      // 
      // ActionPanel
      // 
      this.ActionPanel.BackColor = System.Drawing.SystemColors.ControlDark;
      this.ActionPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.ActionPanel.Controls.Add(this.button4);
      this.ActionPanel.Controls.Add(this.button3);
      this.ActionPanel.Controls.Add(this.restartButton);
      this.ActionPanel.Controls.Add(this.playButton);
      this.ActionPanel.Location = new System.Drawing.Point(121, 23);
      this.ActionPanel.Name = "ActionPanel";
      this.ActionPanel.Size = new System.Drawing.Size(435, 75);
      this.ActionPanel.TabIndex = 4;
      // 
      // button4
      // 
      this.button4.Location = new System.Drawing.Point(320, 20);
      this.button4.Name = "button4";
      this.button4.Size = new System.Drawing.Size(96, 31);
      this.button4.TabIndex = 5;
      this.button4.UseVisualStyleBackColor = true;
      // 
      // button3
      // 
      this.button3.Location = new System.Drawing.Point(218, 20);
      this.button3.Name = "button3";
      this.button3.Size = new System.Drawing.Size(96, 31);
      this.button3.TabIndex = 4;
      this.button3.UseVisualStyleBackColor = true;
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(12, 39);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(96, 21);
      this.button1.TabIndex = 5;
      this.button1.UseVisualStyleBackColor = true;
      // 
      // button2
      // 
      this.button2.Location = new System.Drawing.Point(12, 70);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(96, 21);
      this.button2.TabIndex = 6;
      this.button2.UseVisualStyleBackColor = true;
      // 
      // MoviePlayer
      // 
      this.ClientSize = new System.Drawing.Size(574, 110);
      this.Controls.Add(this.button2);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.ActionPanel);
      this.Controls.Add(this.movieModeButton);
      this.Name = "MoviePlayer";
      this.Load += new System.EventHandler(this.MoviePlayer_Load);
      this.ActionPanel.ResumeLayout(false);
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
    private void movieModeButton_Click(object sender, EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button)
      {
        CadKit.Interfaces.IMovieMode mm = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IMovieMode;
        if (button.Text == "Movie Mode On")
        {
          if (null != mm)
          {
            mm.setMovieMode(true);
            button.Text = "Movie Mode Off";
            this.playButton.Text = "Pause";
          }
          else
          {
          }
        }
        else
        {
          if (null != mm)
          {
            mm.setMovieMode(false);
            button.Text = "Movie Mode On";            
          }
          else
          {
          }
        }
      }
    }

    private void restartButton_Click(object sender, EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button)
      {
        CadKit.Interfaces.IMovieMode mm = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IMovieMode;
        if (null != mm)
        {
          mm.restart();
        }
        else
        {
        }
      }           
    }

    private void playButton_Click(object sender, EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button)
      {
        CadKit.Interfaces.IMovieMode mm = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IMovieMode;
        if (mm.isPlaying())
        {
          if (null != mm)
          {
            mm.pause();
            button.Text = "Pause";
          }
          else
          {            
          }
        }
        else
        {
          if (null != mm)
          {
            mm.play();
            button.Text = "Play";
          }
        }
      }
    }

    private void MoviePlayer_Load(object sender, EventArgs e)
    {

    }
  }
}
