
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IRecentFileList : IReferenced
  {
    /// <summary>
    /// Add the name.
    /// </summary>
    void add(string name);

    /// <summary>
    /// Remove the name.
    /// </summary>
    void remove(string name);

    /// <summary>
    /// Clear the list.
    /// </summary>
    void clear();
  }
}
