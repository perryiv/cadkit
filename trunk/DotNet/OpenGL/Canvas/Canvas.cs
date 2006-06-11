
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace OpenGL
  {
    public partial class Canvas : System.Windows.Forms.Panel
    {
      /// <summary>
      /// Default constructor.
      /// </summary>
      public Canvas()
      {
        this.InitializeComponent();
        this.SizeChanged += this.OnResize;
        this.ContextMenuStrip = this.buildContextMenu();
        base.BackColor = CadKit.Persistence.Registry.instance().getColor( this.GetType().ToString(), "BackgroundColor", base.BackColor );
      }

      /// <summary>
      /// Initialize the inner panel.
      /// </summary>
      private void _initInnerPanel()
      {
        // Find parent form.
        System.Windows.Forms.Form parent = this.FindForm();
        if ( null == parent )
          throw new System.Exception( "Error 1547542956: No parent form found when initializing the inner panel" );

        // Delete (sort of) the existing one.
        if ( null != _innerPanel )
        {
          if ( this.Controls.Contains ( _innerPanel ) )
            this.Controls.Remove( _innerPanel );
          _innerPanel.Dispose();
          _innerPanel = null;
          System.GC.Collect();
        }

        // Make sure our callback is connected.
        if ( false == _flags.hasFormClosingCallback )
          parent.FormClosing += this.OnFormClosing;
        _flags.hasFormClosingCallback = true;

        // Make the inner panel.
        _innerPanel = new System.Windows.Forms.Panel();
        this.Controls.Add( _innerPanel );
        _innerPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      }

      /// <summary>
      /// Build the context menu.
      /// </summary>
      public System.Windows.Forms.ContextMenuStrip buildContextMenu()
      {
        System.Windows.Forms.ContextMenuStrip menu = new System.Windows.Forms.ContextMenuStrip();
        menu.ShowImageMargin = false;
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Properties...";
          item.Click += this.OnEditProperties;
          menu.Items.Add( item );
        }
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Background Color...";
          item.Click += this.OnEditBackgroundColor;
          menu.Items.Add( item );
        }
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Pixel Format...";
          item.Click += this.OnEditPixelFormat;
          menu.Items.Add( item );
        }
        return menu;
      }
    }
  }
}
