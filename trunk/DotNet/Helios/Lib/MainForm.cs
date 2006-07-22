
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public partial class MainForm :
    CadKit.Persistence.Form,
    CadKit.Interfaces.IMenuBar,
    CadKit.Interfaces.IMainForm
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm(string persistentName)
    {
      this.PersistentName = persistentName;
      this.InitializeComponent();
      this.IsMdiContainer = CadKit.Persistence.Registry.Instance.getBool("CadKit.Options.General", "use_mdi_child_windows", false);
      this.Text = CadKit.Helios.Application.Instance.Name;
      this._buildMenu();
      this.Load += this._formLoad;
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      lock (_mutex)
      {
        if (disposing && (components != null))
        {
          components.Dispose();
        }
        base.Dispose(disposing);
      }
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
      this.SuspendLayout();
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(757, 367);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "MainForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "MainForm";
      this.ResumeLayout(false);

    }

    #endregion

    /// <summary>
    /// Get the menu bar.
    /// </summary>
    object CadKit.Interfaces.IMenuBar.MenuBar
    {
      get
      {
        lock (_mutex)
        {
          if (null == this.MainMenuStrip)
          {
            System.Windows.Forms.MenuStrip menu = new System.Windows.Forms.MenuStrip();
            this.Controls.Add(menu);
            this.MainMenuStrip = menu;
          }
          return this.MainMenuStrip;
        }
      }
    }

    /// <summary>
    /// Load all appropriate plugins.
    /// </summary>
    protected void _loadPlugins(object caller)
    {
      try
      {
        lock (_mutex)
        {
          string dir = CadKit.Helios.Application.Instance.Directory;
          string name = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
          string path = dir + '/' + name + ".xml";
          CadKit.Plugins.Manager.Instance.load(path, caller);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3359202912: {0}", e.Message);
      }
    }

    /// <summary>
    /// Delegates used in this class.
    /// </summary>
    public delegate void SplashDelegate(object caller);

    /// <summary>
    /// Called when the form is shown.
    /// </summary>
    private void _formLoad(object sender, System.EventArgs args)
    {
      try
      {
        lock (_mutex)
        {
          this._showSplashScreen(this._loadPlugins);
          this._reportPluginInfo();
        }
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
      lock (_mutex)
      {
        if (duration.Ticks > 0)
          System.Threading.Thread.Sleep(duration);
      }
    }

    /// <summary>
    /// Build and show the splash screen.
    /// </summary>
    private void _showSplashScreen(SplashDelegate del)
    {
      lock (_mutex)
      {
        CadKit.Helios.SplashScreen splash = null;
        try
        {
          System.DateTime start = System.DateTime.Now;
          System.TimeSpan duration = new System.TimeSpan(0, 0, 1);
          splash = this._buildSplashScreen();
          if (null != splash)
          {
            splash.Show();
            splash.Update();
          }
          del((null != splash) ? (object)splash : (object)this);
          this._safeSleep(duration - (System.DateTime.Now - start));
        }
        finally
        {
          if (null != splash)
            splash.Close();
        }
      }
    }

    /// <summary>
    /// Build the splash screen.
    /// </summary>
    private CadKit.Helios.SplashScreen _buildSplashScreen()
    {
      lock (_mutex)
      {
        try
        {
          CadKit.Helios.SplashScreen splash = new CadKit.Helios.SplashScreen(this);
          string file = CadKit.Helios.Application.Instance.IconDir + "/splash_screen.jpg";
          splash.Image = CadKit.Images.Image.load(file);
          splash.Text = CadKit.Helios.Application.Instance.Name + " Startup";
          splash.Label.Text = splash.Text;
          return splash;
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 3966419022: {0}", e.Message);
          return null;
        }
      }
    }

    /// <summary>
    /// Build the menu.
    /// </summary>
    private void _buildMenu()
    {
      lock (_mutex)
      {
        object temp = (this as CadKit.Interfaces.IMenuBar).MenuBar;
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&File");
          this.MainMenuStrip.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.NewDocumentCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.OpenDocumentCommand(this));
          menu.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
          this._addMenuButton(menu, new CadKit.Helios.Commands.ExitCommand(this));
        }
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&Edit");
          this.MainMenuStrip.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.UndoCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.RedoCommand(this));
        }
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&Tools");
          this.MainMenuStrip.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.OptionsCommand(this));
        }
      }
    }

    /// <summary>
    /// Build the menu.
    /// </summary>
    private void _addMenuButton(System.Windows.Forms.ToolStripMenuItem menu, CadKit.Commands.Command command)
    {
      lock (_mutex)
      {
        if (null != menu && null != command)
          menu.DropDownItems.Add(command.MenuButton);
      }
    }

    /// <summary>
    /// Return the main window.
    /// </summary>
    object CadKit.Interfaces.IMainForm.Form
    {
      get { lock (_mutex) { return this; } }
    }

    /// <summary>
    /// Report the plugin info.
    /// </summary>
    private void _reportPluginInfo()
    {
      lock (_mutex)
      {
        string[] names = CadKit.Plugins.Manager.Instance.PluginNames;
        System.Array.Sort(names);
        if (null != names && names.Length > 0)
        {
          System.Console.Write("Found {0} {1}: ", names.Length, ((names.Length > 1) ? "plugins" : "plugin"));
          foreach (string name in names)
          {
            System.Console.Write("{0}; ", name);
          }
          System.Console.WriteLine();
          System.Console.Out.Flush();
        }
      }
    }
  }
}
