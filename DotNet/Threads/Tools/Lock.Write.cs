
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
    public class WriteLock : System.IDisposable
    {
      /// <summary>
      /// Data members.
      /// </summary>
      private System.Threading.ReaderWriterLock _lock = null;


      /// <summary>
      /// Constructor
      /// </summary>
      public WriteLock(System.Threading.ReaderWriterLock lk, System.TimeSpan timeout)
      {
        _lock = lk;
        if (null == _lock)
        {
          throw new System.ArgumentNullException("Error 2719124605: Null lock given");
        }
        try
        {
          // See http://msdn.microsoft.com/msdnmag/issues/05/08/Concurrency/
          _lock.AcquireWriterLock(timeout);
        }
        catch (System.ApplicationException e)
        {
          System.Console.WriteLine("Error 3725156970: Failed to obtain lock for writing. {0}", e.Message);
          throw;
        }
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
            // See http://msdn.microsoft.com/msdnmag/issues/05/08/Concurrency/
            _lock.ReleaseWriterLock();
          }
        }
      }
    }
  }
}
