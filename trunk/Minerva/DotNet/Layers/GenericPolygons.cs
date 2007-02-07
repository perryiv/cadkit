
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
  public partial class GenericPolygons : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.IDataTables
  {
    private DT.Minerva.Glue.PolygonLayerGlue _polygonLayer = new DT.Minerva.Glue.PolygonLayerGlue();


    /// <summary>
    /// Constructor.
    /// </summary>
    public GenericPolygons()
      : base()
    {
      this.Layer = _polygonLayer;

      this.Name = "Custom Polygons";
      this.Layer.LegendText = "Custom Polygons";
    }


    /// <summary>
    /// 
    /// </summary>
    protected GenericPolygons(GenericPolygons layer)
      : base(layer)
    {
      this._polygonLayer = new DT.Minerva.Glue.PolygonLayerGlue(layer._polygonLayer);
      this.Layer = _polygonLayer;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new GenericPolygons(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public GenericPolygons(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _polygonLayer;
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
    protected override void _deserialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._deserialize(info);
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public override string Number
    {
      get
      {
        DT.Minerva.Interfaces.IQuery query = this.DataSource as DT.Minerva.Interfaces.IQuery;
        if (null != query)
        {
          return query.executeCountQuery(this.DataTable, "").ToString();
        }
        return base.Number;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _setLayerProperties()
    {
      if(!this.CustomQuery)
        _polygonLayer.Query = this.DefaultQuery;
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public string DefaultQuery
    {
      get
      {
        string query = "";
        if (null != this.ColorColumn && this.ColorColumn != "")
        {
          query = "SELECT id, " + this.ColorColumn + " FROM " + this.DataTable;
        }
        else
          query = "SELECT id FROM " + this.DataTable;
        return query;
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
        return this.DataSource.PolygonTables.ToArray();
      }
    }
  }
}
