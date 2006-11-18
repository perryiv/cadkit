
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
    CadKit.Interfaces.IPropertyGridObject
  {
    /// <summary>
    /// Default constructor.
    /// </summary>
    public Canvas()
    {
      this.InitializeComponent();
      this.SizeChanged += this.OnResize;
      this.ContextMenuStrip = this.buildContextMenu();
      base.BackColor = CadKit.Persistence.Registry.Instance.getColor(this.GetType().ToString(), "BackgroundColor", base.BackColor);

      // Don't know if this does anything...
      this.SetStyle(System.Windows.Forms.ControlStyles.UserPaint | System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer | System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);
    }


    /// <summary>
    /// Initialize the inner panel.
    /// </summary>
    private void _initInnerPanel()
    {
      // Delete (sort of) the existing one.
      if (null != _innerPanel)
      {
        if (this.Controls.Contains(_innerPanel))
          this.Controls.Remove(_innerPanel);
        _innerPanel.Dispose();
        _innerPanel = null;
        System.GC.Collect();
      }

      // Make the inner panel.
      _innerPanel = new System.Windows.Forms.Panel();
      this.Controls.Add(_innerPanel);
      _innerPanel.Dock = System.Windows.Forms.DockStyle.Fill;

      // Add the event handlers.
      _innerPanel.MouseMove  += new System.Windows.Forms.MouseEventHandler(OnMouseMove);
      _innerPanel.MouseUp    += new System.Windows.Forms.MouseEventHandler(OnMouseUp);
      _innerPanel.MouseDown  += new System.Windows.Forms.MouseEventHandler(OnMouseDown);
      _innerPanel.MouseWheel += new System.Windows.Forms.MouseEventHandler(OnMouseWheel);
      _innerPanel.KeyPress   += new System.Windows.Forms.KeyPressEventHandler(OnKeyPress);
      _innerPanel.KeyDown    += new System.Windows.Forms.KeyEventHandler(OnKeyDown);
      _innerPanel.Paint      += this._paint;
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
  }
}
