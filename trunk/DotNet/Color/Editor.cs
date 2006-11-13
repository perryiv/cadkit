
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Color
{
  public partial class Editor : System.Windows.Forms.UserControl
  {

    /// <summary>
    /// Class to edit a color using HAV cone.
    /// </summary>
    public Editor()
    {
      InitializeComponent();

      _wheel.ColorChanged += new Wheel.ColorChangedEventHandler(_wheel_ColorChanged);
      _wheel.MouseUp += new System.Windows.Forms.MouseEventHandler(_onMouseUp);
      _value.ValueChanged += new System.EventHandler(_value_ValueChanged);
      _value.MouseUp += new System.Windows.Forms.MouseEventHandler(_onMouseUp);
      _value.Minimum = 0;
      _value.Maximum = 255;
    }


    /// <summary>
    /// Editing is done.
    /// </summary>
    void _onMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if (null != this.ColorEditDone)
        this.ColorEditDone(this, new CadKit.Color.ColorChangedEventArgs(this.Color));
    }


    /// <summary>
    /// The value slider has changed.
    /// </summary>
    void _value_ValueChanged(object sender, System.EventArgs e)
    {
      _wheel.ColorChanged -= new Wheel.ColorChangedEventHandler(_wheel_ColorChanged);

      HSV hsv = new HSV(_wheel.Color);
      hsv.v = _value.Value;
      RGB rgb = new RGB(hsv);
      this.Color = System.Drawing.Color.FromArgb( _wheel.Color.A, rgb.r, rgb.g, rgb.b );

      _wheel.ColorChanged += new Wheel.ColorChangedEventHandler(_wheel_ColorChanged);
    }


    /// <summary>
    /// The color wheel as changed.
    /// </summary>
    void _wheel_ColorChanged(object sender, ColorChangedEventArgs e)
    {
      _value.ValueChanged -= new System.EventHandler(_value_ValueChanged);
      HSV hsv = new HSV(_wheel.Color);
      _value.Value = hsv.v;

      if (null != this.ColorChanged)
        this.ColorChanged(this, new CadKit.Color.ColorChangedEventArgs(this.Color));

      _value.ValueChanged += new System.EventHandler(_value_ValueChanged);
    }


    /// <summary>
    /// Get/Set the color.
    /// </summary>
    public System.Drawing.Color Color
    {
      get
      {
        return _wheel.Color;
      }
      set
      {
        _wheel.Color = value;
        _wheel.Refresh();

        if (null != this.ColorChanged)
          this.ColorChanged(this, new CadKit.Color.ColorChangedEventArgs(value));
      }
    }


    /// <summary>
    /// Get/Set the persistent name.
    /// </summary>
    public string PersistentName
    {
      get { return _wheel.PersistentName; }
      set { _wheel.PersistentName = value; }
    }

    /// <summary>
    /// The kind of delegate that handles color-changed events.
    /// </summary>
    public delegate void ColorChangedEventHandler(object sender, CadKit.Color.ColorChangedEventArgs e);

    /// <summary>
    /// Called when the color changes.
    /// </summary>
    public event ColorChangedEventHandler ColorChanged;


    /// <summary>
    /// Called when color edit is finished.
    /// </summary>
    public event ColorChangedEventHandler ColorEditDone;
  }
}
