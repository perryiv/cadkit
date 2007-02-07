
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
  public partial class GenericLines : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.IDataTables
  {
    private DT.Minerva.Glue.LineLayerGlue _lineLayer = new DT.Minerva.Glue.LineLayerGlue();
    private string _primaryKeyColumn = "id";

    /// <summary>
    /// Constructor.
    /// </summary>
    public GenericLines()
      : base()
    {
      this.Layer = _lineLayer;
      this.Layer.LegendText = "Custom Lines";
    }


    /// <summary>
    /// 
    /// </summary>
    protected GenericLines(GenericLines layer)
      : base(layer)
    {
      this._lineLayer = new DT.Minerva.Glue.LineLayerGlue(layer._lineLayer);
      this.Layer = _lineLayer;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new GenericLines(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public GenericLines(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _lineLayer;
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
      
      this.LineWidth = (float)info.GetValue("LineWidth", typeof(float));
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);

      info.AddValue("LineWidth", this.LineWidth);
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
        return "NA";
      }
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Display"),
      System.ComponentModel.Description("Width for the lines."),
      System.ComponentModel.Browsable(true),
    ]
    public float LineWidth
    {
      get { lock (this.Mutex) { return _lineLayer.LineWidth; } }
      set { lock (this.Mutex) { _lineLayer.LineWidth = value; } }
    }



    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public string PrimaryKeyColumn
    {
      get { return _primaryKeyColumn; }
      set { _primaryKeyColumn = value; }
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
        return "SELECT " + this.PrimaryKeyColumn + " as id, srid(geom) as srid, asBinary(geom) as geom FROM " + this.DataTable;
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
        return this.DataSource.LineTables.ToArray();
      }
    }


    /// <summary>
    /// Set layer properties.
    /// </summary>
    protected override void _setLayerProperties()
    {
      if(!this.CustomQuery)
        this.Layer.Query = this.DefaultQuery;
    }
  }
}
