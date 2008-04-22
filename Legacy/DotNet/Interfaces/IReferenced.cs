
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IReferenced
  {
    /// <summary>
    /// Increment the reference count.
    /// </summary>
    void reference();


    /// <summary>
    /// Decrement the reference count.
    /// </summary>
    void dereference();


    /// <summary>
    /// Decrement the reference count. Pass "false" if you don't want cleanup.
    /// </summary>
    void dereference ( bool allowCleanup );


    /// <summary>
    /// Return the number of reference to this instance.
    /// </summary>
    uint RefCount { get; }
  }
}
