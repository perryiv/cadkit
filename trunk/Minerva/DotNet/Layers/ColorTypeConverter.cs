
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls
{
  public class ColorTypeConverter : System.ComponentModel.EnumConverter
  {
    public ColorTypeConverter(System.Type type)
      : base(type)
    {
    }

    public override object ConvertTo(System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, System.Type destinationType)
    {
      BaseLayer layer = context.Instance as BaseLayer;
      if (null != layer)
        return layer.ColorProperties.ColorMode.ToString();
      return "";
    }
  }
}
