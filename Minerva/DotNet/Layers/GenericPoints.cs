
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
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
      _pointLayer = layer._pointLayer;
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
      base.GetObjectData(info, context);
    }


    /// <summary>
    /// Destructor.
    /// </summary>
    ~GenericPoints()
    {
      _pointLayer = null;
    }


    protected override void _setLayerProperties()
    {
      _pointLayer.Query = this.Query;
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public string Query
    {
      get
      {
        return "SELECT id FROM " + this.DataTable;
      }
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
        return this.DataSource.PointTables.ToArray();
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


    public System.Drawing.Color Color
    {
      get { return (System.Drawing.Color)_pointLayer.Color; }
      set { _pointLayer.Color = value; }
    }


    /// <summary>
    /// Get/Set the primitive size.
    /// </summary>
    [
      System.ComponentModel.Category("Primitive"),
      System.ComponentModel.Description("Size of primitive"),
      System.ComponentModel.Browsable(true),
    ]
    public float Size
    {
      get { return _pointLayer.Size; }
      set { _pointLayer.Size = value; }
    }
  }
}
