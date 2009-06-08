
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IPropertyGridObject : IReferenced
  {
    /// <summary>
    /// Return the appropriate object for the property grid.
    /// </summary>
    object PropertyGridObject { get; }
  }
}
