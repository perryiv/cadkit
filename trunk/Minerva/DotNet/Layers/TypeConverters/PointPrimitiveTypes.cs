
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.TypeConverters
{
  public class PointPrimitiveTypes : System.ComponentModel.StringConverter
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
      System.String []types = new System.String [] { "Point", "Sphere", "Cone", "Disk", "Cube", "Inverted Cone" };
      return new StandardValuesCollection ( types );
    }

    public override bool GetStandardValuesExclusive(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }
  }
}
