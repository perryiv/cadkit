
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls
{
  [
    System.Serializable()
  ]
  public class ColorProperties :
    System.Runtime.Serialization.ISerializable
  {
    /// <summary>
    /// What mode to color in.
    /// </summary>
    public enum Mode
    {
      SINGLE_COLOR,
      GRADIENT_COLOR
    };

    private Mode _mode = Mode.SINGLE_COLOR;
    private System.Drawing.Color _color = System.Drawing.Color.Blue;
    private System.Drawing.Color _minColor = System.Drawing.Color.SkyBlue;
    private System.Drawing.Color _maxColor = System.Drawing.Color.DarkBlue;


    /// <summary>
    /// 
    /// </summary>
    public ColorProperties()
    {
      _mode = Mode.SINGLE_COLOR;
    }


    public ColorProperties(ColorProperties colorProperties)
    {
      this._mode = colorProperties._mode;
      this._color = colorProperties._color;
      this._minColor = colorProperties._minColor;
      this._maxColor = colorProperties._maxColor;
    }


    /// <summary>
    /// 
    /// </summary>
    public ColorProperties(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      this.ColorMode = (Mode)info.GetValue("ColorMode", typeof(Mode));
      this.Color = (System.Drawing.Color)info.GetValue("Color", typeof(System.Drawing.Color));
      this.MinColor = (System.Drawing.Color)info.GetValue("MinColor", typeof(System.Drawing.Color));
      this.MaxColor = (System.Drawing.Color)info.GetValue("MaxColor", typeof(System.Drawing.Color));
    }


    /// <summary>
    /// 
    /// </summary>
    void System.Runtime.Serialization.ISerializable.GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      info.AddValue("ColorMode", this.ColorMode);
      info.AddValue("Color", this.Color);
      info.AddValue("MinColor", this.MinColor);
      info.AddValue("MaxColor", this.MaxColor);
    }


    /// <summary>
    /// 
    /// </summary>
    public Mode ColorMode
    {
      get { return _mode; }
      set { _mode = value; }
    }

    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Color Color
    {
      get { return _color; }
      set { _color = value; }
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Color MinColor
    {
      get { return _minColor; }
      set { _minColor = value; }
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Drawing.Color MaxColor
    {
      get { return _maxColor; }
      set { _maxColor = value; }
    }

    /// <summary>
    /// 
    /// </summary>
    public DT.Minerva.Glue.BaseColorFunctor colorFunctor(BaseLayer layer)
    {
      if (_mode == Mode.SINGLE_COLOR)
      {
        DT.Minerva.Glue.SingleColorFunctor functor = new DT.Minerva.Glue.SingleColorFunctor();
        functor.Color = _color;
        return functor;
      }
      else if (_mode == Mode.GRADIENT_COLOR)
      {
        DT.Minerva.Glue.GradientColorFunctor funtor = new DT.Minerva.Glue.GradientColorFunctor();
        funtor.MinColor = MinColor;
        funtor.MaxColor = MaxColor;

        DT.Minerva.Interfaces.IDataSource dataSource = layer.DataSource;

        if (null != dataSource)
        {
          string columnName = layer.ColorColumn;
          funtor.MinValue = double.Parse(dataSource.getMinValue(layer.DataTable, columnName));
          funtor.MaxValue = double.Parse(dataSource.getMaxValue(layer.DataTable, columnName));
          return funtor;
        }
      }

      return null;
    }
  }
}
