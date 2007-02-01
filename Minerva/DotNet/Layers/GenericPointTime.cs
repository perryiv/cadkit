
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
  public partial class GenericPointTime : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.ITimeSeries,
    DT.Minerva.Interfaces.IDataTables
  {
    private DT.Minerva.Glue.PointTimeLayerGlue _pointTimeLayer = new DT.Minerva.Glue.PointTimeLayerGlue();
    private System.DateTime _minDate = new System.DateTime();
    private System.DateTime _maxDate = new System.DateTime();


    /// <summary>
    /// Constructor.
    /// </summary>
    public GenericPointTime() : base()
    {
      _pointTimeLayer.Format = "%s";
      this.Layer = _pointTimeLayer;
    }


    /// <summary>
    /// Copy contructor.
    /// </summary>
    protected GenericPointTime(GenericPointTime layer)
      : base(layer)
    {
      this._pointTimeLayer = new DT.Minerva.Glue.PointTimeLayerGlue(layer._pointTimeLayer);
      this.Layer = _pointTimeLayer;
      this._minDate = layer.MinDate;
      this._maxDate = layer.MaxDate;
    }


    /// <summary>
    /// Clone.
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new GenericPointTime(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public GenericPointTime(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _pointTimeLayer;
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

      this.MinDate = info.GetDateTime("MinDate");
      this.MaxDate = info.GetDateTime("MaxDate");
      this.DateColumn = info.GetString("DateColumn");
      this.PrimitiveSize = (float)info.GetValue("PrimitiveSize", typeof(float));
      this.PrimitiveType = info.GetString("PrimitiveType");
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);

      info.AddValue("MinDate", this.MinDate);
      info.AddValue("MaxDate", this.MaxDate);
      info.AddValue("DateColumn", this.DateColumn);
      info.AddValue("PrimitiveSize", this.PrimitiveSize);
      info.AddValue("PrimitiveType", this.PrimitiveType);
    }


    /// <summary>
    /// Get/Set the minimum date to show.
    /// </summary>
    /// 
    [
      System.ComponentModel.Category("Date"),
      System.ComponentModel.Description("Minimum date to show"),
      System.ComponentModel.Browsable(true)
    ]
    public System.DateTime MinDate
    {
      get { lock (this.Mutex) { return _minDate; } }
      set { lock (this.Mutex) { _minDate = value; this.NeedsUpdate = true; } }
    }


    /// <summary>
    /// Get/Set the maximium date to show.
    /// </summary>
    [
      System.ComponentModel.Category("Date"),
      System.ComponentModel.Description("Maximium date to show"),
      System.ComponentModel.Browsable(true)
    ]
    public System.DateTime MaxDate
    {
      get { lock (this.Mutex) { return _maxDate; } }
      set { lock (this.Mutex) { _maxDate = value; this.NeedsUpdate = true; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Date"),
      System.ComponentModel.Description("Column that contains the date."),
      System.ComponentModel.Browsable(true)
    ]
    public string DateColumn
    {
      get { lock (this.Mutex) { return _pointTimeLayer.DateColumn; } }
      set { lock (this.Mutex) { _pointTimeLayer.DateColumn = value; } }
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
      get { lock (this.Mutex) { return _pointTimeLayer.Size; } }
      set { lock (this.Mutex) { _pointTimeLayer.Size = value; } }
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
      get { lock (this.Mutex) { return _pointTimeLayer.PrimitiveType; } }
      set { lock (this.Mutex) { _pointTimeLayer.PrimitiveType = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _setLayerProperties()
    {
      _pointTimeLayer.Query = this.Query;
    }


    /// <summary>
    /// 
    /// </summary>
    public string Query
    {
      get
      {
        lock (this.Mutex)
        {
          string query = "SELECT " + this.DateColumn + ", id FROM " + this.DataTable;
          if (this.Where.Length > 0)
            query += " WHERE " + this.Where;
          return query;
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private string Where
    {
      get
      {
        lock (this.Mutex)
        {
          string where = "";

          if (this.DateColumn.Length > 0)
          {
            where += this.DateColumn + " > '" + this.MinDate.ToShortDateString() + "'";
            where += " AND " + this.DateColumn + " < '" + this.MaxDate.ToShortDateString() + "'";
          }
          return where;
        }
      }
    }


    /// <summary>
    /// Get the minimium date from date_column in the database.
    /// </summary>
    public System.String getMinDate(string datatable, string dateColumn)
    {
      string minDate = this.DataSource.getMinValue(datatable, dateColumn);
      System.DateTime date = System.DateTime.Parse(minDate);
      return date.ToShortDateString();
    }


    /// <summary>
    /// Get the maximium date from date_column in the database.
    /// </summary>
    public System.String getMaxDate(string datatable, string dateColumn)
    {
      string maxDate = this.DataSource.getMaxValue(datatable, dateColumn);
      System.DateTime date = System.DateTime.Parse(maxDate);
      return date.ToShortDateString();
    }


    /// <summary>
    /// 
    /// </summary>
    void DT.Minerva.Interfaces.ITimeSeries.showGraph()
    {
      try
      {
        this._showGraph();
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 3911622368: exception caught while trying to show graph.");
        System.Console.WriteLine("Message: {0}", ex.Message);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _showGraph()
    {
      //string query = "SELECT count(wnv_date), wnv_date FROM wnv_allpoints1 WHERE " + this.Where + " GROUP BY wnv_date ORDER BY wnv_date";
      //DT.Minerva.Interfaces.IQuery iQuery = this.DataSource as DT.Minerva.Interfaces.IQuery;
      //if (null != iQuery)
      //{

      //  System.Data.IDataReader dr = iQuery.execteQueryIDataReader(query);
      //  System.Collections.Generic.Dictionary<System.DateTime, int> map = new System.Collections.Generic.Dictionary<System.DateTime, int>();
      //  while (dr.Read())
      //  {
      //    System.Int64 count = (System.Int64)dr[0];
      //    System.DateTime date = (System.DateTime)dr[1];
      //    map.Add(date, (int)count);
      //  }

      //  CadKit.Graph.ZedGraphForm graph = new CadKit.Graph.ZedGraphForm();

      //  ZedGraph.GraphPane pane = graph.GraphPane;

      //  // Set the titles and axis labels
      //  pane.Title.Text = "Cases time series graph";
      //  pane.XAxis.Title.Text = "Date";
      //  pane.YAxis.Title.Text = "Number of Occurences";

      //  ZedGraph.PointPairList points = new ZedGraph.PointPairList();

      //  System.DateTime current = this.MinDate;
      //  System.DateTime max = this.MaxDate;

      //  while (current < max)
      //  {
      //    int count = 0;
      //    for (int i = 0; i < 7; ++i)
      //    {
      //      if (map.ContainsKey(current))
      //        count += map[current];

      //      current = current.AddDays(1.0);
      //    }
      //    double x = (double)new ZedGraph.XDate(current);
      //    double y = (double)count;
      //    points.Add(x, y);
      //  }

      //  pane.AddCurve("time series", points, System.Drawing.Color.Black, ZedGraph.SymbolType.None);
      //  pane.XAxis.Type = ZedGraph.AxisType.Date;
      //  graph.ZedGraphControl.AxisChange();

      //  graph.Show();
      //}
    }


    /// <summary>
    /// 
    /// </summary>
    string[] DT.Minerva.Interfaces.IDataTables.DataTables
    {
      get
      {
        lock (this.Mutex)
        {
          return this.DataSource.PointTimeTables.ToArray();
        }
      }
    }
  }
}
