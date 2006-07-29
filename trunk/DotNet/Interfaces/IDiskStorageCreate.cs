
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IDiskStorageCreate
  {
    /// <summary>
    /// Create disk storage.
    /// </summary>
    IDiskStorage create ( string name );
  }
}
