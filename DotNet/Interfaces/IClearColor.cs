
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IClearColor : IReferenced
  {
    /// <summary>
    /// Get/set the clear color.
    /// </summary>
    object ClearColor { get; set; }
  }
}
