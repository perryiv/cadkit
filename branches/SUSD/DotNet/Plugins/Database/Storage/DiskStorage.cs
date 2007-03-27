
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Database.Storage
{
  public class DiskStorage :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IDiskStorage
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    string _name = null;
    bool _persist = false;

    /// <summary>
    /// Construct a storage object.
    /// </summary>
    public DiskStorage ( string name )
    {
      if (null == name)
        throw new System.ArgumentNullException("Error 2411466750: null name given for storage object", null as System.Exception);
      _name = name;
    }

    /// <summary>
    /// Return the name of this storage object.
    /// </summary>
    string CadKit.Interfaces.IDiskStorage.Name
    {
      get { lock (_mutex) { return _name; } }
    }

    /// <summary>
    /// By default any files used to store data are deleted when the object 
    /// gets deleted. Set to true in order to save these files.
    /// </summary>
    bool CadKit.Interfaces.IDiskStorage.Persist
    {
      get { lock (_mutex) { return _persist; } }
      set { lock (_mutex) { _persist = value; } }
    }
  }
}
