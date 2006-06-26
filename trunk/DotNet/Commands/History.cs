
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public class History : System.Collections.Generic.LinkedList<CadKit.Commands.Command>
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private static History _instance = null;
    private object _mutex = new object();

    /// <summary>
    /// Constructor.
    /// </summary>
    private History()
    {
    }

    /// <summary>
    /// Get the instance.
    /// </summary>
    public static History Instance
    {
      get
      {
        lock ("CadKit.Commands.History.instance")
        {
          if (null == _instance)
            _instance = new History();
          return _instance;
        }
      }
    }

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    public virtual bool canUndo()
    {
      lock (_mutex)
      {
        return ((this.Count > 0) && this.Last.Value.canUndo());
      }
    }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    public virtual bool canRedo()
    {
      lock (_mutex)
      {
        return ((this.Count > 0) && this.Last.Value.canRedo());
      }
    }
  }
}
