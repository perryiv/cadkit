
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
  public class GenericPolygonTime : 
    GenericPolygons,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable
  {
    private DT.Minerva.Glue.PolygonTimeLayerGlue _polygonTimeLayerGlue = new DT.Minerva.Glue.PolygonTimeLayerGlue();


    /// <summary>
    /// Constructor.
    /// </summary>
    public GenericPolygonTime()
      : base()
    {
      this.Layer = _polygonTimeLayerGlue;
    }


    /// <summary>
    /// 
    /// </summary>
    protected GenericPolygonTime(GenericPolygonTime layer)
      : base(layer)
    {
      this._polygonTimeLayerGlue = new DT.Minerva.Glue.PolygonTimeLayerGlue(layer._polygonTimeLayerGlue);
      this.Layer = _polygonTimeLayerGlue;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new GenericPolygonTime(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public GenericPolygonTime(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _polygonTimeLayerGlue;
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
    /// 
    /// </summary>
    protected override void _setLayerProperties()
    {
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public new string DefaultQuery
    {
      get 
      {
        lock (this.Mutex)
        {
          string query = "";
          if (null != this.LabelColumn && this.LabelColumn != "")
          {
            query = "SELECT id, " + this.LabelColumn + ", " + this.StepColumn + ", srid(geom) as srid, asBinary(geom) as geom FROM " + this.DataTable;
          }
          else
            query = base.DefaultQuery;

          return query;
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string StepColumn
    {
      get { lock (this.Mutex) { return _polygonTimeLayerGlue.StepColumn; } }
      set { lock (this.Mutex) { _polygonTimeLayerGlue.StepColumn = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    public DT.Minerva.Glue.PolygonTimeLayerGlue.Mode StepMode
    {
      get { lock (this.Mutex) { return _polygonTimeLayerGlue.StepMode; } }
      set { lock (this.Mutex) { _polygonTimeLayerGlue.StepMode = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    public float ZScaleFactor
    {
      get { lock (this.Mutex) { return _polygonTimeLayerGlue.ZScaleFactor; } }
      set { lock (this.Mutex) { _polygonTimeLayerGlue.ZScaleFactor = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    public float XOffset
    {
      get { lock (this.Mutex) { return _polygonTimeLayerGlue.XOffset; } }
      set { lock (this.Mutex) { _polygonTimeLayerGlue.XOffset = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    public float YOffset
    {
      get { lock (this.Mutex) { return _polygonTimeLayerGlue.YOffset; } }
      set { lock (this.Mutex) { _polygonTimeLayerGlue.YOffset = value; } }
    }

    /// <summary>
    /// Get the minimium date from date_column in the database.
    /// </summary>
    public string getMinDate(string datatable, string dateColumn)
    {
      string minDate = this.DataSource.getMinValue(datatable, dateColumn);
      System.DateTime date = System.DateTime.Parse(minDate);
      return date.Month + "/" + date.Day + "/" + date.Year;
    }


    /// <summary>
    /// Get the maximium date from date_column in the database.
    /// </summary>
    public string getMaxDate(string datatable, string dateColumn)
    {
      string maxDate = this.DataSource.getMaxValue(datatable, dateColumn);
      System.DateTime date = System.DateTime.Parse(maxDate);
      return date.Month + "/" + date.Day + "/" + date.Year;
    }
  }
}
