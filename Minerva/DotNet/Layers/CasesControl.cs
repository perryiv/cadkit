
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
  public partial class CasesControl : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable
  {
    public enum Mode
    {
      ALL,
      HUMAN,
      ALL_MOSQUITOS
    }

    private DT.Minerva.Glue.PointTimeLayerGlue _pointTimeLayer = new DT.Minerva.Glue.PointTimeLayerGlue();
    private System.DateTime _minDate = new System.DateTime();
    private System.DateTime _maxDate = new System.DateTime();
    private Mode _mode = Mode.ALL;

    /// <summary>
    /// Constructor
    /// </summary>
    public CasesControl()
      : base()
    {
      _pointTimeLayer.Format = "%s %s";
      _pointTimeLayer.Columns = "species wnv_date";
      _pointTimeLayer.DateColumn = "wnv_date";

      this.Layer = _pointTimeLayer;
      this.Layer.Tablename = "wnv_allpoints1";

      this.DataSourceChanged += this._dataSourceChangedHandler;

      this.ColorProperties.ColorMode = ColorProperties.Mode.SINGLE_COLOR;
      this.ColorProperties.Color  = System.Drawing.Color.FromArgb(255, 58, 110, 165);

      this.Name = "Cases";
      this.Layer.LegendText = this.Name;
    }


    /// <summary>
    /// 
    /// </summary>
    protected CasesControl(CasesControl layer)
      : base(layer)
    {
      this._pointTimeLayer = new DT.Minerva.Glue.PointTimeLayerGlue(layer._pointTimeLayer);
      this.Layer = this._pointTimeLayer;
      this._mode = layer._mode;
      this._minDate = layer._minDate;
      this._maxDate = layer._maxDate;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new CasesControl(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public CasesControl(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
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

      this.FirstDate = info.GetDateTime("MinDate");
      this.LastDate = info.GetDateTime("MaxDate");
      this.PrimitiveSize = (float)info.GetValue("PrimitiveSize", typeof(float));
      this.PrimitiveType = info.GetString("PrimitiveType");

      try
      {
        this.Species = (Mode)info.GetValue("Species", typeof(Mode));
      }
      catch (System.Runtime.Serialization.SerializationException)
      {
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected override void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      base._serialize(info);

      info.AddValue("MinDate", this.FirstDate);
      info.AddValue("MaxDate", this.LastDate);
      info.AddValue("PrimitiveSize", this.PrimitiveSize);
      info.AddValue("PrimitiveType", this.PrimitiveType);
      info.AddValue("Species", this.Species);
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _dataSourceChangedHandler(DT.Minerva.Interfaces.IDataSource dataSource)
    {
      this.FirstDate = System.DateTime.Parse(this.DataSource.getMinValue("wnv_allpoints1", "wnv_date"));
      this.LastDate = System.DateTime.Parse(this.DataSource.getMaxValue("wnv_allpoints1", "wnv_date"));
    }


    /// <summary>
    /// Get/Set the minimum date to show.
    /// </summary>
    /// 
    [
      System.ComponentModel.Category("Date"),
      System.ComponentModel.Description("First date to show"),
      System.ComponentModel.Browsable(true),
    ]
    public System.DateTime FirstDate
    {
      get { lock (this.Mutex) { return _minDate; } }
      set { lock (this.Mutex) { _minDate = value; this.NeedsUpdate = true; } }
    }


    /// <summary>
    /// Get/Set the maximium date to show.
    /// </summary>
    [
      System.ComponentModel.Category("Date"),
      System.ComponentModel.Description("Last date to show"),
      System.ComponentModel.Browsable(true),
    ]
    public System.DateTime LastDate
    {
      get { lock (this.Mutex) { return _maxDate; } }
      set { lock (this.Mutex) { _maxDate = value; this.NeedsUpdate = true; } }
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
      _pointTimeLayer.DateColumn = "wnv_date";
      _pointTimeLayer.Query = this.Query;
    }


    /// <summary>
    /// 
    /// </summary>
    public Mode Species
    {
      get { lock (this.Mutex) { return _mode; } }
      set { lock (this.Mutex) { _mode = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    private string Where
    {
      get
      {
        string where = "";

        where += "wnv_allpoints1.wnv_date > '" + this.FirstDate.ToShortDateString() + "'";

        where += " AND wnv_allpoints1.wnv_date < '" + this.LastDate.ToShortDateString() + "'";

        if (_mode == Mode.HUMAN)
          where += " AND wnv_allpoints1.species='Human'";
        else if(_mode == Mode.ALL_MOSQUITOS)
          where += " AND wnv_allpoints1.species!='Human'";

        return where;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    private string Query
    {
      get
      {
        string query = "";

        query += "SELECT wnv_allpoints1.wnv_date, wnv_allpoints1.id FROM wnv_allpoints1";
        query += " WHERE " + this.Where;

        return query;
      }
    }


    /// <summary>
    /// Get the minimium date from all species in the database.
    /// </summary>
    public System.String getMinDateSpecies()
    {
      string minDate = this.DataSource.getMinValue("wnv_allpoints1", "wnv_date");
      System.DateTime date = System.DateTime.Parse(minDate);
      return date.ToShortDateString();
    }


    /// <summary>
    /// Get the maximium date from all species in the database.
    /// </summary>
    public System.String getMaxDateSpecies()
    {
      string maxDate = this.DataSource.getMaxValue("wnv_allpoints1", "wnv_date");
      System.DateTime date = System.DateTime.Parse(maxDate);
      return date.ToShortDateString();
    }


    /// <summary>
    /// Get the number in the current query?
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
          string where = this.Where;
          return query.executeCountQuery(this.Layer.Tablename, where).ToString();
        }
        return base.Number;
      }
    }
  }
}
