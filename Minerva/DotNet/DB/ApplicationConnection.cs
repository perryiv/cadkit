
///////////////////////////////////////////////////////////////////////////////
//
//  Class that talks to the database.  I think this class could eventually be
//  turned into a web service.
//
///////////////////////////////////////////////////////////////////////////////



namespace WNV
{
  using Strings = System.Collections.Generic.List<string>;

  namespace DB
  {
    /// <summary>
    /// 
    /// </summary>
    public class ApplicationConnection : Connection
    {
      private object _mutex = new object();
      //private static ApplicationConnection _instance = null;
      private System.Int32 _sessionId = 0;

      private WNV.Glue.Controller _controller = new WNV.Glue.Controller();

      /// <summary>
      /// Constructor.
      /// </summary>
      public ApplicationConnection()
      {
      }


      /// <summary>
      /// Destructor.
      /// </summary>
      ~ApplicationConnection()
      {
        _controller.disconnect();
      }


      /// <summary>
      /// 
      /// </summary>
      public override void connect()
      {
        base.connect();
        _controller.connect(this.hostname(), this.database(), this.username(), this.password());
      }


      /// <summary>
      /// 
      /// </summary>
      public override void disconnect()
      {
        _controller.disconnect();
        base.disconnect();
      }


      /// <summary>
      /// Connect to a running session, or create it.
      /// </summary>
      /// <param name="name"></param>
      public void connectToSession(System.String name)
      {
        _sessionId = _controller.connectToSession(name);
        //System.String query = "SELECT * FROM wnv_sessions WHERE name = '" + name + "'";

        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //if (dr.Read())
        //{
        //  _sessionId = (System.Int32)dr["id"];
        //}
        //else
        //{
        //  // We need to create a new session.
        //  System.String[][] values = new System.String[1][];
        //  values[0] = new System.String[] { "name", name };

        //  _sessionId = (System.Int32) this.executeInsertQuery("wnv_sessions", values);
        //}
      }


      /// <summary>
      /// 
      /// </summary>
      public void deleteSession()
      {
        _controller.deleteSession();
        //this._clearTable("wnv_layers");
        //this._clearTable("wnv_event_table");
        //this._clearTable("wnv_cases_criteria");
        //this._clearTable("wnv_draw_line_data");
        //this._clearTable("wnv_polygon_criteria");
        //this._clearTable("wnv_animate_table");
      }


      /// <summary>
      /// 
      /// </summary>
      /// <returns></returns>
      public Strings getAvailableSessions()
      {
        return _controller.getAvailableSessions();
        //System.String query = "SELECT * FROM wnv_sessions ORDER BY name";

        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //Strings sessions = new Strings();

        //while (dr.Read())
        //{
        //  sessions.Add((System.String)dr["name"]);
        //}

        //return sessions;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="tableName"></param>
      private void _clearTable(System.String tableName)
      {
        System.String query = "DELETE FROM " + tableName + " WHERE session_id = " + _sessionId;

        this.executeQuery(query);
      }


      /// <summary>
      /// Build a query for the draw command table
      /// </summary>
      private System.Int32 executeLayerQuery(WNV.Layers.LayerInfo layerInfo)
      {
        System.String[][] values = new System.String[8][];
        values[0] = new System.String[] { "criteria_id",   layerInfo.criteriaID().ToString() };
        values[1] = new System.String[] { "type",          layerInfo.type().ToString() };
        values[2] = new System.String[] { "session_id",    _sessionId.ToString() };
        values[3] = new System.String[] { "hostname",      layerInfo.hostname() };
        values[4] = new System.String[] { "database_name", layerInfo.database() };
        values[5] = new System.String[] { "table_name",    layerInfo.tablename() };
        values[6] = new System.String[] { "render_bin",    layerInfo.renderBin().ToString() };
        values[7] = new System.String[] { "label_column",  layerInfo.labelColumn() };

        return this.executeInsertQuery("wnv_layers", values);
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="layerID"></param>
      /// <returns></returns>
      public int getCriteriaID(int layerID)
      {
        return _controller.getCriteriaID(layerID);
        //string query = "SELECT criteria_id FROM wnv_draw_commands WHERE id = " + layerID + " AND session_id = " + _sessionId;

        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //if (dr.Read())
        //{
        //  return (int)dr["criteria_id"];
        //}

        //return -1;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="layerID"></param>
      /// <returns></returns>
      public string getLayerTable(int layerID)
      {
        return _controller.getLayerTable(layerID);
        //string query = "SELECT wnv_layer_types.criteria_table_name FROM wnv_layers, wnv_draw_command_types WHERE wnv_layers.id = "
        //  + layerID + " AND wnv_layers.session_id = " + _sessionId + " AND wnv_layers.type = wnv_layer_types.id";

        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //if (dr.Read())
        //{
        //  return (string)dr[0];
        //}

        //return "";
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="type"></param>
      /// <param name="eventId"></param>
      public void executeEventTableQuery( int type, int eventId)
      {
        _controller.executeEventTableQuery(type, eventId);
        //System.String[][] values = new System.String[3][];
        //values[0] = new System.String[] { "type", type.ToString() };
        //values[1] = new System.String[] { "event_id", eventId.ToString() };
        //values[2] = new System.String[] { "session_id", _sessionId.ToString() };

        //this.executeInsertQuery("wnv_event_table", values);
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="layerID"></param>
      public void removeLayer(int layerID)
      {
        _controller.removeLayer(layerID);
        //this.executeEventTableQuery(3, layerID);
      }


      /// <summary>
      /// 
      /// </summary>
      /// <returns></returns>
      public Strings getPointPrimitiveTypes()
      {
        return _controller.getPointPrimitiveTypes();
        //Strings strings = new Strings();

        //System.String query = "SELECT id,primitive_type FROM wnv_point_data_primitive_types";
        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //while (dr.Read())
        //{
        //  /// Get data from database.
        //  System.Int32 id = (System.Int32)dr["id"];
        //  System.String type = (System.String)dr["primitive_type"];

        //  strings.Add( id.ToString() );
        //  strings.Add( type );
        //}

        //return strings;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="color"></param>
      /// <param name="index"></param>
      /// <param name="where"></param>
      /// <returns></returns>
      public int showPointTime(WNV.Layers.LayerInfo layerInfo, string color, int index, float size, System.String query)
      {
        return _controller.showPointTime(layerInfo, color, index, size, query);
        //System.String[][] values = new System.String[7][];
        //values[0] = new System.String[] { "the_query", query };
        //values[1] = new System.String[] { "material_color", color };
        //values[2] = new System.String[] { "primitive_type", index.ToString() };
        //values[3] = new System.String[] { "primitive_size", size.ToString() };
        //values[4] = new System.String[] { "session_id", _sessionId.ToString() };
        //values[5] = new System.String[] { "tooltip_format", "Speices: %s Date: %s" };
        //values[6] = new System.String[] { "tooltip_columns", "species wnv_date" };

        //System.Int32 criteriaId = this.executeInsertQuery( "wnv_cases_criteria", values);

        //layerInfo.type(1);
        //layerInfo.criteriaID(criteriaId);

        //System.Int32 eventId = this.executeLayerQuery(layerInfo);

        //this.executeEventTableQuery(1, eventId);

        //return eventId;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="color"></param>
      /// <param name="where"></param>
      /// <param name="dataTable"></param>
      /// <returns></returns>
      public int showLineData(WNV.Layers.LayerInfo layerInfo, string color, string where, float lineWidth)
      {
        return _controller.showLineData(layerInfo, color, where, lineWidth);
        //System.String[][] values = new System.String[4][];
        //values[0] = new System.String[] { "color", color };
        //values[1] = new System.String[] { "session_id", _sessionId.ToString() };
        //values[2] = new System.String[] { "where_clause", where };
        //values[3] = new System.String[] { "line_width", lineWidth.ToString() };

        //System.Int32 criteriaId = this.executeInsertQuery("wnv_draw_line_data", values);

        //layerInfo.type(3);
        //layerInfo.criteriaID(criteriaId);

        //System.Int32 eventId = this.executeLayerQuery(layerInfo);

        //this.executeEventTableQuery(1, eventId);

        //return eventId;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="dataTable"></param>
      /// <param name="fieldName"></param>
      /// <param name="type"></param>
      /// <param name="minColor"></param>
      /// <param name="maxColor"></param>
      /// <param name="where"></param>
      /// <returns></returns>
      public int showPolygonData(WNV.Layers.LayerInfo layerInfo, string fieldName, int type, string minColor, string maxColor, string where, string tooltip)
      {
        return _controller.showPolygonData(layerInfo, fieldName, minColor, maxColor, where, tooltip);
        //System.String[][] values = new System.String[6][];
        //values[0] = new System.String[] { "min_color", minColor };
        //values[1] = new System.String[] { "max_color", maxColor };
        //values[2] = new System.String[] { "session_id", _sessionId.ToString() };
        //values[3] = new System.String[] { "field_name", fieldName };
        //values[4] = new System.String[] { "where_clause", where };
        //values[5] = new System.String[] { "tooltip_format", tooltip };

        //System.Int32 criteriaId = this.executeInsertQuery("wnv_polygon_criteria", values);

        //layerInfo.type(type);
        //layerInfo.criteriaID(criteriaId);

        //System.Int32 eventId = this.executeLayerQuery(layerInfo);

        //this.executeEventTableQuery(1, eventId);

        //return eventId;
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="minColor"></param>
      /// <param name="maxColor"></param>
      /// <param name="where"></param>
      /// <param name="layerID"></param>
      public void modifyPolygonData(string fieldName, string minColor, string maxColor, string where, int layerID)
      {
        _controller.modifyPolygonData(fieldName, minColor, maxColor, where, layerID);
        //int criteriaID = this.getCriteriaID(layerID);
        //string tableName = this.getLayerTable(layerID);

        //string query = "UPDATE " + tableName +
        //  " SET min_color='" + minColor +
        //    "', max_color='" + maxColor +
        //    "', field_name='" + fieldName +
        //    "' WHERE session_id = " + _sessionId;

        //this.executeQuery(query);

        //this.executeEventTableQuery(1, layerID);
      }


      /// <summary>
      /// 
      /// </summary>
      /// <param name="dataTable"></param>
      /// <param name="size"></param>
      /// <param name="type"></param>
      /// <param name="color"></param>
      /// <returns></returns>
      public int showPointData(WNV.Layers.LayerInfo layerInfo, float size, int primitive_type, int layer_type, string color)
      {
        return _controller.showPointData(layerInfo, size, primitive_type,layer_type, color);
        //System.String[][] values = new System.String[4][];
        //values[0] = new System.String[] { "material_color", color };
        //values[1] = new System.String[] { "session_id", _sessionId.ToString() };
        //values[2] = new System.String[] { "primitive_type", primitive_type.ToString() };
        //values[3] = new System.String[] { "primitive_size", size.ToString() };

        //System.Int32 criteriaId = this.executeInsertQuery("wnv_point_data", values);

        //layerInfo.type(layer_type);
        //layerInfo.criteriaID(criteriaId);

        //System.Int32 eventId = this.executeLayerQuery(layerInfo);

        //this.executeEventTableQuery(1, eventId);

        //return eventId;
      }



      /// <summary>
      /// Start animation.
      /// </summary>
      public void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays)
      {
        _controller.startAnimation(speed, accumulate, dateTimeStep, timeWindow, numDays);
        //System.String[][] values = new System.String[7][];
        //values[0] = new System.String[] { "animate", true.ToString() };
        //values[1] = new System.String[] { "session_id", _sessionId.ToString() };
        //values[2] = new System.String[] { "speed", speed.ToString() };
        //values[3] = new System.String[] { "accumulate", accumulate.ToString() };
        //values[4] = new System.String[] { "date_time_step", dateTimeStep.ToString() };
        //values[5] = new System.String[] { "time_window", timeWindow.ToString() };
        //values[6] = new System.String[] { "num_days_to_show", numDays.ToString() };

        //System.Int32 eventId = this.executeInsertQuery("wnv_animate_table", values);

        //this.executeEventTableQuery(2, eventId);
      }


      /// <summary>
      /// Stop Animation.
      /// </summary>
      public void stopAnimation()
      {
        _controller.stopAnimation();
        //System.String[][] values = new System.String[2][];
        //values[0] = new System.String[] { "animate", false.ToString() };
        //values[1] = new System.String[] { "session_id", _sessionId.ToString() };

        //System.Int32 eventId = this.executeInsertQuery("wnv_animate_table", values);

        //this.executeEventTableQuery(2, eventId);
      }


      /// <summary>
      /// 
      /// </summary>
      /// <returns></returns>
      public Strings getLayerTypes()
      {
        return _controller.getLayerTypes();
        //Strings commands = new Strings();
        //System.String query = "SELECT * FROM wnv_layer_types ORDER BY type";
        //Npgsql.NpgsqlDataReader dr = this.executeQueryDataReader(query);

        //while (dr.Read())
        //{
        //  commands.Add((string)dr["type"]);
        //}

        //return commands;
      }
    }
  }
}
