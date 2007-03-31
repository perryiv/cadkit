
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
  public class Base : System.IDisposable, CadKit.Interfaces.IReferenced
  {
    /// <summary
    /// Local types.
    /// </summary>
    class Objects : System.Collections.Generic.Dictionary<ulong, string> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private uint _refCount = 0;
    private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();
    private bool _isClean = false;
    private ulong _objectNumber = CadKit.Referenced.Base.NextNumber;
    private static Objects _objects = new Objects();
    private static ulong _objectCounter = 0;


    /// <summary>
    /// Constructor
    /// </summary>
    public Base()
    {
      _objects.Add(this.ObjectNumber, this.GetType().ToString());
    }


    /// <summary>
    /// Destructor
    /// </summary>
    ~Base()
    {
      try
      {
        _objects.Remove(this.ObjectNumber);
        if (0 != _refCount)
        {
          System.Console.WriteLine(System.String.Format("Error 3799827700: deleting object {0} with reference count of {1}", this.ToString(), _refCount.ToString()));
        }
        if (false == _isClean)
        {
          System.Console.WriteLine(System.String.Format("Error 3427194855: deleting object {0} which has not been cleaned", this.ToString()));
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(System.String.Format("Error 1499157600: {0}", e.Message));
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
        //this._cleanup();
      }
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.reference()
    {
      this.reference();
    }


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference()
    {
      this.dereference();
    }


    /// <summary>
    /// Decrement the reference count. Pass "false" if you don't want cleanup.
    /// </summary>
    void CadKit.Interfaces.IReferenced.dereference(bool allowCleanup)
    {
      this.dereference(allowCleanup);
    }


    /// <summary>
    /// Increment the reference count.
    /// </summary>
    public void reference()
    {
      try
      {
        using (this.Lock.write())
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
      this.dereference(true);
    }


    /// <summary>
    /// Decrement the reference count. Pass "false" if you don't want cleanup.
    /// </summary>
    public void dereference(bool allowCleanup)
    {
      try
      {
        if (0 == this.RefCount)
        {
          System.Console.WriteLine("Error 3765910177: reference count is already zero");
          return;
        }
        using (this.Lock.write())
        {
          --_refCount;
          if (0 == _refCount && true == allowCleanup)
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
      _objects.Remove(this.ObjectNumber);
      using (this.Lock.write())
      {
        _isClean = true;
      }
    }


    /// <summary>
    /// See if this instance is clean.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public bool IsClean
    {
      get
      {
        using (this.Lock.read())
        {
          return _isClean;
        }
      }
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    uint CadKit.Interfaces.IReferenced.RefCount
    {
      get { return this.RefCount; }
    }


    /// <summary>
    /// Return the reference count.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public uint RefCount
    {
      get
      {
        using (this.Lock.read())
        {
          return _refCount;
        }
      }
    }


    /// <summary>
    /// Get the object number.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public ulong ObjectNumber
    {
      get
      {
        using (this.Lock.read())
        {
          return _objectNumber;
        }
      }
    }


    /// <summary>
    /// Get the lock object.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public CadKit.Threads.Tools.Lock Lock
    {
      get
      {
        // If this gets called from the finalizer then the lock may have 
        // already been destroyed and set to null.
        if (null == _lock)
        {
          _lock = new CadKit.Threads.Tools.Lock();
        }
        return _lock;
      }
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
    /// Return the next object number.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    private static ulong NextNumber
    {
      get
      {
        lock ("CadKit.Referenced.Base.NextNumber")
        {
          return _objectCounter++;
        }
      }
    }


    /// <summary>
    /// Return total number of objects.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public static ulong TotalNumObjects
    {
      get
      {
        lock ("CadKit.Referenced.Base.TotalNumObjects")
        {
          int count = ((_objects.Count < 0) ? 0 : _objects.Count);
          return (ulong)count;
        }
      }
    }


    /// <summary>
    /// Return a string containing the object data. Used for debugging.
    /// </summary>
    [
    System.ComponentModel.Browsable(false)
    ]
    public static string ExistingObjects
    {
      get
      {
        lock ("CadKit.Referenced.Base.ExistingObjects")
        {
          string answer = "";
          Objects.KeyCollection keys = _objects.Keys;
          foreach (ulong key in keys)
          {
            answer += System.String.Format("{0}: {1}\n", key, _objects[key]);
          }
          return answer;
        }
      }
    }
  }
}
