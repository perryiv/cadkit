 
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
  public partial class BaseLayer : 
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    CadKit.Interfaces.IPropertyGridObject,
    CadKit.Interfaces.ILayer,
    DT.Minerva.Interfaces.IColumnNames,
    DT.Minerva.Interfaces.ILayer
  {
    /// <summary>
    /// Delegates.
    /// </summary>
    public delegate void DataTableDelegate(string tablename);
    public delegate void DataSourceDelegate(DT.Minerva.Interfaces.IDataSource dataSource);

    /// <summary>
    /// Delegates.
    /// </summary>
    protected DataTableDelegate _dataTableChanged;
    protected DataSourceDelegate _dataSourceChanged;


    /// <summary>
    /// Data Members.
    /// </summary>
    private object _mutex = new object();
    private DT.Minerva.Glue.LayerGlue _layer = null;
    private DT.Minerva.Interfaces.IDataSource _dataSource = null;
    private bool _needsUpdate = false;
    private string _name = "";
    private ColorProperties _properties = new ColorProperties();
    private bool _customQuery = false;


    /// <summary>
    /// 
    /// </summary>
    protected BaseLayer()
    {
    }


    /// <summary>
    /// Deserialization constructor.
    /// </summary>
    public BaseLayer(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      this._deserialize(info);
    }


    /// <summary>
    /// Serialize
    /// </summary>
    public void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
      this._serialize(info);
    }


    /// <summary>
    /// 
    /// </summary>
    protected object _getValue(System.Runtime.Serialization.SerializationInfo info, string name, System.Type type)
    {
      object o = null;
      
      try
      {
        o = info.GetValue(name, type);
      }
      catch (System.Runtime.Serialization.SerializationException) 
      { 
      }

      return o;
    }


    /// <summary>
    /// 
    /// </summary>
    protected virtual void _deserialize(System.Runtime.Serialization.SerializationInfo info)
    {
      if (null != this.Layer)
      {
        this.Name = info.GetString("Name");
        this.DataTable = info.GetString("DataTable");
        this.LabelColumn = info.GetString("LabelColumn");
        this.ColorProperties = (ColorProperties)info.GetValue("ColorProperties", typeof(ColorProperties));
        this.Offset = (float)info.GetValue("Offset", typeof(float));
        this.RenderBin = info.GetInt32("RenderBin");
        this.LegendText = info.GetString("LegendText");

        try
        {
          this.CustomQuery = info.GetBoolean("CustomQuery");
        }
        catch (System.Runtime.Serialization.SerializationException)
        {
        }

        try
        {
          this.Query = info.GetString("Query");
        }
        catch (System.Runtime.Serialization.SerializationException)
        {
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected virtual void _serialize(System.Runtime.Serialization.SerializationInfo info)
    {
      info.AddValue("Name", this.Name);
      info.AddValue("DataTable", this.DataTable);
      info.AddValue("LabelColumn", this.LabelColumn);
      info.AddValue("ColorProperties", this.ColorProperties);
      info.AddValue("Offset", this.Offset);
      info.AddValue("RenderBin", this.RenderBin);
      info.AddValue("LegendText", this.LegendText);
      info.AddValue("CustomQuery", this.CustomQuery);
      info.AddValue("Query", this.Query);
    }


    /// <summary>
    /// 
    /// </summary>
    protected BaseLayer(BaseLayer layer)
    {
      this.Layer = layer.Layer;
      this.ColorProperties = new ColorProperties(layer.ColorProperties);
      this.DataSource = layer.DataSource;
      this.Name = layer.Name;
      this.NeedsUpdate = layer.NeedsUpdate;
      this.CustomQuery = layer.CustomQuery;
    }
    

    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new BaseLayer(this);
    }


    /// <summary>
    /// 
    /// </summary>
    System.IntPtr DT.Minerva.Interfaces.ILayer.layerPtr()
    {
        return this.Layer.layerPtr();
    }


    /// <summary>
    /// Get the mutex.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    protected object Mutex
    {
      get { return _mutex; }
    }



    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public bool CustomQuery
    {
      get { return _customQuery; }
      set { _customQuery = value; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Database")
    ]
    public string Query
    {
      get { return this.Layer.Query; }
      set { this.Layer.Query = value; }
    }


    /// <summary>
    /// Get/Set the name.
    /// </summary>
    public string Name
    {
      get { lock (this.Mutex) { return _name; } }
      set { lock (this.Mutex) { _name = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    protected bool Shown
    {
      get { return _layer.ShowLayer; }
      set { _layer.ShowLayer = value; }
    }


    /// <summary>
    /// 
    /// </summary>
    bool CadKit.Interfaces.ILayer.Shown
    {
      get { return this.Shown; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Color"),
      System.ComponentModel.Description("Column to use as the column for gradient color"),
      System.ComponentModel.TypeConverter(typeof(DT.Minerva.Layers.Controls.TypeConverters.ColumnNames))
    ]
    public string ColorColumn
    {
      get { lock (this.Mutex) { return _layer.ColorColumn; } }
      set { lock (this.Mutex) { _layer.ColorColumn = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Color"),
    System.ComponentModel.TypeConverter(typeof(ColorTypeConverter)),
    System.ComponentModel.Editor(typeof(ColorUITypeEditor), typeof(System.Drawing.Design.UITypeEditor))
    ]
    public ColorProperties ColorProperties
    {
      get { lock (this.Mutex) { return _properties; } }
      set { lock (this.Mutex) { _properties = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public DT.Minerva.Glue.LayerGlue Layer
    {
      get { lock (this.Mutex) { return _layer; } }
      set { lock (this.Mutex) { _layer = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Display")
    ]
    public float Offset
    {
      get { lock (this.Mutex) { return _layer.Offset; } }
      set { lock (this.Mutex) { _layer.Offset = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Display")
    ]
    public int RenderBin
    {
      get { lock (this.Mutex) { return _layer.RenderBin; } }
      set { lock (this.Mutex) { _layer.RenderBin = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Display")
    ]
    public string LegendText
    {
      get { lock (this.Mutex) { return _layer.LegendText; } }
      set { lock (this.Mutex) { _layer.LegendText = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Label"),
    System.ComponentModel.Description("Should a label be shown?"),
    System.ComponentModel.Browsable(true),
    ]
    public bool ShowLabel
    {
      get { lock (this.Mutex) { return _layer.ShowLabel; } }
      set { lock (this.Mutex) { _layer.ShowLabel = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Label")
    ]
    public float LabelZOffset
    {
      get { lock (this.Mutex) { return _layer.LabelZOffset; } }
      set { lock (this.Mutex) { _layer.LabelZOffset = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Category("Label")
    ]
    public System.Drawing.Color LabelColor
    {
      get { lock (this.Mutex) { return (System.Drawing.Color) _layer.LabelColor; } }
      set { lock (this.Mutex) { _layer.LabelColor = value; } }
    }


    /// <summary>
    /// Get/Set the data table.
    /// </summary>
    [
      System.ComponentModel.Category("Database"),
      System.ComponentModel.TypeConverter(typeof(DT.Minerva.Layers.Controls.TypeConverters.DataTables))
    ]
    public string DataTable
    {
      get { lock (this.Mutex) { return _layer.Tablename; } }
      set
      {
        lock (this.Mutex)
        {
          _layer.Tablename = value;
          this.LabelColumn = "";

          if (null != this.DataTableChanged)
            this.DataTableChanged(this.DataTable);
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public DataTableDelegate DataTableChanged
    {
      get { lock (this.Mutex) { return _dataTableChanged; } }
      set { lock (this.Mutex) { _dataTableChanged = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
      System.ComponentModel.Category("Label"),
      System.ComponentModel.Description("Column to use as the label"),
      System.ComponentModel.TypeConverter(typeof(DT.Minerva.Layers.Controls.TypeConverters.ColumnNames))
    ]
    public string LabelColumn
    {
      get { lock (this.Mutex) { return _layer.LabelColumn; } }
      set { lock (this.Mutex) { _layer.LabelColumn = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.ReadOnly(true)
    ]
    public bool NeedsUpdate
    {
      get { return _needsUpdate; }
      set { _needsUpdate = value; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public DT.Minerva.Interfaces.IDataSource DataSource
    {
      get { lock (this.Mutex) { return _dataSource; } }
      set 
      { 
        lock (this.Mutex) 
        { 
          _dataSource = value; 

          if(null != this.DataSourceChanged)
            this.DataSourceChanged(_dataSource);
        } 
      }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public DataSourceDelegate DataSourceChanged
    {
      get { lock (this.Mutex) { return _dataSourceChanged; } }
      set { lock (this.Mutex) { _dataSourceChanged = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public virtual string Number
    {
      get { return "NA"; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public virtual string Min
    {
      get { return "NA"; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public virtual string Max
    {
      get { return "NA"; }
    }


    public void setLayerProperties()
    {
      this.Layer.ColorFunctor = this.ColorProperties.colorFunctor(this);
      this._setLayerProperties();
    }

    /// <summary>
    /// 
    /// </summary>
    protected virtual void _setLayerProperties()
    {
    }


    /// <summary>
    /// Update the layer.
    /// </summary>
    public void modify()
    {
      this.setLayerProperties();
      this.NeedsUpdate = false;
    }


    /// <summary>
    /// Update the layer.
    /// </summary>
    void CadKit.Interfaces.ILayer.modify()
    {
      this.setLayerProperties();
      this.NeedsUpdate = false;
    }


    /// <summary>
    /// Hide the layer.
    /// </summary>
    void CadKit.Interfaces.ILayer.hide()
    {
      this.Shown = false;
    }


    /// <summary>
    /// Show the layer.
    /// </summary>
    void CadKit.Interfaces.ILayer.show()
    {
      this.Shown = true;
      this.setLayerProperties();
    }


    /// <summary>
    /// 
    /// </summary>
    void CadKit.Interfaces.ILayer.addToFavorites()
    {
      Factory.Instance.addType(this.Name, this);
    }


    /// <summary>
    /// Get the property grid object.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public object PropertyGridObject
    {
      get { return this; }
    }


    /// <summary>
    /// 
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public string[] ColumnNames
    {
      get
      {
        if(null != this.DataSource && this.DataTable.Length > 0 )
          return this.DataSource.getColumnNames(this.DataTable).ToArray();
        return new string[] { };
      }
    }
  }
}
