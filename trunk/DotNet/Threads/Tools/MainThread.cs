
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Tools
{
  public class MainThread
  {
    private static MainThread _instance = null;
    private System.Threading.Thread _thread = System.Threading.Thread.CurrentThread;

    private MainThread()
    {
    }

    public static MainThread Instance
    {
      get
      {
        lock ("CadKit.Threads.Tools.MainThread.Instance")
        {
          if (null == _instance)
            _instance = new MainThread();
          return _instance;
        }
      }
    }

    public bool IsMainThread
    {
      get { return (null == _thread) ? false : (System.Threading.Thread.CurrentThread.ManagedThreadId == _thread.ManagedThreadId); }
    }
  }
}
