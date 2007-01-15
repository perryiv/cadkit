
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Lib
{
  public class Controller : DT.Minerva.Interfaces.IApplicationConnection
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public class Connections : System.Collections.Generic.List<DT.Minerva.Interfaces.IDatabaseConnection> { }

    /// <summary>
    /// Delegates.
    /// </summary>
    public delegate void RemoveLayerDelegate(DT.Minerva.Interfaces.ILayer layer);
    public delegate void ShowLayerDelegate(DT.Minerva.Interfaces.ILayer layer);
    public delegate void ModifyLayerDelegate(DT.Minerva.Interfaces.ILayer layer);
    public delegate void StartAnimationDelegate(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);
    public delegate void StopAnimationDelegate();
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


    /// Remove layer with given id.
    public void removeLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != this.RemoveLayer)
        this.RemoveLayer(layer);
    }

    /// <summary>
    ///  Show a layer.
    /// </summary>
    public void showLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != this.ShowLayer)
        this.ShowLayer(layer);
    }

    /// Modify polygon data.
    public void modifyLayer(DT.Minerva.Interfaces.ILayer layer)
    {
      if (null != this.ModifyLayer)
        this.ModifyLayer(layer);
    }

    /// Start animation.
    public void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays)
    {
      if (null != this.StartAnimation)
        this.StartAnimation(speed, accumulate, dateTimeStep, timeWindow, numDays);
    }

    /// Stop Animation.
    public void stopAnimation()
    {
      if (null != this.StopAnimation)
        this.StopAnimation();
    }


    /// <summary>
    /// Layer events.
    /// </summary>
    public event RemoveLayerDelegate RemoveLayer;
    public event ShowLayerDelegate ShowLayer;
    public event ModifyLayerDelegate ModifyLayer;
    

    /// <summary>
    /// Animation events.
    /// </summary>
    public event StartAnimationDelegate StartAnimation;
    public event StopAnimationDelegate StopAnimation;


    /// <summary>
    /// Data source events.
    /// </summary>
    public event DataSourceAddedDelegate DataSourceAdded;
  }
}
