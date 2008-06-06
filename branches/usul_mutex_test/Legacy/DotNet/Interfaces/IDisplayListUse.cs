
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  /// <summary>
  /// Delegate for notification of a change.
  /// </summary>
  public delegate void DisplayListUseChangedDelegate(object changed, bool oldValue, bool newValue);

  public interface IDisplayListUse : IReferenced
  {
    /// <summary>
    /// Set/get the use of display lists.
    /// </summary>
    bool UseDisplayLists { get; set; }

    /// <summary>
    /// Get/set the delegate.
    /// </summary>
    CadKit.Interfaces.DisplayListUseChangedDelegate DisplayListUseChanged { get; set; }
  }
}
