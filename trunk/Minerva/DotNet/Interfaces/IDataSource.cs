
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
    void getMinMaxValue(string query, string column, ref double min, ref double max);
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
