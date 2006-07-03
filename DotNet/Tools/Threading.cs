
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class Threading
  {
    private static Threading _instance = null;
    private int _id = System.Threading.Thread.CurrentThread.ManagedThreadId;

    private Threading()
    {
    }

    public static Threading Instance
    {
      get
      {
        lock ("CadKit.Tools.Threading.Instance")
        {
          if (null == _instance)
            _instance = new Threading();
          return _instance;
        }
      }
    }

    public bool IsMainThread
    {
      // No need to lock a mutex here.
      get { return (System.Threading.Thread.CurrentThread.ManagedThreadId == _id); }
    }
  }
}
