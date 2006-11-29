
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Tools
{
  public partial class Lock
  {
    /// <summary>
    /// Local class for locking a read. Use with a "using" statement.
    /// </summary>
    public class ReadLock : System.IDisposable
    {
      /// <summary>
      /// Data members.
      /// </summary>
      private static int _count = 0;
      private static object _countMutex = new object();
      private System.Threading.ReaderWriterLock _lock = null;


      /// <summary>
      /// Constructor
      /// </summary>
      public ReadLock(System.Threading.ReaderWriterLock lk, System.TimeSpan timeout)
      {
        _lock = lk;
        if (null == _lock)
        {
          throw new System.ArgumentNullException("Error 2719124605: Null lock given");
        }
        try
        {
          if (true == _lock.IsWriterLockHeld)
          {
            throw new System.Exception("Error 1128851690: Writer-lock is already held");
          }
          _lock.AcquireReaderLock(timeout);
          this._incrementCount();
          //this._printCount();
        }
        catch (System.ApplicationException e)
        {
          System.Console.WriteLine("Error 3866214997: Failed to obtain lock for reading. {0}", e.Message);
          this._printCount();
          throw;
        }
      }


      /// <summary>
      /// Print the count.
      /// </summary>
      private void _printCount()
      {
        System.Console.WriteLine("Total number of read-locks: {0}", this.Count);
      }


      /// <summary>
      /// Dispose this instance.
      /// </summary>
      void System.IDisposable.Dispose()
      {
        if (null != _lock)
        {
          if (true == _lock.IsWriterLockHeld)
          {
            System.Console.WriteLine("Error 2517188982: Writer-lock is held");
            return;
          }
          else if (true == _lock.IsReaderLockHeld)
          {
            _lock.ReleaseReaderLock();
            this._decrementCount();
            //this._printCount();
          }
        }
      }


      /// <summary>
      /// Increment the count.
      /// </summary>
      private void _incrementCount()
      {
        lock (_countMutex) { ++_count; }
      }


      /// <summary>
      /// Decrement the count.
      /// </summary>
      private void _decrementCount()
      {
        lock (_countMutex)
        {
          if (0 == _count)
          {
            throw new System.Exception("Error 3380498059: Lock count is already zero");
          }
          --_count;
        }
      }


      /// <summary>
      /// Return total number of locks.
      /// </summary>
      int Count
      {
        get { lock (_countMutex) { return _count; } }
      }
    }
  }
}
