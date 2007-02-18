
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using Types = System.Collections.Generic.Dictionary<string, object>;

namespace DT.Minerva.Layers.Controls
{
  public class Factory
  {
    private static Factory _instance = null;
    private Types _types = new Types();


    /// <summary>
    /// 
    /// </summary>
    private Factory()
    {
      if (this.hasPersistantLayers())
      {
        this._deserialize();
      }

      if(!this.hasType("Custom Polygons"))
        _types.Add("Custom Polygons", new DT.Minerva.Layers.Controls.GenericPolygons());

      if (!this.hasType("Custom Lines"))
        _types.Add("Custom Lines", new DT.Minerva.Layers.Controls.GenericLines());

      if (!this.hasType("Custom Points"))
        _types.Add("Custom Points", new DT.Minerva.Layers.Controls.GenericPoints());

      if (!this.hasType("Custom Point Time"))
        _types.Add("Custom Point Time", new DT.Minerva.Layers.Controls.GenericPointTime());

      if (!this.hasType("Custom Polygon Time"))
        _types.Add("Custom Polygon Time", new DT.Minerva.Layers.Controls.GenericPolygonTime());

      if (!this.hasType("R Results"))
        _types.Add("R Results", new DT.Minerva.Layers.Controls.RControl());

      System.Windows.Forms.Application.ApplicationExit += new System.EventHandler(Application_ApplicationExit);
    }


    /// <summary>
    /// 
    /// </summary>
    void Application_ApplicationExit(object sender, System.EventArgs e)
    {
      this._serialize();
      _types.Clear();
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _serialize()
    {
      System.IO.Stream stream = null;
      try
      {
        stream = System.IO.File.Open(this.Filename, System.IO.FileMode.Create);
        //System.Xml.Serialization.XmlSerializer formatter = new System.Xml.Serialization.XmlSerializer(typeof(Types));
        System.Runtime.Serialization.Formatters.Binary.BinaryFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

        formatter.Serialize(stream, _types);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3243196136: Exception caught while trying to serialize layer data.");
        System.Console.WriteLine("Message: {0}", e.Message);
      }
      finally
      {
        if (null != stream)
          stream.Close();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    protected void _deserialize()
    {
      System.IO.Stream stream = null;
      try
      {
        stream = System.IO.File.Open(this.Filename, System.IO.FileMode.OpenOrCreate);
        //System.Xml.Serialization.XmlSerializer formatter = new System.Xml.Serialization.XmlSerializer(typeof(Types));
        System.Runtime.Serialization.Formatters.Binary.BinaryFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
        _types = (Types)formatter.Deserialize(stream);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2803039886: Exception caught while trying to deserialize layer data.");
        System.Console.WriteLine("Message: {0}", e.Message);
      }
      finally
      {
        if (null != stream)
          stream.Close();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public static Factory Instance
    {
      get
      {
        lock ("DT.Minerva.Layers.Controls.Factory")
        {
          if (null == _instance)
            _instance = new Factory();
          return _instance;
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public string[] Names
    {
      get
      {
        System.Collections.Generic.List<string> strings = new System.Collections.Generic.List<string>();
        foreach (string s in _types.Keys)
        {
          strings.Add(s);
        }
        return strings.ToArray();
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public void addType(string name, object type)
    {
      if(this.hasType(name))
        _types[name] = type;
      else
        _types.Add(name, type);
    }


    /// <summary>
    /// 
    /// </summary>
    public bool hasType(string name)
    {
      return _types.ContainsKey(name);
    }


    /// <summary>
    /// 
    /// </summary>
    protected string Filename
    {
      get
      {
        return System.Windows.Forms.Application.UserAppDataPath + "\\layers.saved";
      }
    }


    /// <summary>
    /// See if there is peristant layer data.
    /// </summary>
    protected bool hasPersistantLayers()
    {
      if( System.IO.File.Exists(this.Filename) )
      {
        System.IO.FileInfo info = new System.IO.FileInfo(this.Filename);
        if (info.Length > 0)
          return true;
      }

      return false;
    }


    /// <summary>
    /// 
    /// </summary>
    public CadKit.Interfaces.ILayer create(string type, DT.Minerva.Interfaces.IDataSource dataSource)
    {
      DT.Minerva.Layers.Controls.BaseLayer iLayer = null;

      if (_types.ContainsKey(type))
      {
        System.ICloneable clone = _types[type] as System.ICloneable;

        if (null != clone)
        {
          iLayer = (BaseLayer)clone.Clone();

          if (null != iLayer)
          {
            iLayer.DataSource = dataSource as DT.Minerva.Interfaces.IDataSource;

            if (null != iLayer.Layer && null == iLayer.Layer.Connection)
              iLayer.Layer.Connection = dataSource as DT.Minerva.Interfaces.IDatabaseConnection;
          }
        }
      }
      return iLayer;
    }
  }
}
