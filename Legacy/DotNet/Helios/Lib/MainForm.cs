
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
    private WeifenLuo.WinFormsUI.DeserializeDockContent _deserializeDockContent;
    private FormsMap _persistantForms = new FormsMap();
    private FormsMap _windowForms = new FormsMap();
    private System.Windows.Forms.ToolStripMenuItem _windowMenu = null;
    private System.Windows.Forms.MenuStrip _menuStrip = null;
    private System.Windows.Forms.StatusStrip _statusStrip = null;
    private System.Windows.Forms.ToolStrip _toolStrip = null;
    private WeifenLuo.WinFormsUI.DockPanel _dockPanel = null;
    private CadKit.Helios.RecentFiles _recentFiles = new CadKit.Helios.RecentFiles();


    /// <summary>
    /// Constructor.
    /// </summary>
    public MainForm(string persistentName)
    {
      _recentFiles.reference();
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
      this._buildToolBar();
      this.Load += this._formLoad;
      this.FormClosing += this._formClosing;
      this.FormClosed += this._formClosed;
      this.AllowDrop = true;
      this.DragEnter += this._dragEnter;
      this.DragDrop += this._dragDrop;
    }


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      // Release all the plugins.
      CadKit.Plugins.Manager.Instance.release(this);

      lock (_mutex)
      {
        if (disposing && (components != null))
        {
          components.Dispose();
          components = null;
        }
        base.Dispose(disposing);

        _deserializeDockContent = null;
        _persistantForms = null;
        _windowForms = null;
        _windowMenu = null;
        _menuStrip = null;
        _statusStrip = null;
        _toolStrip = null;
        _dockPanel = null;
        if (null != _recentFiles)
        {
          _recentFiles.dereference();
          _recentFiles = null;
        }
      }
    }


    /// <summary>
    /// Called when a drag-and-drop operation starts.
    /// See http://www.codeproject.com/csharp/dragdrop.asp
    /// </summary>
    private void _dragEnter(object sender, System.Windows.Forms.DragEventArgs args)
    {
      bool isFile = (null != args && null != args.Data && true == args.Data.GetDataPresent(System.Windows.Forms.DataFormats.FileDrop));
      args.Effect = (isFile) ? System.Windows.Forms.DragDropEffects.Copy : System.Windows.Forms.DragDropEffects.None;
    }


    /// <summary>
    /// Called when a drag-and-drop operation occurs.
    /// See http://www.codeproject.com/csharp/dragdrop.asp
    /// </summary>
    private void _dragDrop(object sender, System.Windows.Forms.DragEventArgs args)
    {
      try
      {
        CadKit.Interfaces.ICommand open = new CadKit.Helios.Commands.DragDropFilesCommand(args, this);
        open.execute();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1685020732: {0}", e.Message);
      }
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
    /// The form is closing. Close all documents and deserialize dock content.
    /// </summary>
    void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs args)
    {
      try
      {
        _dockPanel.SaveAsXml(this.DockSettingsFile);
        CadKit.Documents.Manager.Instance.closeAllDocuments();
        CadKit.Plugins.Manager.Instance.release(this);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 7161821420: {0}", e.Message);
      }
    }


    /// <summary>
    /// The form is closed.
    /// </summary>
    void _formClosed(object sender, System.Windows.Forms.FormClosedEventArgs args)
    {
      try
      {
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1370142206: {0}", e.Message);
      }
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

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
      this._menuStrip = new System.Windows.Forms.MenuStrip();
      this._statusStrip = new System.Windows.Forms.StatusStrip();
      this._toolStrip = new System.Windows.Forms.ToolStrip();
      this._dockPanel = new WeifenLuo.WinFormsUI.DockPanel();
      this.SuspendLayout();
      // 
      // _menuStrip
      // 
      this._menuStrip.Location = new System.Drawing.Point(0, 0);
      this._menuStrip.Name = "_menuStrip";
      this._menuStrip.Size = new System.Drawing.Size(568, 24);
      this._menuStrip.TabIndex = 0;
      this._menuStrip.Text = "_menuStrip";
      // 
      // _statusStrip
      // 
      this._statusStrip.Location = new System.Drawing.Point(0, 276);
      this._statusStrip.Name = "_statusStrip";
      this._statusStrip.Size = new System.Drawing.Size(568, 22);
      this._statusStrip.TabIndex = 3;
      this._statusStrip.Text = "_statusStrip";
      // 
      // _toolStrip
      // 
      this._toolStrip.Location = new System.Drawing.Point(0, 24);
      this._toolStrip.Name = "_toolStrip";
      this._toolStrip.Size = new System.Drawing.Size(568, 25);
      this._toolStrip.TabIndex = 4;
      this._toolStrip.Text = "_toolStrip";
      // 
      // _dockPanel
      // 
      this._dockPanel.ActiveAutoHideContent = null;
      this._dockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._dockPanel.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
      this._dockPanel.Location = new System.Drawing.Point(0, 49);
      this._dockPanel.Name = "_dockPanel";
      this._dockPanel.Size = new System.Drawing.Size(568, 227);
      this._dockPanel.TabIndex = 5;
      // 
      // MainForm
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(568, 298);
      this.Controls.Add(this._dockPanel);
      this.Controls.Add(this._toolStrip);
      this.Controls.Add(this._menuStrip);
      this.Controls.Add(this._statusStrip);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MainMenuStrip = this._menuStrip;
      this.Margin = new System.Windows.Forms.Padding(2);
      this.Name = "MainForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "MainForm";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    /// <summary>
    /// Get the menu bar.
    /// </summary>
    object CadKit.Interfaces.IMenuBar.MenuBar
    {
      get { lock (_mutex) { return this.MenuBar; } }
    }

    /// <summary>
    /// Get the menu bar.
    /// </summary>
    public System.Windows.Forms.MenuStrip MenuBar
    {
      get
      {
        lock (_mutex)
        {
          if (null == this.MainMenuStrip)
          {
            System.Windows.Forms.MenuStrip menu = new System.Windows.Forms.MenuStrip();
            menu.Dock = System.Windows.Forms.DockStyle.Top;
            this.Controls.Add(menu);
            this.MainMenuStrip = menu;
          }
          return this.MainMenuStrip;
        }
      }
    }

    public System.Windows.Forms.ToolStrip ToolStrip
    {
      get { lock (_mutex) { return _toolStrip; } }
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
          string xml = dir + "/../configs/" + name + ".xml";
          CadKit.Plugins.Manager.Instance.load(dir, xml, caller);
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
          string file = CadKit.Helios.Application.Instance.SplashImage;
          splash.Image = CadKit.Images.Image.load(file);
          splash.Text = CadKit.Helios.Application.Instance.Name + " Startup";
          splash.Label.Text = splash.Text;
          splash.Icon = this.Icon;
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
    protected virtual void _buildMenu()
    {
      lock (_mutex)
      {
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&File");
          this.MenuBar.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.NewDocumentCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.OpenDocumentCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.InsertFileCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.SaveDocumentCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.SaveAsDocumentCommand(this));
          menu.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
          {
            System.Windows.Forms.ToolStripMenuItem export = CadKit.Tools.Menu.makeMenu("&Export");
            menu.DropDownItems.Add(export);
            this._addMenuButton(export, new CadKit.Helios.Commands.ExportFileCommand(this));
            this._addMenuButton(export, new CadKit.Helios.Commands.ExportModelCommand(this));
            this._addMenuButton(export, new CadKit.Helios.Commands.ExportSceneCommand(this));
            this._addMenuButton(export, new CadKit.Helios.Commands.ExportImageCommand(this));
          }
          menu.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
          menu.DropDownItems.Add(_recentFiles.Menu);
          menu.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
          this._addMenuButton(menu, new CadKit.Helios.Commands.ExitCommand(this));
        }
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&Edit");
          this.MenuBar.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.UndoCommand(this));
          this._addMenuButton(menu, new CadKit.Helios.Commands.RedoCommand(this));
        }
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&View");
          this.MenuBar.Items.Add(menu);
          {
            System.Windows.Forms.ToolStripMenuItem camera = CadKit.Tools.Menu.makeMenu("&Camera");
            menu.DropDownItems.Add(camera);
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewFitCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewHomeCommand(this));
            camera.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewFrontCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewBackCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewLeftCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewRightCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewTopCommand(this));
            this._addMenuButton(camera, new CadKit.Helios.Commands.ViewBottomCommand(this));
          }
          {
            System.Windows.Forms.ToolStripMenuItem polygons = CadKit.Tools.Menu.makeMenu("&Polygons");
            menu.DropDownItems.Add(polygons);
            this._addMenuButton(polygons, new CadKit.Helios.Commands.PolygonsFillCommand(this));
            this._addMenuButton(polygons, new CadKit.Helios.Commands.PolygonsHiddenLinesCommand(this));
            this._addMenuButton(polygons, new CadKit.Helios.Commands.PolygonsLineCommand(this));
            this._addMenuButton(polygons, new CadKit.Helios.Commands.PolygonsPointsCommand(this));
          }
          {
            System.Windows.Forms.ToolStripMenuItem shading = CadKit.Tools.Menu.makeMenu("&Shading");
            menu.DropDownItems.Add(shading);
            this._addMenuButton(shading, new CadKit.Helios.Commands.ShadeModelSmoothCommand(this));
            this._addMenuButton(shading, new CadKit.Helios.Commands.ShadeModelFlatCommand(this));
          }
          {
            System.Windows.Forms.ToolStripMenuItem textures = CadKit.Tools.Menu.makeMenu("Texture &Mode");
            menu.DropDownItems.Add(textures);
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureMode1DCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureMode2DCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureMode3DCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureModeRectangleCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureModeCubeMapCommand(this));
            textures.DropDownItems.Add(new System.Windows.Forms.ToolStripSeparator());
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureModeAllCommand(this));
          }
          {
            System.Windows.Forms.ToolStripMenuItem textures = CadKit.Tools.Menu.makeMenu("Texture &Environment");
            menu.DropDownItems.Add(textures);
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureEnvironmentAddCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureEnvironmentBlendCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureEnvironmentDecalCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureEnvironmentModulateCommand(this));
            this._addMenuButton(textures, new CadKit.Helios.Commands.TextureEnvironmentReplaceCommand(this));
          }
        }
        {
          System.Windows.Forms.ToolStripMenuItem menu = CadKit.Tools.Menu.makeMenu("&Tools");
          this.MenuBar.Items.Add(menu);
          this._addMenuButton(menu, new CadKit.Helios.Commands.OptionsCommand(this));
        }
        {
          _windowMenu = CadKit.Tools.Menu.makeMenu("&Windows");
          this.MenuBar.Items.Add(_windowMenu);
        }
      }
    }

    /// <summary>
    /// Build the tool bar.
    /// </summary>
    protected virtual void _buildToolBar()
    {
      lock (_mutex)
      {
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.NewDocumentCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.OpenDocumentCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.SaveDocumentCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewFitCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewHomeCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewFrontCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewBackCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewTopCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewBottomCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewRightCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewLeftCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.PolygonsFillCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.PolygonsHiddenLinesCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.PolygonsLineCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.PolygonsPointsCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ShadeModelSmoothCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ShadeModelFlatCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewerNavigateCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewerPickCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.ViewerSeekCommand(this));
        _toolStrip.Items.Add(new System.Windows.Forms.ToolStripSeparator());
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.FrameDumpCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.DisplayListCommand(this));
        this._addToolbarButton(_toolStrip, new CadKit.Helios.Commands.RenderLoopCommand(this));
      }
    }

    /// <summary>
    /// Add the tool-bar button.
    /// </summary>
    protected void _addToolbarButton(System.Windows.Forms.ToolStrip strip, CadKit.Commands.Command command)
    {
      lock (_mutex)
      {
        if (null != strip && null != command)
        {
          strip.Items.Add(command.ToolButton);
        }
      }
    }

    /// <summary>
    /// Add the menu button.
    /// </summary>
    protected void _addMenuButton(System.Windows.Forms.ToolStripMenuItem menu, CadKit.Commands.Command command)
    {
      lock (_mutex)
      {
        if (null != menu && null != command)
        {
          menu.DropDownItems.Add(command.MenuButton);
        }
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
      get { return this.DockPanel; }
    }

    protected WeifenLuo.WinFormsUI.DockPanel DockPanel
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
    void CadKit.Interfaces.IPersistantFormData.registerPersistanceForm(string name, object obj)
    {
      System.Windows.Forms.Form form = obj as System.Windows.Forms.Form;
      if (null != form)
      {
        _persistantForms.Add(name, form);
      }
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
    void CadKit.Interfaces.IWindowMenu.addFormWindowMenu(string name, object obj)
    {
      try
      {
        System.Windows.Forms.Form form = obj as System.Windows.Forms.Form;
        if (null != form)
        {
          _windowForms[name] = form;

          if (null != _windowMenu)
          {
            System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem(name);
            item.Click += new System.EventHandler(_onWindowItemClick);
            _windowMenu.DropDownItems.Add(item);
          }
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


    /// <summary>
    /// Get/Set the window menu.
    /// </summary>
    public System.Windows.Forms.ToolStripMenuItem WindowMenu
    {
      get { lock (_mutex) { return _windowMenu; } }
      set { lock (_mutex) { _windowMenu = value; } }
    }


    /// <summary>
    /// Surround all messages with try-catch block.
    /// </summary>
    protected override void WndProc(ref System.Windows.Forms.Message m)
    {
      try
      {
        base.WndProc(ref m);
      }
      catch (System.Exception e)
      {
        // Do not direct to console because that will fire more events.
        System.Diagnostics.Trace.WriteLine("Error 2670602680: {0}", e.Message);
      }
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.reference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference()
    {
      // TODO
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference(bool allowCleanup)
    {
      // TODO
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    uint CadKit.Interfaces.IReferenced.RefCount
    {
      get { return 0; } // TODO
    }
  }
}

