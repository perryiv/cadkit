
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls.TypeConverters
{
  class PointPrimitiveTypes : System.ComponentModel.StringConverter
  {
    public PointPrimitiveTypes()
    {
    }

    public override bool GetStandardValuesSupported(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(System.ComponentModel.ITypeDescriptorContext context)
    {
      return new StandardValuesCollection ( DT.Minerva.Glue.PointLayerGlue.getPointPrimitiveTypes().ToArray() );
    }

    public override bool GetStandardValuesExclusive(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }
  }
}
