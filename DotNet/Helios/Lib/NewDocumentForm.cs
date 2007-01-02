
namespace CadKit.Helios.Lib
{
  public partial class NewDocumentForm : WeifenLuo.WinFormsUI.DockContent
  {
    private object _caller = null;
    private CadKit.Interfaces.IDocumentNew[] _documentNew = null;
    private static NewDocumentForm _instance = null;

    private NewDocumentForm()
    {
      InitializeComponent();

      this.DockableAreas =
          WeifenLuo.WinFormsUI.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.DockAreas.Float;
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockRight;
      this.HideOnClose = true;
    }


    public object Caller
    {
      get
      {
        return _caller;
      }
      set
      {
        System.Windows.Forms.Form form = _caller as System.Windows.Forms.Form;

        if (null != form)
        {
          form.Shown -= this._parentShown;
          form.Shown += this._parentShown;
        }
        
        _caller = value;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void _parentShown(object sender, System.EventArgs e)
    {
      _documentNew = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IDocumentNew>();

      for (uint i = 0; i < _documentNew.Length; ++i)
      {
        CadKit.Documents.Document doc = _documentNew[i].create(_caller) as CadKit.Documents.Document;
        CadKit.Interfaces.IDocument iDoc = doc as CadKit.Interfaces.IDocument;

        if (null != doc && null != iDoc)
        {
          System.Windows.Forms.LinkLabel label = new System.Windows.Forms.LinkLabel();
          label.Name = iDoc.TypeName;
          label.Text = iDoc.TypeName;
          label.Click += new System.EventHandler(label_Click);
          label.Image = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/new_document.png");
          label.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
          label.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
          _flowLayoutPanel.Controls.Add(label);
        }

        doc = null;
      }

      System.Windows.Forms.Form parent = sender as System.Windows.Forms.Form;
      CadKit.Tools.ToolWindow.configure(this, parent, "New Document", false);

      _configureDockWindow(sender, this);

      CadKit.Interfaces.IWindowMenu windowMenu = sender as CadKit.Interfaces.IWindowMenu;
      if (null != windowMenu)
      {
        windowMenu.addFormWindowMenu(this.Text, this);
      }

      parent.Activate();
    }


    void label_Click(object sender, System.EventArgs e)
    {
      int index = _flowLayoutPanel.Controls.IndexOf(sender as System.Windows.Forms.Control);

      if (index > 0 && index < _documentNew.Length)
      {
        // Create the document.
        CadKit.Documents.Document doc = _documentNew[index].create(_caller) as CadKit.Documents.Document;
        CadKit.Interfaces.IDocument idoc = doc as CadKit.Interfaces.IDocument;
        CadKit.Documents.Manager.Instance.addDocument(idoc);

        // Give the document a command history. Assigning this avoids a dependency.
        if (null != doc)
        {
          doc.CommandHistory = new CadKit.Commands.History();
        }

        // Set the delegate.
        CadKit.Documents.Manager.Instance.setGuiDelegate(idoc, this.Caller);

        // Create the default user-interface.
        if (false == this._createDefaultGui(idoc))
        {
          idoc.close();
          CadKit.Documents.Manager.Instance.remove(idoc);
        }
      }

    }

    /// <summary>
    /// Local types.
    /// </summary>
    private delegate bool CreateDefaultGuiDelegate(CadKit.Interfaces.IDocument idoc);

    /// <summary>
    /// Create the default user-interface.
    /// </summary>
    private bool _createDefaultGui(CadKit.Interfaces.IDocument idoc)
    {
      bool result = false;
      try
      {
        System.Windows.Forms.Form form = CadKit.Helios.Application.Instance.MainForm;
        if (true == form.InvokeRequired)
        {
          CreateDefaultGuiDelegate function = new CreateDefaultGuiDelegate(this._createDefaultGui);
          result = (bool)(form.Invoke(function, new object[] { idoc }));
        }
        else
        {
          CadKit.Interfaces.IGuiCreate gui = idoc as CadKit.Interfaces.IGuiCreate;
          if (null != gui)
          {
            gui.create(this.Caller);
            result = true;
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2697332471: Failed to create default user interface: {0}", e.Message);
      }
      return result;
    }


    private static void _configureDockWindow(object sender, NewDocumentForm form)
    {
      CadKit.Interfaces.IDockPanel dockPanel = sender as CadKit.Interfaces.IDockPanel;
      if (null != dockPanel)
      {
        WeifenLuo.WinFormsUI.DockPanel panel = dockPanel.DockPanel as WeifenLuo.WinFormsUI.DockPanel;
        if (null != panel)
        {

          CadKit.Interfaces.IPersistantFormData register = sender as CadKit.Interfaces.IPersistantFormData;

          if (null != register)
            register.registerPersistanceForm(typeof(NewDocumentForm).ToString(), form);

          // Show the form if we don't have persistant data.  If there is persistant data, it will be shown elsewhere.
          if (false == register.hasPersistantFormData())
            form.Show(panel);
        }
        else
        {
          form.Show();
        }
      }
      else
      {
        form.Show();
      }
    }


    public static NewDocumentForm Instance
    {
      get
      {
        lock ("CadKit.Helios.Lib.Instance")
        {
          if (null == _instance)
            _instance = new NewDocumentForm();
          return _instance;
        }
      }
    }
  }
}
