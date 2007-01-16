
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
  public partial class GenericLines : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.IDataTables
  {
    private DT.Minerva.Glue.LineLayerGlue _lineLayer = new DT.Minerva.Glue.LineLayerGlue();
    private string _where = null;

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
      this._where = layer._where;
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
      this.Where = (string)info.GetString("Where");
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);

      info.AddValue("LineWidth", this.LineWidth);
      info.AddValue("Where", this.Where);
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
          return query.executeCountQuery(this.DataTable, this.Where).ToString();
        }
        return "NA";
      }
    }


    /// <summary>
    /// Return where clause.
    /// </summary>
    public string Where
    {
      get { return _where; }
      set { _where = value; }
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
    public string Query
    {
      get
      {
        string query = "SELECT id FROM " + this.DataTable;

        if (null != this.Where && this.Where.Length > 0)
          query += System.String.Format(" WHERE {0}", this.Where);

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
        return this.DataSource.LineTables.ToArray();
      }
    }


    /// <summary>
    /// Set layer properties.
    /// </summary>
    protected override void _setLayerProperties()
    {
      this.Layer.Query = this.Query;
    }
  }
}
