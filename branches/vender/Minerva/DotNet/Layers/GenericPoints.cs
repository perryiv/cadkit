
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
    System.ICloneable
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
      // Get the index of the selected item
      //int index = _primitives.SelectedIndex + 1;

      //float percent = float.Parse(_size.Value.ToString()) / 100;

      //float size = percent * 5;

      //_pointLayer.RenderBin = this.RenderBin;
      //_pointLayer.Offset = this.Offset;
      //_pointLayer.Tablename = _dataTable;
      //_pointLayer.Size = size;
      //_pointLayer.Color = _colorButton.BackColor;
      //_pointLayer.Query = "SELECT id FROM " + _dataTable;
    }
  }
}
