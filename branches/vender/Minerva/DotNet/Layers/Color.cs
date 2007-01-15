
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls
{
  /// <summary>
  /// 
  /// </summary>
  public partial class ColorChooser : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Data members.
    /// </summary>    
    private System.Windows.Forms.Design.IWindowsFormsEditorService _editorService = null;
    private BaseLayer _layer = null;
    private object _caller = null;


    public ColorChooser()
    {
    }

    /// <summary>
    /// Constructor.
    /// </summary>
    public ColorChooser(object caller, System.Windows.Forms.Design.IWindowsFormsEditorService editorService)
    {
      InitializeComponent();

      _caller = caller;
      _layer = caller as BaseLayer;
      _editorService = editorService;

      _singleColor.AutoCheck = true;
      _gradientColor.AutoCheck = true;

      if (null != _layer)
      {
        if (_layer.ColorProperties.ColorMode == ColorProperties.Mode.SINGLE_COLOR)
          _singleColor.Checked = true;
        else if (_layer.ColorProperties.ColorMode == ColorProperties.Mode.GRADIENT_COLOR)
          _gradientColor.Checked = true;
      }

      this._buildControls();
    }
    

    /// <summary>
    /// 
    /// </summary>
    private void _singleColor_CheckedChanged(object sender, System.EventArgs e)
    {
      if (null != _layer)
      {
        _layer.ColorProperties.ColorMode = ColorProperties.Mode.SINGLE_COLOR;
        this._buildControls();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private System.Windows.Forms.Button _createButton(System.Drawing.Color color)
    {
      System.Windows.Forms.Button button = new System.Windows.Forms.Button();
      button.BackColor = color;
      button.Size = new System.Drawing.Size(40, 20);
      button.Click += new System.EventHandler(button_Click);
      return button;
    }


    /// <summary>
    /// 
    /// </summary>
    void button_Click(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      System.Windows.Forms.ColorDialog dialog = new System.Windows.Forms.ColorDialog();
      dialog.Color = button.BackColor;

      if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        button.BackColor = dialog.Color;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _gradientColor_CheckedChanged(object sender, System.EventArgs e)
    {
      if (null != _layer)
      {
        _layer.ColorProperties.ColorMode = ColorProperties.Mode.GRADIENT_COLOR;
        this._buildControls();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _buildControls()
    {
      if (null == _layer)
        return;

      flowLayoutPanel1.Controls.Clear();

      if (_layer.ColorProperties.ColorMode == ColorProperties.Mode.GRADIENT_COLOR)
      {
        // Min button.
        System.Windows.Forms.Button minButton = this._createButton(_layer.ColorProperties.MinColor);
        minButton.BackColorChanged += new System.EventHandler(minButton_BackColorChanged);
        flowLayoutPanel1.Controls.Add(minButton);

        // Max button.
        System.Windows.Forms.Button maxButton = this._createButton(_layer.ColorProperties.MaxColor);
        maxButton.BackColorChanged += new System.EventHandler(maxButton_BackColorChanged);
        flowLayoutPanel1.Controls.Add(maxButton);
      }
      else if (_layer.ColorProperties.ColorMode == ColorProperties.Mode.SINGLE_COLOR)
      {
        System.Windows.Forms.Button button = this._createButton(_layer.ColorProperties.Color);
        button.BackColorChanged += new System.EventHandler(button_BackColorChanged);
        flowLayoutPanel1.Controls.Add(button);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void button_BackColorChanged(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button && null != _layer)
        _layer.ColorProperties.Color = button.BackColor;
    }


    /// <summary>
    /// 
    /// </summary>
    void maxButton_BackColorChanged(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button && null != _layer)
        _layer.ColorProperties.MaxColor = button.BackColor;
    }


    /// <summary>
    /// 
    /// </summary>
    void minButton_BackColorChanged(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Button button = sender as System.Windows.Forms.Button;

      if (null != button && null != _layer)
         _layer.ColorProperties.MinColor = button.BackColor;
    }


    /// <summary>
    /// 
    /// </summary>
    private void _cancel_Click(object sender, System.EventArgs e)
    {
      _editorService.CloseDropDown();
    }


    /// <summary>
    /// 
    /// </summary>
    private void _okButton_Click(object sender, System.EventArgs e)
    {
      _editorService.CloseDropDown();
    }
  }
}
