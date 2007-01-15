
namespace DT.Minerva.DB
{
  public class Connection : 
    DT.Minerva.Interfaces.IDatabaseConnection, 
    DT.Minerva.Interfaces.IQuery
  {
    private Npgsql.NpgsqlConnection _connection = null;
    private string _hostname;
    private string _database;
    private string _username;
    private string _password;

    /// <summary>
    /// Constructor.
    /// </summary>
    public Connection()
    {
    }


    /// <summary>
    /// Dispose.
    /// </summary>
    public void Dispose()
    {
      if (null != _connection)
        _connection.Close();
    }


    /// <summary>
    /// Get/Set the hostname.
    /// </summary>
    public string Hostname
    {
      get
      {
        return _hostname;
      }
      set
      {
        _hostname = value;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string Database
    {
      get
      {
        return _database;
      }
      set
      {
        _database = value;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string Username
    {
      get
      {
        return _username;
      }
      set
      {
        _username = value;
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string Password
    {
      get
      {
        return _password;
      }
      set
      {
        _password = value;
      }
    }


    /// <summary>
    /// Connect to the server.
    /// </summary>
    public void connect()
    {
      string connection = "";
      connection += "Server=" + _hostname + ";";
      connection += "Port=5432;";
      connection += "User Id=" + _username + ";";
      connection += "Password=" + _password + ";";
      connection += "Database=" + _database + ";";
      _connection = new Npgsql.NpgsqlConnection(connection);
      _connection.Open();
    }

    /// <summary>
    /// Disconnect.
    /// </summary>
    public void disconnect()
    {
      _connection.Close();
    }

    /// <summary>
    /// Execute the query.
    /// </summary>
    public void executeQuery(System.String query)
    {
      Npgsql.NpgsqlCommand command = new Npgsql.NpgsqlCommand(query, _connection);

      System.Int32 rows = command.ExecuteNonQuery();
    }

    /// <summary>
    /// Get the maximum id from the given table.
    /// </summary>
    private System.Int32 _getMaxId(System.String table)
    {
      System.String query = "SELECT MAX(id) as id FROM " + table;

      Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

      if (dr.Read())
      {
        return (System.Int32)dr["id"];
      }

      throw new System.Exception("Error 1172716358: Could not get id from the data base.");
    }


    /// <summary>
    /// Execute the query and return a data reader.
    /// </summary>
    public Npgsql.NpgsqlDataReader executeQueryDataReader(string query)
    {
      Npgsql.NpgsqlCommand command = new Npgsql.NpgsqlCommand(query, _connection);

      Npgsql.NpgsqlDataReader dr = command.ExecuteReader();

      return dr;
    }


    /// <summary>
    /// 
    /// </summary>
    System.Data.IDataReader DT.Minerva.Interfaces.IQuery.execteQueryIDataReader(string query)
    {
      return this.executeQueryDataReader(query);
    }


    /// <summary>
    /// Get count from table.
    /// </summary>
    long DT.Minerva.Interfaces.IQuery.executeCountQuery(string table, string where)
    {
      string query = "SELECT COUNT(*) FROM " + table;

      if (null != where && where != "")
        query += " WHERE " + where;

      Npgsql.NpgsqlCommand command = new Npgsql.NpgsqlCommand(query, _connection);
      return (long)command.ExecuteScalar();
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="table"></param>
    /// <returns></returns>
    public System.Int64 getRowCount(System.String table)
    {
      System.String query = "SELECT COUNT(*) FROM " + table;
      Npgsql.NpgsqlCommand command = new Npgsql.NpgsqlCommand(query, _connection);
      return (System.Int64)command.ExecuteScalar();
    }


    /// <summary>
    /// 
    /// </summary>
    public System.Int32 executeInsertQuery(System.String tableName, System.String[][] namesAndValues)
    {
      if (namesAndValues.Length == 0)
        throw new System.Exception("Error 1978842812: No names or values in the query");

      System.String names = "( ";
      System.String values = " VALUES ( ";

      for (int i = 0; i < namesAndValues.Length; ++i)
      {
        names += namesAndValues[i][0];

        values += " '" + namesAndValues[i][1] + "' ";

        if (i != namesAndValues.Length - 1)
        {
          names += ",";
          values += ",";
        }
      }

      names += " )";
      values += " )";

      System.String query = "INSERT INTO " + tableName + names + values;

      this.executeQuery(query);

      return this._getMaxId(tableName);
    }
  }
}
