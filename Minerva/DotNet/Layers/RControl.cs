
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
  public partial class RControl : 
    BaseLayer,
    System.Runtime.Serialization.ISerializable,
    System.ICloneable,
    DT.Minerva.Interfaces.IDataTables
  {
    DT.Minerva.Glue.RLayerGlue _layer = new DT.Minerva.Glue.RLayerGlue();

    /// <summary>
    /// 
    /// </summary>
    public RControl()
      : base()
    {
      this.Layer = _layer;
      this.Name = "Cluster";
    }


    /// <summary>
    /// 
    /// </summary>
    protected RControl(RControl layer)
      : base(layer)
    {
      _layer = new DT.Minerva.Glue.RLayerGlue(layer._layer);
      this.Layer = _layer;
    }


    /// <summary>
    /// 
    /// </summary>
    object System.ICloneable.Clone()
    {
      return new RControl(this);
    }


    /// <summary>
    /// 
    /// </summary>
    public RControl(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
      : base()
    {
      this.Layer = _layer;
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

      try
      {
        this.DrawMode = (DT.Minerva.Glue.RLayerGlue.Mode)info.GetValue("DrawMode", typeof(DT.Minerva.Glue.RLayerGlue.Mode));
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

      info.AddValue("DrawMode", this.DrawMode);
    }


    /// <summary>
    /// 
    /// </summary>
    public DT.Minerva.Glue.RLayerGlue.Mode DrawMode
    {
      get { lock (this.Mutex) { return _layer.DrawMode; } }
      set { lock (this.Mutex) { _layer.DrawMode = value; } }
    }


    /// <summary>
    /// 
    /// </summary>
    string[] DT.Minerva.Interfaces.IDataTables.DataTables
    {
      get { lock (this.Mutex) { return this.DataSource.Tables; } }
    }
  }
}
