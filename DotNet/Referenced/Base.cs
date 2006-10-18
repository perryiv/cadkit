
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for objects that need to clean up resources when nobody 
//  references them anymore. 
//
//  More specifically, these resources are released immediately through the 
//  protected virtual "cleanup" function. 
//
//  If waiting for the garbage collection will work for you, then you do not 
//  need this base class.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Referenced
{
  public class Base : System.IDisposable
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private uint _refCount = 0;
    private object _mutex = new object();
    private bool _isClean = false;
    private ulong _objectNumber = _objectCounter++;
    private static ulong _objectCounter = 0;

    /// <summary>
    /// Constructor
    /// </summary>
    public Base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Base()
    {
      if (0 != _refCount)
      {
        System.Console.WriteLine(System.String.Format("Error 3799827700: deleting object {0} with reference count of {1}", this.ToString(), _refCount.ToString()));
      }
      if (false == _isClean)
      {
        System.Console.WriteLine(System.String.Format("Error 3427194855: deleting object {0} which has not been cleaned", this.ToString()));
      }
    }

    /// <summary>
    /// Called by the system when collecting garbage. If an instance is 
    /// created and it's never referenced and dereferenced then the normal 
    /// path to cleanup does not happen. This makes sure it always happens.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      if (false == this.IsClean)
      {
        this._cleanup();
      }
    }

    /// <summary>
    /// Increment the reference count.
    /// </summary>
    public void reference()
    {
      try
      {
        lock (_mutex)
        {
          ++_refCount;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3516250869: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    public void dereference()
    {
      try
      {
        lock (_mutex)
        {
          System.Diagnostics.Debug.Assert(_refCount > 0, "Error 3765910177: reference count is already zero");
          --_refCount;
          if (0 == _refCount)
          {
            this._cleanup();
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2162012100: {0}\n{1}", e.Message, e.StackTrace);
      }
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected virtual void _cleanup()
    {
      lock (_mutex) { _isClean = true; }
    }

    /// <summary>
    /// See if this instance is clean.
    /// </summary>
    public bool IsClean
    {
      get { lock (_mutex) { return _isClean; } }
    }

    /// <summary>
    /// Return the reference count.
    /// </summary>
    public uint RefCount
    {
      get { lock (_mutex) { return _refCount; } }
    }

    /// <summary>
    /// Get the mutex, for external locking of this instance.
    /// </summary>
    public object Mutex
    {
      get { return _mutex; }
    }

    /// <summary>
    /// Convenience function for safe referencing.
    /// </summary>
    public static void reference(CadKit.Referenced.Base t)
    {
      if (null != t)
      {
        t.reference();
      }
    }

    /// <summary>
    /// Convenience function for safe dereferencing.
    /// </summary>
    public static void dereference(CadKit.Referenced.Base t)
    {
      if (null != t)
      {
        t.dereference();
      }
    }

    /// <summary>
    /// Get the object number.
    /// </summary>
    public ulong ObjectNumber
    {
      get { return _objectNumber; }
    }
  }
}
