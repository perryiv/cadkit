
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class ScopedReference<T> : System.IDisposable
  {
    /// <summary>
    /// Default constructor.
    /// </summary>
    public ScopedReference()
    {
      this.Value = default(T);
    }


    /// <summary>
    /// Constructor.
    /// </summary>
    public ScopedReference(T obj)
    {
      this.Value = obj;
    }


    /// <summary>
    /// Copy constructor.
    /// </summary>
    public ScopedReference(ScopedReference<T> obj)
    {
      this.Value = (null != obj) ? obj.Value : default(T);
    }


    /// <summary>
    /// Called by the system, usually from a "using" block.
    /// </summary>
    void System.IDisposable.Dispose()
    {
      try
      {
        this.Value = default(T);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(System.String.Format("Error 3912194771: {0}", e.Message));
      }
    }


    /// <summary>
    /// Is the object valid?
    /// </summary>
    public bool Valid
    {
      get
      {
        return (null != _object);
      }
    }


    /// <summary>
    /// Get/set the object.
    /// </summary>
    public T Value
    {
      get
      {
        return _object;
      }
      set
      {
        // If both are null then do nothing.
        if (null == _object && null == value)
        {
          return;
        }

        // If neither is null then check for same object.
        if ((null != _object) && (null != value) && (true == _object.Equals(value)))
        {
          return;
        }

        // Make a copy of the current one.
        T old = _object;

        // Assign new object.
        _object = value;

        // Reference new object.
        if (null != _object)
        {
          ((CadKit.Interfaces.IReferenced)_object).reference();
        }

        // Dereference old handle. Make sure we do this last because new 
        // handle could be a child (indirect or direct) of "old".
        if (null != old)
        {
          ((CadKit.Interfaces.IReferenced)old).dereference();
        }
      }
    }


    /// <summary>
    /// Get the object and release ownership.
    /// </summary>
    public T release()
    {
      // Save a pointer.
      T saved = _object;

      // If we have a valid handle...
      if (null != _object)
      {
        // Unreference it but make sure it is not deleted.
        ((CadKit.Interfaces.IReferenced)_object).dereference(false);

        // Make our internal pointer null;
        _object = default(T);
      }

      // Return the saved handle.
      return saved;
    }


    /// <summary>
    /// Data members.
    /// </summary>
    private T _object = default(T);
  }
}
