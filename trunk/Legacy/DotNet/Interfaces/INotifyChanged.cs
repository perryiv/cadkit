
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  /// <summary>
  /// Delegate for notification of a change.
  /// </summary>
  public delegate void NotifyChangedDelegate(object changed, object caller);


  public interface INotifyChanged : IReferenced
  {
    /// <summary>
    /// Get/set the delegate.
    /// </summary>
    CadKit.Interfaces.NotifyChangedDelegate NotifyChanged { get; set; }
  }
}
