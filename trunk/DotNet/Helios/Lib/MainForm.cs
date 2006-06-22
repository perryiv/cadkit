
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Lib
{
  public partial class MainForm : CadKit.Persistence.Form
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm( string persistentName )
    {
      this.PersistentName = persistentName;
      this.InitializeComponent();
      this._loadPlugins();
    }

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
        this.SuspendLayout();
        // 
        // MainForm
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(757, 367);
        this.Name = "MainForm";
        this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
        this.Text = "MainForm";
        this.ResumeLayout(false);

    }

    #endregion

    /// <summary>
    /// Load all appropriate plugins.
    /// </summary>
    protected void _loadPlugins()
    {
      try
      {
        string dir = CadKit.Helios.Lib.Application.instance().directory();
        string name = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
        string path = dir + '/' + name + ".xml";
        CadKit.Plugins.Manager.instance().load( path );
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 3359202912: {0}", e.Message );
      }
    }
  }
}
