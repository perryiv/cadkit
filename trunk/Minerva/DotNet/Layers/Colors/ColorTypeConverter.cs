
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Colors
{
  public class ColorTypeConverter : System.ComponentModel.EnumConverter
  {
    public ColorTypeConverter(System.Type type)
      : base(type)
    {
    }

    public override object ConvertTo(System.ComponentModel.ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, System.Type destinationType)
    {
      DT.Minerva.Interfaces.IColorMode colorMode = context.Instance as DT.Minerva.Interfaces.IColorMode;
      if (null != colorMode)
        return colorMode.ColorMode;
      return "";
    }
  }
}
