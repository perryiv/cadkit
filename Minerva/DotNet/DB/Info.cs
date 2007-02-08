
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using Strings = System.Collections.Generic.List<string>;

namespace DT.Minerva.DB
{
  public class Info :
    DT.Minerva.DB.Connection,
    DT.Minerva.Interfaces.IDataSource
  {
    public Info()
    {
    }

    /// <summary>
    /// Get tables that contain polygon data.
    /// </summary>
    public string[] PolygonTables
    {
      get
      {
        Strings strings = new Strings();

        string[] tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POLYGON" || geomType == "MULTIPOLYGON")
          {
            strings.Add(table);
          }
        }

        return strings.ToArray();
      }
    }


    /// <summary>
    /// Get all tables that contain line data.
    /// </summary>
    public string[] LineTables
    {
      get
      {
        Strings strings = new Strings();

        string[] tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POLYGON" || geomType == "MULTIPOLYGON" || geomType == "LINESTRING" || geomType == "MULTILINESTRING")
          {
            strings.Add(table);
          }
        }

        return strings.ToArray();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string[] PointTables
    {
      get
      {
        Strings strings = new Strings();

        string[] tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POINT" || geomType == "MULTIPOINT")
          {
            strings.Add(table);
          }
        }

        return strings.ToArray();
      }
    }


    /// <summary>
    /// Get tables that have point data and a time column.
    /// </summary>
    public string[] PointTimeTables
    {
      get
      {
        Strings strings = new Strings();

        // Get point tables.
        string[] tableNames = this.PointTables;

        foreach (string table in tableNames)
        {
          // If this table has a date column...
          if (this.hasColumnType(table, "date"))
          {
            strings.Add(table);
          }
        }

        return strings.ToArray();
      }
    }


    /// <summary>
    /// Get all tables that have a geometry column.
    /// </summary>
    public string[] GeometryTables
    {
      get
      {
        Strings strings = new Strings();

        string query = "SELECT table_name FROM information_schema.columns WHERE column_name='geom'";
        Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        while (dr.Read())
        {
          strings.Add((string)dr["table_name"]);
        }

        return strings.ToArray();

      }
    }


    /// <summary>
    /// Get all the column names of the given table.
    /// </summary>
    public string[] getColumnNames(string table)
    {
      Strings strings = new Strings();

      string query = "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "'";

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        strings.Add((string)dr[0]);
      }

      return strings.ToArray();
    }


    /// <summary>
    /// Get all column names for the given table and type.
    /// </summary>
    public string[] getColumnNames(string table, string type)
    {
      Strings strings = new Strings();
      string query = "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "' AND data_type='" + type + "'";
      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        strings.Add((string)dr[0]);
      }

      return strings.ToArray();
    }


    /// <summary>
    /// Does the table have a column of the given type.
    /// </summary>
    public bool hasColumnType(string tableName, string type)
    {
      string[] columnNames = this.getColumnNames(tableName);

      foreach (string column in columnNames)
      {
        string query = "SELECT data_type FROM information_schema.columns WHERE table_name='" + tableName + "' AND column_name='" + column + "'";

        Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        if (dr.Read())
        {
          string columnType = (string)dr["data_type"];
          columnType = columnType.Trim();
          if (columnType == type)
            return true;
        }
      }

      return false;
    }


    /// <summary>
    /// Get the geometry type from the table name.  Assumes that the table has a column named geom.
    /// </summary>
    protected string _getGeometryType(string tableName)
    {
      string query = "SELECT GeometryType(geom) FROM " + tableName + " WHERE IsEmpty(geom)=false LIMIT 1";

      Npgsql.NpgsqlDataReader geomTypeDR = this.executeQueryDataReader(query);

      if (geomTypeDR.Read())
      {
        if (!geomTypeDR.IsDBNull(0))
        {
          return (string)geomTypeDR[0];
        }
      }

      return "";
    }


    /// <summary>
    /// 
    /// </summary>
    public string getMinValue(string tablename, string column)
    {
      string query = "SELECT MIN(" + column + ") as min FROM " + tablename;

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      if (dr.Read())
        return dr["min"].ToString();

      return "";
    }


    /// <summary>
    /// 
    /// </summary>
    public string getMaxValue(string tablename, string column)
    {
      string query = "SELECT MAX(" + column + ") as max FROM " + tablename;

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      if (dr.Read())
        return dr["max"].ToString();

      return "";
    }


    /// <summary>
    /// 
    /// </summary>
    public string[] getDistinctValues(string tablename, string column)
    {
      Strings strings = new Strings();

      string query = "SELECT DISTINCT(" + column + ") FROM " + tablename + " ORDER BY " + column;

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        string value = dr[0].ToString();

        strings.Add(value.Trim());
      }

      return strings.ToArray();
    }


    /// <summary>
    /// Get all public tables.
    /// </summary>
    public string[] Tables
    {
      get
      {
        Strings strings = new Strings();

        string query = "SELECT table_name FROM information_schema.tables WHERE table_schema='public'";

        Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        while (dr.Read())
        {
          strings.Add((string)dr[0]);
        }

        return strings.ToArray();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string getColumnType(string tableName, string column)
    {
      string query = "SELECT data_type FROM information_schema.columns WHERE table_name='" + tableName + "' AND column_name='" + column + "'";

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      if (dr.Read())
      {
        return dr[0] as string;
      }

      return "";
    }
  }
}
