
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.DB
{
  public class Connections
  {
    /// <summary>
    /// Delegates.
    /// </summary>
    public delegate void ConnectionAddedDelegate(DT.Minerva.Interfaces.IDatabaseConnection item);

    private static Connections _instance = null;
    private object _mutex = new object();
    private System.Collections.Generic.List<DT.Minerva.Interfaces.IDatabaseConnection> _connections = new System.Collections.Generic.List<DT.Minerva.Interfaces.IDatabaseConnection>();

    /// <summary>
    /// Constructor.
    /// </summary>
    private Connections()
    {
    }

    /// <summary>
    /// Get the instance.
    /// </summary>
    public static Connections Instance
    {
      get
      {
        lock ("DT.Minerva.Lib.Connections")
        {
          if (null == _instance)
            _instance = new Connections();
          return _instance;
        }
      }
    }


    /// <summary>
    /// Get the list of data sources.
    /// </summary>
    public DT.Minerva.Interfaces.IDatabaseConnection[] OpenConnections
    {
      get { lock (_mutex) { return _connections.ToArray(); } }
    }


    /// <summary>
    /// Add a data source.
    /// </summary>
    public void addConnection ( DT.Minerva.Interfaces.IDatabaseConnection item)
    {
      _connections.Add(item);

      if (null != this.ConnectionAdded)
        this.ConnectionAdded(item);
    }


    /// <summary>
    /// Data source events.
    /// </summary>
    public event ConnectionAddedDelegate ConnectionAdded;
  }
}
