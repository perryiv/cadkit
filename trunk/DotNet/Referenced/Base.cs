
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

namespace CadKit
{
  namespace Referenced
  {
    public class Base
    {
      public Base()
      {
      }

      ~Base()
      {
        System.Diagnostics.Debug.Assert( 0 == _refCount, "Error 3799827700: deleting object with reference count of " + _refCount.ToString() );
        System.Diagnostics.Debug.Assert( true == _isClean, "Error 3427194855: deleting object that has not been cleaned" );
      }

      public void reference()
      {
        try
        {
          lock ( _mutex )
          {
            ++_refCount;
          }
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine( "Error 3516250869: {0}\n{1}", e.Message, e.StackTrace );
        }
      }

      public void dereference()
      {
        try
        {
          lock ( _mutex )
          {
            --_refCount;
            if ( 0 == _refCount )
            {
              this._cleanup();
            }
          }
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine( "Error 2162012100: {0}\n{1}", e.Message, e.StackTrace );
        }
      }

      protected virtual void _cleanup()
      {
        _isClean = true;
      }

      public bool IsClean
      {
        get 
        {
          lock ( _mutex )
          return _isClean;
        }
      }

      public uint RefCount
      {
        get
        {
          lock ( _mutex )
          return _refCount;
        }
      }

      uint _refCount = 0;
      object _mutex = new object();
      bool _isClean = false;
    }
  }
}
