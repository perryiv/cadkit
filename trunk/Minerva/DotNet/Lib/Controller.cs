
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Lib
{
  public class Controller
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public class Connections : System.Collections.Generic.List<DT.Minerva.Interfaces.IDatabaseConnection> { }

    /// <summary>
    /// Delegates.
    /// </summary>
    public delegate void DataSourceAddedDelegate(DT.Minerva.Interfaces.IDatabaseConnection item);

    private static Controller _instance = null;
    private object _mutex = new object();
    private Connections _connections = new Connections();

    /// <summary>
    /// Constructor.
    /// </summary>
    private Controller()
    {
    }

    /// <summary>
    /// Get the instance.
    /// </summary>
    public static Controller Instance
    {
      get
      {
        lock ("DT.Minerva.Lib.Connections")
        {
          if (null == _instance)
            _instance = new Controller();
          return _instance;
        }
      }
    }


    /// <summary>
    /// Get the list of data sources.
    /// </summary>
    public Connections DataSources
    {
      get { lock (_mutex) { return _connections; } }
    }


    /// <summary>
    /// Add a data source.
    /// </summary>
    public void addDataSource ( DT.Minerva.Interfaces.IDatabaseConnection item)
    {
      _connections.Add(item);

      if(null!= this.DataSourceAdded)
        this.DataSourceAdded(item);
    }


    /// <summary>
    /// Data source events.
    /// </summary>
    public event DataSourceAddedDelegate DataSourceAdded;
  }
}
