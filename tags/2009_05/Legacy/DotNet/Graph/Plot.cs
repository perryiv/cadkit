
namespace CadKit.Graph
{
  public class Plot
  {
    private System.Drawing.Point[] _points = null;
    private System.Drawing.Point _min = new System.Drawing.Point();
    private System.Drawing.Point _max = new System.Drawing.Point();

    /// <summary>
    /// Class that contains points to draw and information on how to draw them.
    /// </summary>
    public Plot()
    {
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Point[] Points
    {
      get { return _points; }
      set { _points = value; this._findMinAndMax(); }
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Point Min
    {
      get { return _min; }
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Point Max
    {
      get { return _max; }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="g"></param>
    public void draw(System.Drawing.Graphics g)
    {
      if (null != this.Points && this.Points.Length > 0)
        g.DrawLines(System.Drawing.Pens.Black, this.Points);
    }


    /// <summary>
    /// 
    /// </summary>
    private void _findMinAndMax()
    {
      int minX = int.MaxValue;
      int minY = int.MaxValue;
      int maxX = int.MinValue;
      int maxY = int.MinValue;

      for (int i = 0; i < _points.Length; ++i)
      {
        int x = _points[i].X;
        int y = _points[i].Y;

        // Set mins and maxs.
        if (x < minX)
          minX = x;
        if (x > maxX)
          maxX = x;
        if (y < minY)
          minY = y;
        if (y > maxY)
          maxY = y;
      }

      this._min = new System.Drawing.Point(minX, minY);
      this._max = new System.Drawing.Point(maxX, maxY);
    }

    internal void scaleAndFlip(float scaleX, float scaleY, int height)
    {
      for (int i = 0; i < _points.Length; ++i)
      {
        _points[i].X = (int) (_points[i].X * scaleX);
        _points[i].Y = (int) (_points[i].Y * scaleY);
        _points[i].Y = height - _points[i].Y;
      }
    }
  }
}
