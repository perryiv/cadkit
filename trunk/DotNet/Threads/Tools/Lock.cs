
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Tools
{
  ///
  /// See http://msdn.microsoft.com/msdnmag/issues/05/08/Concurrency/
  ///
  public partial class Lock
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private System.Threading.ReaderWriterLock _lock = new System.Threading.ReaderWriterLock();
    private readonly System.TimeSpan _timeout = new System.TimeSpan(0, 0, 10);


    /// <summary>
    /// Constructor
    /// </summary>
    public Lock()
    {
    }


    /// <summary>
    /// Constructor
    /// </summary>
    public Lock(int timeOutSeconds)
    {
      _timeout = new System.TimeSpan(0, 0, timeOutSeconds);
    }


    /// <summary>
    /// Return new read-lock.
    /// </summary>
    public ReadLock read()
    {
      return new ReadLock(_lock, _timeout);
    }


    /// <summary>
    /// Return new write-lock.
    /// </summary>
    public WriteLock write()
    {
      return new WriteLock(_lock, _timeout);
    }
  }
}
