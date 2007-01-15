
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
    /// <returns></returns>
    public Strings PolygonTables
    {
      get
      {
        Strings strings = new Strings();

        Strings tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POLYGON" || geomType == "MULTIPOLYGON")
          {
            strings.Add(table);
          }
        }

        return strings;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="table"></param>
    /// <returns></returns>
    public Strings getColumnNames(string table)
    {
      Strings strings = new Strings();

      string query = "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "'";

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        strings.Add((string)dr[0]);
      }

      return strings;
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public Strings LineTables
    {
      get
      {
        Strings strings = new Strings();

        Strings tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POLYGON" || geomType == "MULTIPOLYGON" || geomType == "LINESTRING" || geomType == "MULTILINESTRING")
          {
            strings.Add(table);
          }
        }

        return strings;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    public Strings PointTables
    {
      get
      {
        Strings strings = new Strings();

        Strings tableNames = this.GeometryTables;

        foreach (string table in tableNames)
        {
          string geomType = this._getGeometryType(table);

          if (geomType == "POINT" || geomType == "MULTIPOINT")
          {
            strings.Add(table);
          }
        }

        return strings;
      }
    }


    /// <summary>
    /// Get tables that have point data and a time column.
    /// </summary>
    /// <returns></returns>
    public Strings PointTimeTables
    {
      get
      {
        Strings strings = new Strings();

        // Get point tables.
        Strings tableNames = this.PointTables;

        foreach (string table in tableNames)
        {
          // If this table has a date column...
          if (this.hasColumnType(table, "date"))
          {
            strings.Add(table);
          }
        }

        return strings;
      }
    }


    /// <summary>
    /// Get all tables that have a geometry column.
    /// </summary>
    /// <returns></returns>
    public Strings GeometryTables
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

        return strings;

      }
    }


    public System.Collections.Generic.List<string> getColumnNames(string table, string type)
    {
      System.Collections.Generic.List<string> strings = new System.Collections.Generic.List<string>();
      string query = "SELECT column_name FROM information_schema.columns WHERE table_name='" + table + "' AND data_type='" + type + "'";
      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        strings.Add((string)dr[0]);
      }

      return strings;
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="tableName"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public bool hasColumnType(string tableName, string type)
    {
      Strings columnNames = this.getColumnNames(tableName);

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
    /// <param name="tableName"></param>
    /// <returns></returns>
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
    /// <param name="tablename"></param>
    /// <param name="column"></param>
    /// <returns></returns>
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
    /// <param name="tablename"></param>
    /// <param name="column"></param>
    /// <returns></returns>
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
    /// <param name="tablename"></param>
    /// <param name="column"></param>
    /// <returns></returns>
    public Strings getDistinctValues(string tablename, string column)
    {
      Strings strings = new Strings();

      string query = "SELECT DISTINCT(" + column + ") FROM " + tablename + " ORDER BY " + column;

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      while (dr.Read())
      {
        string value = dr[0].ToString();

        strings.Add(value.Trim());
      }

      return strings;
    }


    /// <summary>
    /// 
    /// </summary>
    public Strings Tables
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

        return strings;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="tableName"></param>
    /// <param name="column"></param>
    /// <returns></returns>
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
