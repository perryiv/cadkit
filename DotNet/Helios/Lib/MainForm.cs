
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
    /// Data members.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm( string persistentName )
    {
      this.PersistentName = persistentName;
      this.InitializeComponent();
      this.Load += this._formLoad;
    }

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
    protected void _loadPlugins ( object caller )
    {
      try
      {
        string dir = CadKit.Helios.Lib.Application.instance().directory();
        string name = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
        string path = dir + '/' + name + ".xml";
        CadKit.Plugins.Manager.instance().load(path, caller);
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 3359202912: {0}", e.Message );
      }
    }

    /// <summary>
    /// Delegates used in this class.
    /// </summary>
    public delegate void SplashDelegate(object caller);

    /// <summary>
    /// Called when the form is shown.
    /// </summary>
    private void _formLoad(object sender, System.EventArgs args )
    {
      try
      {
        this._showSplashScreen(this._loadPlugins);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1031380962: {0}", e.Message);
      }
    }

    /// <summary>
    /// Sleep for the duration as long as it is positive.
    /// </summary>
    private void _safeSleep(System.TimeSpan duration)
    {
      if ( duration.Ticks > 0 )
        System.Threading.Thread.Sleep(duration);
    }

    /// <summary>
    /// Build and show the splash screen.
    /// </summary>
    private void _showSplashScreen ( SplashDelegate del )
    {
      CadKit.Helios.Lib.SplashScreen splash = null;
      try
      {
        System.DateTime start = System.DateTime.Now;
        System.TimeSpan duration = new System.TimeSpan(0, 0, 5);
        splash = this._buildSplashScreen();
        splash.Show();
        splash.Update();
        del ( splash );
        this._safeSleep(duration - (System.DateTime.Now - start));
      }
      finally
      {
        if (null != splash)
          splash.Close();
      }
    }

    /// <summary>
    /// Build the splash screen.
    /// </summary>
    private CadKit.Helios.Lib.SplashScreen _buildSplashScreen()
    {
      CadKit.Helios.Lib.SplashScreen splash = new CadKit.Helios.Lib.SplashScreen();
      string file = CadKit.Helios.Lib.Application.instance().directory() + "/icons/splash_screen.jpg";
      splash.Image = CadKit.Images.Image.load(file);
      splash.Label.Text = "Hi Perry";
      splash.Text = CadKit.Helios.Lib.Application.instance().Name;
      return splash;
    }
  }
}
