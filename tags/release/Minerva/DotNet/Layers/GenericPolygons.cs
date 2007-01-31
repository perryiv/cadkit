
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
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

      this.Format = info.GetString("Format");
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);

      info.AddValue("Format", this.Format);
    }


    /// <summary>
    /// 
    /// </summary>
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
      _polygonLayer.FieldName = this.ColumnName;
      _polygonLayer.Query = this.Query;
    }


    /// <summary>
    /// 
    /// </summary>
    public string Query
    {
      get
      {
        string query = "";
        if (null != this.ColumnName && this.ColumnName != "")
        {
          query = "SELECT id, " + this.ColumnName + " FROM " + this.DataTable;
        }
        else
          query = "SELECT id FROM " + this.DataTable;
        return query;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string Format
    {
      get { lock (this.Mutex) { return _polygonLayer.Format; } }
      set { lock (this.Mutex) { _polygonLayer.Format = value; } }
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
