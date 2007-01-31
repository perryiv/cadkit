
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins
{
  class Factory : CadKit.Interfaces.IClassFactory
  {
    /// <summary>
    /// Data members.
    /// </summary>
    class TypeMap : System.Collections.Generic.Dictionary<string, System.Type> { }
    private TypeMap _types = new TypeMap();
    private object _mutex = new object();

    /// <summary>
    /// Constructor.
    /// </summary>
    public Factory()
    {
      this._addType("CadKit.Interfaces.IPlugin", "DT.Minerva.Plugins.Animate.Component");
    }

    /// <summary>
    /// Add a type.
    /// </summary>
    private void _addType(string key, string name)
    {
      lock (_mutex)
      {
        System.Type type = System.Type.GetType(name);
        _types[key] = type;
      }
    }

    /// <summary>
    /// Create the instance.
    /// </summary>
    object CadKit.Interfaces.IClassFactory.createInstance(string key)
    {
      try
      {
        lock (_mutex)
        {
          System.Type type = _types[key];
          string name = type.ToString();
          return System.Reflection.Assembly.GetExecutingAssembly().CreateInstance(name);
        }
      }
      catch (System.Collections.Generic.KeyNotFoundException)
      {
        return null;
      }
    }
  }
}
