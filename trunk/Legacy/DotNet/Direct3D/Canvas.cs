
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Direct3D
{
  public partial class Canvas : System.Windows.Forms.Panel
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private System.ComponentModel.IContainer components;
    private object _mutex = new object();
    private Microsoft.DirectX.Direct3D.Device _device = null;

    /// <summary>
    /// Delegates
    /// </summary>
    private delegate void VoidReturnWithNoArguments();

    /// <summary>
    /// Constructor
    /// </summary>
    public Canvas() : base()
    {
      this.InitializeComponent();
      this.init();
      this.SizeChanged += this._sizeChanged;
      this.ContextMenuStrip = this.buildContextMenu();
      base.BackColor = CadKit.Persistence.Registry.Instance.getColor(this.GetType().ToString(), "BackgroundColor", base.BackColor);
    }

    #region Component Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      this.SuspendLayout();
      // 
      // Canvas
      // 
      this.Name = "Canvas";
      this.ResumeLayout(false);

    }

    #endregion

    /// <summary>
    /// Initialize the graphics.
    /// </summary>
    public void init()
    {
      try
      {
        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnWithNoArguments(this.init));
        }
        else
        {
          this.clean();
          Microsoft.DirectX.Direct3D.PresentParameters presentParams = new Microsoft.DirectX.Direct3D.PresentParameters();
          presentParams.Windowed = true;
          presentParams.SwapEffect = Microsoft.DirectX.Direct3D.SwapEffect.Discard;
          _device = new Microsoft.DirectX.Direct3D.Device(0, Microsoft.DirectX.Direct3D.DeviceType.Hardware, this, Microsoft.DirectX.Direct3D.CreateFlags.SoftwareVertexProcessing, presentParams);
        }
      }
      catch (Microsoft.DirectX.DirectXException e)
      {
        System.Console.WriteLine("Error 1725801807: {0}", e.Message);
        _device = null;
      }
    }

    /// <summary>
    /// Clean the graphics.
    /// </summary>
    public void clean()
    {
      try
      {
        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnWithNoArguments(this.clean));
        }
        else
        {
          if (null != _device)
          {
            _device.Dispose();
            _device = null;
            System.GC.Collect();
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2842649394: {0}", e.Message);
        _device = null;
      }
    }

    /// <summary>
    /// Paint the background. We override this to prevent flicker.
    /// </summary>
    protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs a)
    {
      if (false == this.InvokeRequired)
      {
      }
    }

    /// <summary>
    /// Called when the control needs to be painted.
    /// </summary>
    protected override void OnPaint(System.Windows.Forms.PaintEventArgs a)
    {
      if (false == this.InvokeRequired)
      {
        this.render();
      }
    }

    /// <summary>
    /// Called before the parent form closes.
    /// </summary>
    private void _formClosing(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    private void _editProperties(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    private void _editBackgroundColor(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Called when the context menu button is selected.
    /// </summary>
    private void _editPixelFormat(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Called when the size changes.
    /// </summary>
    private void _sizeChanged(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Call to render the scene.
    /// </summary>
    public void render()
    {
      try
      {
        if (true == this.InvokeRequired)
        {
          this.BeginInvoke(new VoidReturnWithNoArguments(this.render));
        }
        else
        {
          this._render();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 5364542090: {0}", e.Message);
      }
    }

    /// <summary>
    /// Called by public render function.
    /// </summary>
    private void _render()
    {
      this._checkThread(3425569000, "CadKit.Direct3D.Canvas._render");

      try
      {
        // Do nothing if the device is null. 
        if (null == _device)
          return;

        // Clear the background.
        _device.Clear(Microsoft.DirectX.Direct3D.ClearFlags.Target, base.BackColor, 1.0f, 0);

        // Begin the scene. If this throws then we do not call EndScene.
        _device.BeginScene();

        // Render the scene. This should never throw.
        //_device.draw

        // End the scene.
        _device.EndScene();
        _device.Present();
      }

      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2842350084: {0}", e.Message);
      }
    }

    /// <summary>
    /// Build the context menu.
    /// </summary>
    public System.Windows.Forms.ContextMenuStrip buildContextMenu()
    {
      this._checkThread(1033805713, "CadKit.Direct3D.Canvas.buildContextMenu");

      System.Windows.Forms.ContextMenuStrip menu = null;
      try
      {
        menu = new System.Windows.Forms.ContextMenuStrip();
        menu.ShowImageMargin = false;
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Properties...";
          item.Click += this._editProperties;
          menu.Items.Add(item);
        }
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Background Color...";
          item.Click += this._editBackgroundColor;
          menu.Items.Add(item);
        }
        {
          System.Windows.Forms.ToolStripMenuItem item = new System.Windows.Forms.ToolStripMenuItem();
          item.Text = "Pixel Format...";
          item.Click += this._editPixelFormat;
          menu.Items.Add(item);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 5364542090: {0}", e.Message);
      }
      return menu;
    }

    /// <summary>
    /// Throw an exception if this function gets called from the non-gui thread.
    /// </summary>
    private void _checkThread(System.UInt32 id, string function)
    {
      if (true == this.InvokeRequired)
      {
        throw new System.Exception
          (System.String.Format
            ("Error {0}: calling {1}() from wrong thread '{2}', use BeginInvoke()",
             id, function, System.Threading.Thread.CurrentThread.ToString()));
      }
    }
  }
}
