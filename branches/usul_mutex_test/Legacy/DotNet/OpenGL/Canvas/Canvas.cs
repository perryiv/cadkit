
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.OpenGL
{
  public partial class Canvas :
    System.Windows.Forms.UserControl,
    CadKit.Interfaces.IPropertyGridObject,
    CadKit.Interfaces.IReferenced
  {
    /// <summary>
    /// Default constructor.
    /// </summary>
    public Canvas()
    {
      this.InitializeComponent();
      this.SizeChanged += this.OnResize;
      this.ContextMenuStrip = this.buildContextMenu();

      this.ClearColor = CadKit.Persistence.Registry.Instance.getColor(this.GetType().ToString(), "ClearColor", this.ClearColor);

      // Don't know if this does anything...
      this.SetStyle(System.Windows.Forms.ControlStyles.UserPaint, true);
      this.SetStyle(System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer, true);
      this.SetStyle(System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);
    }


    /// <summary>
    /// Initialize the inner panel.
    /// </summary>
    private void _initInnerPanel()
    {
      // Delete (sort of) the existing one.
      if (null != this.InnerControl)
      {
        if (this.Controls.Contains(this.InnerControl))
          this.Controls.Remove(this.InnerControl);
        this.InnerControl.Dispose();
        this.InnerControl = null;
        System.GC.Collect();
      }

      // Make the inner panel.
      this.InnerControl = new InnerPanel();
      this.Controls.Add(this.InnerControl);
      this.InnerControl.Dock = System.Windows.Forms.DockStyle.Fill;

      // Add the event handlers.
      this.InnerControl.MouseMove += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
      this.InnerControl.MouseUp += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
      this.InnerControl.MouseDown += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
      this.InnerControl.MouseWheel += new System.Windows.Forms.MouseEventHandler(OnMouseWheel);
      this.InnerControl.KeyPress += new System.Windows.Forms.KeyPressEventHandler(OnKeyPress);
      this.InnerControl.KeyDown += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
      this.InnerControl.Paint += this._paint;
      this.InnerControl.Click += new System.EventHandler(OnClick);
    }


    /// <summary>
    /// Build the context menu.
    /// </summary>
    public virtual System.Windows.Forms.ContextMenuStrip buildContextMenu()
    {
      System.Windows.Forms.ContextMenuStrip menu = new System.Windows.Forms.ContextMenuStrip();
      menu.ShowImageMargin = false;
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Properties...";
        item.Click += this.OnEditProperties;
        menu.Items.Add(item);
      }
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Background Color...";
        item.Click += this.OnEditBackgroundColor;
        menu.Items.Add(item);
      }
      {
        System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
        item.Text = "Pixel Format...";
        item.Click += this.OnEditPixelFormat;
        menu.Items.Add(item);
      }
      return menu;
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
