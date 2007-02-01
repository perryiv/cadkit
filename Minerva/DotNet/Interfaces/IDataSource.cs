
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Interfaces
{
  public interface IDataSource
  {
    System.Collections.Generic.List<string> getColumnNames(string table);
    System.Collections.Generic.List<string> getColumnNames(string table, string type);
    System.Collections.Generic.List<string> getDistinctValues(string tablename, string column);
    string getMaxValue(string tablename, string column);
    string getMinValue(string tablename, string column);
    string getColumnType(string tableName, string column);
    bool hasColumnType(string tableName, string type);
    System.Collections.Generic.List<string> Tables { get; }
    System.Collections.Generic.List<string> GeometryTables { get; }
    System.Collections.Generic.List<string> PointTables { get; }
    System.Collections.Generic.List<string> PointTimeTables { get; }
    System.Collections.Generic.List<string> PolygonTables { get; }
    System.Collections.Generic.List<string> LineTables { get; }
  }
}
