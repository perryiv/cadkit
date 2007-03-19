
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.TypeConverters
{
  public class DataTables : System.ComponentModel.StringConverter
  {
    public DataTables()
    {
    }

    public override bool GetStandardValuesSupported(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(System.ComponentModel.ITypeDescriptorContext context)
    {
      DT.Minerva.Interfaces.IDataTables dataTables = context.Instance as DT.Minerva.Interfaces.IDataTables;
      if (null != dataTables)
        return new StandardValuesCollection(dataTables.DataTables);
      return base.GetStandardValues(context);
    }

    public override bool GetStandardValuesExclusive(System.ComponentModel.ITypeDescriptorContext context)
    {
      return true;
    }
  }
}
