

namespace CadKit.Graph
{
  public class Points : System.Collections.Generic.List<System.Drawing.Point> { }

  public partial class Panel : System.Windows.Forms.UserControl
  {
    private Plot _plot = null;
    private bool _needToScalePoints = false;

    public Panel()
    {
      InitializeComponent();
      this.BackColor = System.Drawing.Color.White;
    }

    public Plot Plot
    {
      get { return _plot; }
      set { _plot = value; _needToScalePoints = true; }
    }

    protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
    {
      base.OnPaint(e);

      if (_needToScalePoints)
        this._scaleAndFlipPoints();

      if(null != _plot)
        _plot.draw(e.Graphics);
    }


    protected override void OnResize(System.EventArgs e)
    {
      base.OnResize(e);
      this._needToScalePoints = true;
    }


    private void _scaleAndFlipPoints()
    {
      if (null == _plot)
        return;

      int extentX = _plot.Max.X - _plot.Min.X;
      int extentY = _plot.Max.Y - _plot.Min.Y;

      float scaleX = (float) this.Width / (float)extentX;
      float scaleY = (float) this.Height / (float)extentY;

      _plot.scaleAndFlip(scaleX, scaleY, this.Height);

      _needToScalePoints = false;
    }
  }
}
