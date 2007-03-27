
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.TypeConverters
{
  public class ColumnNames : System.ComponentModel.StringConverter
  {
    public ColumnNames()
    {
    }

    public override bool GetStandardValuesSupported(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(System.ComponentModel.ITypeDescriptorContext context)
    {
      DT.Minerva.Interfaces.IColumnNames columnNames = context.Instance as DT.Minerva.Interfaces.IColumnNames;
      if (null != columnNames)
        return new StandardValuesCollection(columnNames.ColumnNames);
      return base.GetStandardValues(context);
    }

    public override bool GetStandardValuesExclusive(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }
  }
}
