
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
    string[] getColumnNames(string table);
    string[] getColumnNames(string table, string type);
    string[] getDistinctValues(string tablename, string column);
    string getMaxValue(string tablename, string column);
    string getMinValue(string tablename, string column);
    string getColumnType(string tableName, string column);
    bool hasColumnType(string tableName, string type);
    string[] Tables { get; }
    string[] GeometryTables { get; }
    string[] PointTables { get; }
    string[] PointTimeTables { get; }
    string[] PolygonTables { get; }
    string[] LineTables { get; }
  }
}
