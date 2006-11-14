
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
    CadKit.Interfaces.IMainForm,
    CadKit.Interfaces.IDockPanel,
    CadKit.Interfaces.IPersistantFormData,
    CadKit.Interfaces.IRecentFileList,
    CadKit.Interfaces.IWindowMenu
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private class FormsMap : System.Collections.Generic.Dictionary<string, System.Windows.Forms.Form> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private System.ComponentModel.IContainer components = null;
    private WeifenLuo.WinFormsUI.DockPanel _dockPanel = null;
    private WeifenLuo.WinFormsUI.DeserializeDockContent _deserializeDockContent;
    private FormsMap _persistantForms = new FormsMap();
    private FormsMap _windowForms = new FormsMap();
    private System.Windows.Forms.ToolStripMenuItem _windowMenu = null;
    private CadKit.Helios.RecentFiles _recentFiles = new CadKit.Helios.RecentFiles();

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm(string persistentName)
    {
      _deserializeDockContent = new WeifenLuo.WinFormsUI.DeserializeDockContent(this._getContentFromPersistString);

      this.PersistentName = persistentName;
      this.InitializeComponent();
      this.IsMdiContainer = true;

      if (CadKit.Persistence.Registry.Instance.getBool("CadKit.Options.General", "use_system_mdi_child_windows", true))
        _dockPanel.DocumentStyle = WeifenLuo.WinFormsUI.DocumentStyles.SystemMdi;
      else if (CadKit.Persistence.Registry.Instance.getBool("CadKit.Options.General", "use_tabbed_mdi_child_windows", true))
        _dockPanel.DocumentStyle = WeifenLuo.WinFormsUI.DocumentStyles.DockingMdi; // DockingSdi or DockingWindow makes it assert in CadKit.Referenced.Base on exit.

      this.Text = CadKit.Helios.Application.Instance.Name;
      this._buildMenu();
      this.Load += this._formLoad;
      this.FormClosed += this._formClosed;
    }


    /// <summary>
    /// Return the docking content associated with the string.
    /// </summary>
    private WeifenLuo.WinFormsUI.IDockContent _getContentFromPersistString(string persistString)
    {
      if (_persistantForms.ContainsKey(persistString))
        return _persistantForms[persistString] as WeifenLuo.WinFormsUI.IDockContent;

      return null;
    }


    /// <summary>
    /// The form is closed.  Deserialize dock content.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _formClosed(object sender, System.Windows.Forms.FormClosedEventArgs e)
    {
      _dockPanel.SaveAsXml(this.DockSettingsFile);
    }

    /// <summary>
    /// Get the file where dock settings reside.
    /// </summary>
    private string DockSettingsFile
    {
      get
      {
        string program = System.Reflection.Assembly.GetEntryAssembly().GetName().Name; // Handles ".vshost"
        return System.Windows.Forms.Application.UserAppDataPath + "\\" + program + "_dock.xml";
      }
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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
      this._dockPanel = new WeifenLuo.WinFormsUI.DockPanel();
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
      // 
      // _dockPanel
      // 
      this._dockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._dockPanel.Name = "_dockPanel";
      this._dockPanel.TabIndex = 0;
      this.Controls.Add(this._dockPanel);

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

          this.Shown += this._formShown;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1031380962: {0}", e.Message);
      }
    }


    /// <summary>
    /// The form is shown.  Load any serialized dock content.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void _formShown(object sender, System.EventArgs args)
    {
      lock (_mutex)
      {
        try
        {
          string configFile = this.DockSettingsFile;
          if (System.IO.File.Exists(configFile))
            _dockPanel.LoadFromXml(configFile, _deserializeDockContent);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1948208679: {0}", e.Message);
        }
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
          menu.DropDownItems.Add(_recentFiles.Menu);
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
        {
          _windowMenu = CadKit.Tools.Menu.makeMenu("&Windows");
          this.MainMenuStrip.Items.Add(_windowMenu);
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
    /// Return the parent form for child views.
    /// </summary>
    object CadKit.Interfaces.IDockPanel.DockPanel
    {
      get { lock (_mutex) { return this._dockPanel; } }
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

    /// <summary>
    /// Register a peristant form.
    /// </summary>
    void CadKit.Interfaces.IPersistantFormData.registerPersistanceForm(string name, System.Windows.Forms.Form form)
    {
      _persistantForms.Add(name, form);
    }

    /// <summary>
    /// See if there is peristant form data.
    /// </summary>
    bool CadKit.Interfaces.IPersistantFormData.hasPersistantFormData()
    {
      return System.IO.File.Exists(this.DockSettingsFile);
    }

    /// <summary>
    /// Add the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.add(string name)
    {
      if (null != _recentFiles)
        _recentFiles.add(name);
    }

    /// <summary>
    /// Remove the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.remove(string name)
    {
      if (null != _recentFiles)
        _recentFiles.remove(name);
    }

    /// <summary>
    /// Clear the list.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.clear()
    {
      if (null != _recentFiles)
        _recentFiles.clear();
    }


    /// <summary>
    /// Add a form with a string for a key.
    /// </summary>
    public void addFormWindowMenu(string name, System.Windows.Forms.Form form)
    {
      try
      {
        _windowForms[name] = form;

        if (null != _windowMenu)
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem(name);
          item.Click += new System.EventHandler(_onWindowItemClick);
          _windowMenu.DropDownItems.Add(item);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3038971820: Exception caught while trying to add to Window menu.");
        System.Console.WriteLine("Message: {0}", e.Message);
      }
    }


    /// <summary>
    /// Show a window.
    /// </summary>
    void _onWindowItemClick(object sender, System.EventArgs e)
    {
      try
      {
        System.Windows.Forms.ToolStripMenuItem item = sender as System.Windows.Forms.ToolStripMenuItem;

        if (null != item)
        {
          System.Windows.Forms.Form form = _windowForms[item.Text];
          WeifenLuo.WinFormsUI.DockContent dock = form as WeifenLuo.WinFormsUI.DockContent;
          if (null != dock)
          {
            dock.Show(_dockPanel);
          }
          else
          {
            form.Show();
          }
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 3223971820: Exception caught while trying to show window.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }
  }
}
