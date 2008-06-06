
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Color
{
  public class Wheel : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Constants
    /// </summary>
    const float DEG_TO_RAD = ((float)System.Math.PI) / 180;
    const float RAD_TO_DEG = 1.0f / DEG_TO_RAD;

    /// <summary>
    /// Constructor
    /// </summary>
    public Wheel()
      : base()
    {
      // Initialize color.
      //_color = CadKit.Persistence.Registry.Instance.getColor(this._persistentName, "Color", _color);

      // Makes a redraw cause a paint.
      this.SetStyle(System.Windows.Forms.ControlStyles.ResizeRedraw, true);

      // Eliminates flicker.
      this.SetStyle(System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer, true);
      this.SetStyle(System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);

      // Add mouse event handlers.
      this.MouseMove += this._mouseEvent;
      this.MouseDown += this._mouseEvent;

      // Add resize handler.
      this.SizeChanged += _sizeChanged;
    }

    /// <summary>
    /// Called when the size changes.
    /// </summary>
    void _sizeChanged(object sender, System.EventArgs e)
    {
      this._makeBitmap();
    }

    /// <summary>
    /// Called when there is a mouse event.
    /// </summary>
    void _mouseEvent(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if (System.Windows.Forms.MouseButtons.Left == e.Button)
      {
        this.Point = e.Location;
        this.Invalidate();
      }
    }

    /// <summary>
    /// Generate points along the circumference.
    /// </summary>
    private static System.Drawing.PointF[] _makePoints(System.Drawing.PointF center, float radius, int num)
    {
      System.Drawing.PointF[] points = new System.Drawing.PointF[num];
      for (int i = 0; i < points.Length; ++i)
      {
        float degrees = (((float)i) / (points.Length - 1)) * 360;
        float radians = degrees * DEG_TO_RAD;
        float x = radius * ((float)System.Math.Cos(radians));
        float y = radius * ((float)System.Math.Sin(radians));
        points[i].X = center.X + x;
        points[i].Y = center.Y - y;
      }
      return points;
    }

    /// <summary>
    /// Generate colors along the circumference.
    /// </summary>
    private static System.Drawing.Color[] _makeColors(int num, int value)
    {
      System.Drawing.Color[] colors = new System.Drawing.Color[num];
      for (int i = 0; i < num; ++i)
      {
        float u = ((float)i) / (num - 1);
        float hue = u * 255;
        CadKit.Color.HSV hsv = new CadKit.Color.HSV((int)hue, 255, value);
        CadKit.Color.RGB rgb = new CadKit.Color.RGB(hsv);
        colors[i] = System.Drawing.Color.FromArgb(rgb.r, rgb.g, rgb.b);
      }
      return colors;
    }

    /// <summary>
    /// Called when the control needs to be painted.
    /// </summary>
    protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
    {
      // Needed for the default background color.
      base.OnPaint(e);

      // Draw the parts.
      this._drawWheel(e);
      this._drawPointDragger(e);
    }

    /// <summary>
    /// Draw the wheel.
    /// </summary>
    private void _drawWheel(System.Windows.Forms.PaintEventArgs e)
    {
      if (null == _bitmap)
        this._makeBitmap();
      if (null != _bitmap)
        e.Graphics.DrawImage(_bitmap, 0, 0);
    }

    /// <summary>
    /// Make the bitmap for the wheel.
    /// </summary>
    private void _makeBitmap()
    {
      // If there already is a bitmap then dispose it.
      if (null != _bitmap)
        _bitmap.Dispose();

      // Get the size.
      int w = this.ClientRectangle.Width;
      int h = this.ClientRectangle.Height;

      // If the size is too small then punt.
      if (w <= 0 || h <= 0)
        return;

      // The number of points we use is a function of radius.
      int num = (360 * this.Radius) / 100;

      // Never go below this many points.
      num = System.Math.Max(num, 10);

      // Make the points around the perimeter.
      float radius = this.RadiusF;
      System.Drawing.PointF[] points = CadKit.Color.Wheel._makePoints(this.CenterF, radius, num);

      // Make the circle.
      using (System.Drawing.Drawing2D.PathGradientBrush brush = new System.Drawing.Drawing2D.PathGradientBrush(points))
      {
        CadKit.Color.HSV currentHSV = new HSV(this.Color);
        RGB grayscale = new RGB(new HSV(0, 0, currentHSV.v));
        brush.CenterColor = System.Drawing.Color.FromArgb(255, grayscale.r, grayscale.g, grayscale.b);
        brush.CenterPoint = this.CenterF;
        brush.SurroundColors = CadKit.Color.Wheel._makeColors(num, currentHSV.v);

        // Make new bitmap.
        float diameter = radius * 2;
        _bitmap = new System.Drawing.Bitmap(w, h, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

        // Make a new graphic from the bitmap.
        using (System.Drawing.Graphics graphic = System.Drawing.Graphics.FromImage(_bitmap))
        {
          // This writes to the bitmap.
          graphic.FillEllipse(brush, this.ClientRectangle);
        }
      }
    }

    /// <summary>
    /// Get the radius of the wheel.
    /// </summary>
    public float RadiusF
    {
      get
      {
        float w = this.ClientSize.Width;
        float h = this.ClientSize.Height;
        float size = System.Math.Min(w, h);
        return (size / 2);
      }
    }

    /// <summary>
    /// Get the radius of the wheel.
    /// </summary>
    public int Radius
    {
      get
      {
        float r = this.RadiusF;
        return ((int)r);
      }
    }

    /// <summary>
    /// Get the center of the wheel.
    /// </summary>
    public System.Drawing.PointF CenterF
    {
      get
      {
        float w = this.ClientSize.Width;
        float h = this.ClientSize.Height;
        float x = w / 2;
        float y = h / 2;
        return new System.Drawing.PointF(x, y);
      }
    }

    /// <summary>
    /// Get the center of the wheel.
    /// </summary>
    public System.Drawing.Point Center
    {
      get
      {
        System.Drawing.PointF c = this.CenterF;
        return new System.Drawing.Point((int)c.X, (int)c.Y);
      }
    }

    /// <summary>
    /// Draw the color dragger.
    /// </summary>
    private void _drawPointDragger(System.Windows.Forms.PaintEventArgs e)
    {
      const float size = 5;
      System.Drawing.PointF point = this.PointF;
      e.Graphics.DrawRectangle(System.Drawing.Pens.Black, point.X, point.Y, size, size);
    }

    /// <summary>
    /// Set/get the color.
    /// </summary>
    public System.Drawing.Color Color
    {
      get { return _color; }
      set
      {
        _color = value;
        if (null != this._persistentName)
          CadKit.Persistence.Registry.Instance.setColor(this._persistentName, "Color", value);
        if (null != this.ColorChanged)
          this.ColorChanged(this, new CadKit.Color.ColorChangedEventArgs(value));
        this._bitmap = null;
      }
    }

    /// <summary>
    /// Get the point.
    /// </summary>
    public System.Drawing.PointF PointF
    {
      get
      {
        // Get the HSV color.
        CadKit.Color.HSV hsv = new CadKit.Color.HSV(_color);

        // Calculate the angle from the hue.
        float degrees = (360.0f * hsv.h) / 255;
        float radians = degrees * DEG_TO_RAD;

        // Calculate the radius from the saturation.
        float radius = (this.RadiusF * hsv.s) / 255;

        // Get the center of the wheel.
        System.Drawing.PointF center = this.CenterF;

        // Calculate the coordinates.
        float x = center.X + radius * ((float)System.Math.Cos(radians));
        float y = center.Y - radius * ((float)System.Math.Sin(radians));

        // Return the new point.
        return new System.Drawing.PointF(x, y);
      }
      set
      {
        // Flip the y coordinate.
        float mx = value.X;
        float my = this.ClientRectangle.Height - value.Y;

        // Offset the point with respect to the center.
        System.Drawing.PointF center = this.CenterF;
        System.Drawing.PointF point = new System.Drawing.PointF(mx - center.X, my - center.Y);

        // Calculate the angle in degrees.
        float radians = ((float)System.Math.Atan(point.Y / point.X));
        float degrees = RAD_TO_DEG * radians;
        if (point.X < 0)
          degrees += 180;
        degrees = (degrees + 360) % 360;

        // Get the hue from the angle.
        float h = (degrees * 255) / 360;

        // Calculate the radius.
        float radius = ((float)System.Math.Sqrt(point.X * point.X + point.Y * point.Y));

        // Get the saturation. Keep it in range.
        float s = System.Math.Min((radius * 255) / this.RadiusF, 255);

        // Get the current value channel.
        CadKit.Color.HSV current = new CadKit.Color.HSV(_color);
        float v = current.v;

        // Make the new RGB color.
        CadKit.Color.RGB rgb = new CadKit.Color.RGB(new CadKit.Color.HSV((int)h, (int)s, (int)v));

        // Set the color.
        this.Color = System.Drawing.Color.FromArgb(rgb.r, rgb.g, rgb.b);
      }
    }

    /// <summary>
    /// Get the point.
    /// </summary>
    public System.Drawing.Point Point
    {
      get
      {
        System.Drawing.PointF point = this.PointF;
        return new System.Drawing.Point((int)point.X, (int)point.Y);
      }
      set
      {
        this.PointF = new System.Drawing.PointF((float)value.X, (float)value.Y);
      }
    }

    /// <summary>
    /// Called by the client to dispose resources.
    /// </summary>
    public new void Dispose()
    {
      if (null != _bitmap)
        _bitmap.Dispose();
      _bitmap = null;
    }

    /// <summary>
    /// Set/get the persistant name.
    /// </summary>
    public string PersistentName
    {
      get { return _persistentName; }
      set { _persistentName = value; }
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
    /// Data members.
    /// </summary>
    string _persistentName = null;
    private System.Drawing.Color _color = System.Drawing.Color.Black;
    private System.Drawing.Bitmap _bitmap = null;
  }
}
