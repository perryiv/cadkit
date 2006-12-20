
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.SnapShot
{
  public partial class SnapShotWindow : WeifenLuo.WinFormsUI.DockContent, System.IDisposable
  {
    /// <summary>
    /// Local constants.
    /// </summary>
    private const string REGISTRY_SECTION = "SnapShotWindow";
    private const string FRAME_SCALE = "FrameScale";
    private const string SCATTER_SCALE = "ScatterScale";
    private const string RENDER_PASSES = "RenderPasses";


    /// <summary>
    /// Local types.
    /// </summary>
    class Files : System.Collections.Generic.List<string> { }


    /// <summary>
    /// Data members.
    /// </summary>
    Files _files = new Files();


    /// <summary>
    /// Constructor
    /// </summary>
    public SnapShotWindow()
    {
      try
      {
        this.InitializeComponent();

        this.DockableAreas =
          WeifenLuo.WinFormsUI.DockAreas.DockBottom |
          WeifenLuo.WinFormsUI.DockAreas.DockLeft |
          WeifenLuo.WinFormsUI.DockAreas.DockRight |
          WeifenLuo.WinFormsUI.DockAreas.DockTop |
          WeifenLuo.WinFormsUI.DockAreas.Float;
        this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockBottom;
        this.HideOnClose = true;
        CadKit.Documents.Manager.Instance.ActiveViewChanged += this._activeViewChanged;
        this.FormClosing += this._formClosing;
        this.Disposed += this._disposed;

        _frameScaleTextBox.Text = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, FRAME_SCALE, "1.0");
        _scatterScaleTextBox.Text = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, SCATTER_SCALE, "1.0");
        _renderPassesComboBox.Text = CadKit.Persistence.Registry.Instance.getString(REGISTRY_SECTION, RENDER_PASSES, "1");
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3475115067: {0}", e.Message);
        this.Controls.Clear();
      }
    }


    /// <summary>
    /// Destructor
    /// </summary>
    ~SnapShotWindow()
    {
      this._deleteFiles();
    }


    /// <summary>
    /// Dispose this instance.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      this._deleteFiles();
    }


    /// <summary>
    /// Called when the form is closing.
    /// </summary>
    void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs args)
    {
      this._deleteFiles();
    }


    /// <summary>
    /// Called when the form is disposed.
    /// </summary>
    void _disposed(object sender, System.EventArgs e)
    {
      this._deleteFiles();
    }


    /// <summary>
    /// Dispose this instance.
    /// </summary>
    private void _deleteFiles()
    {
      try
      {
        foreach (string file in _files)
        {
          try
          {
            if (true == System.IO.File.Exists(file))
            {
              System.IO.File.Delete(file);
            }
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 4320789770: {0}", e.Message);
          }
        }
        _files.Clear();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2007655518: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the active view changes.
    /// </summary>
    private void _activeViewChanged(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView)
    {
      try
      {
        this._populateControls();
        this._setEnable(_snapPictureButton);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 9780016380: {0}", e.Message);
      }
    }


    /// <summary>
    /// Populate the controls from the active view.
    /// </summary>
    private void _populateControls()
    {
      this._clearControls();
      CadKit.Interfaces.IDocumentView view = CadKit.Documents.Manager.Instance.ActiveView;
      CadKit.Interfaces.IJitterAntialias jitter = view as CadKit.Interfaces.IJitterAntialias;
      if (null != jitter)
      {
        _renderPassesComboBox.Items.AddRange(new object[] { jitter.AvailableRenderingPasses });
      }
    }


    /// <summary>
    /// Clear the controls.
    /// </summary>
    private void _clearControls()
    {
      _renderPassesComboBox.SelectedItem = null;
      _renderPassesComboBox.Items.Clear();
      _errorProvider.SetError(_renderPassesComboBox, null);
    }


    /// <summary>
    /// Called when the button needs to be painted.
    /// </summary>
    private void _snapPictureButtonPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        this._setEnable(sender);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1393722746: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the button needs to be painted.
    /// </summary>
    private void _setEnable(object sender)
    {
      System.Windows.Forms.Control control = sender as System.Windows.Forms.Control;
      if (null != control)
      {
        CadKit.Interfaces.IDocumentView view = CadKit.Documents.Manager.Instance.ActiveView;
        CadKit.Interfaces.ISnapShot snap = view as CadKit.Interfaces.ISnapShot;
        control.Enabled = (null != snap);
      }
    }


    /// <summary>
    /// Called when the button is pressed to take a picture.
    /// </summary>
    private void _snapPictureButtonClick(object sender, System.EventArgs args)
    {
      try
      {
        this._snapPicture();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2062362483: {0}", e.Message);
      }
    }


    /// <summary>
    /// Take a picture.
    /// </summary>
    private void _snapPicture()
    {
      try
      {
        CadKit.Interfaces.IDocumentView view = CadKit.Documents.Manager.Instance.ActiveView;
        CadKit.Interfaces.ISnapShot snap = view as CadKit.Interfaces.ISnapShot;
        if (null != snap)
        {
          // Get properties.
          float frameScale = this._getFloat(_frameScaleTextBox.Text, _frameScaleTextBox);
          float scatterScale = this._getFloat(_scatterScaleTextBox.Text, _scatterScaleTextBox);
          uint renderPasses = this._getUInt32(_renderPassesComboBox.Text, _renderPassesComboBox);

          // Save in registry.
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, FRAME_SCALE, _frameScaleTextBox.Text);
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, SCATTER_SCALE, _scatterScaleTextBox.Text);
          CadKit.Persistence.Registry.Instance.setString(REGISTRY_SECTION, RENDER_PASSES, _renderPassesComboBox.Text);

          // Make file name.
          string file = this._makeTempImageFile("bmp");

          // Feedback.
          System.Console.WriteLine(System.String.Format("Saving image file: {0}", file));
          System.DateTime start = System.DateTime.Now;

          // Take picture.
          snap.takePicture(file, renderPasses, frameScale, scatterScale);

          // Feedback.
          System.TimeSpan duration = System.DateTime.Now - start;
          System.Console.WriteLine(System.String.Format("Time to save image file: {0}", duration));

          // Save in list.
          _files.Add(file);

          // Launch default program for image file.
          System.Diagnostics.Process.Start(file);
        }
      }

      // Eat these exceptions.
      catch (System.ArgumentNullException)
      {
      }
      catch (System.OverflowException)
      {
      }
      catch (System.FormatException)
      {
      }
    }


    /// <summary>
    /// Make a temporary image file.
    /// </summary>
    private string _makeTempImageFile(string ext)
    {
      string file = System.IO.Path.GetTempFileName();
      System.IO.FileInfo info = new System.IO.FileInfo(file);
      System.IO.File.Delete(file);
      string app = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
      string dir = System.String.Format("{0}/{1}", info.DirectoryName, app);
      System.IO.Directory.CreateDirectory(dir);
      file = System.String.Format("{0}/{1}", dir, info.Name.Replace(".tmp", "." + ext));
      return file;
    }


    /// <summary>
    /// Get the value from the control's text.
    /// </summary>
    private float _getFloat(string text, System.Windows.Forms.Control control)
    {
      return (float)this._getDouble(text, control);
    }


    /// <summary>
    /// Get the value from the control's text.
    /// </summary>
    private double _getDouble(string text, System.Windows.Forms.Control control)
    {
      try
      {
        this._setErrorProvider(control, null);
        return System.Double.Parse(text);
      }
      catch (System.ArgumentNullException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
      catch (System.OverflowException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
      catch (System.FormatException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
    }


    /// <summary>
    /// Get the value from the control's text.
    /// </summary>
    private uint _getUInt32(string text, System.Windows.Forms.Control control)
    {
      try
      {
        this._setErrorProvider(control, null);
        return System.UInt32.Parse(text);
      }
      catch (System.ArgumentNullException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
      catch (System.OverflowException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
      catch (System.FormatException e)
      {
        this._setErrorProvider(control, e.Message);
        throw;
      }
    }


    /// <summary>
    /// Set the error provider.
    /// </summary>
    private void _setErrorProvider(System.Windows.Forms.Control control, string message)
    {
      if (null != control)
      {
        _errorProvider.SetError(control, message);
      }
    }
  }
}
