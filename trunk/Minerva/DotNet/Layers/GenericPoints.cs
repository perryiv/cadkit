
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using Strings = System.Collections.Generic.List<string>;

namespace DT.Minerva.Layers.Controls
{
  [
    System.Serializable()
  ]
  public partial class GenericPoints : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.IDataTables
  {
    private DT.Minerva.Glue.PointLayerGlue _pointLayer = new DT.Minerva.Glue.PointLayerGlue();


    /// <summary>
    /// 
    /// </summary>
    public GenericPoints()
      : base()
    {
      this.Layer = _pointLayer;
      this.Name = "Custom Points";
    }


    /// <summary>
    /// 
    /// </summary>
    protected GenericPoints(GenericPoints layer)
      : base(layer)
    {
      _pointLayer = new DT.Minerva.Glue.PointLayerGlue(layer._pointLayer);
      this.Layer = _pointLayer;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new GenericPoints(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public GenericPoints(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _pointLayer;
      this._deserialize(info);
    }


    /// <summary>
    /// 
    /// </summary>
    public new void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      this._serialize(info);
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);
      info.AddValue("PrimitiveType", this.PrimitiveType);
      info.AddValue("PrimitiveSize", this.PrimitiveSize);
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _deserialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._deserialize(info);
      try
      {
        this.PrimitiveType = info.GetString("PrimitiveType");
        this.PrimitiveSize = System.Convert.ToSingle(info.GetValue("PrimitiveSize", typeof(float)));
      }
      catch (System.Runtime.Serialization.SerializationException)
      {
      }
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~GenericPoints()
    {
      _pointLayer = null;
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _setLayerProperties()
    {
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public string[] DataTables
    {
      get
      {
        return this.DataSource.PointTables;
      }
    }


    /// <summary>
    /// Get/Set the primitive type.
    /// </summary>
    [
      System.ComponentModel.Category("Primitive"),
      System.ComponentModel.Description("Type of primitive"),
      System.ComponentModel.Browsable(true),
      System.ComponentModel.TypeConverter(typeof(TypeConverters.PointPrimitiveTypes))
    ]
    public string PrimitiveType
    {
      get { return _pointLayer.PrimitiveType; }
      set { _pointLayer.PrimitiveType = value; }
    }


    /// <summary>
    /// Get/Set the primitive size.
    /// </summary>
    [
      System.ComponentModel.Category("Primitive"),
      System.ComponentModel.Description("Size of primitive"),
      System.ComponentModel.Browsable(true),
    ]
    public float PrimitiveSize
    {
      get { return _pointLayer.Size; }
      set { _pointLayer.Size = value; }
    }
  }
}
