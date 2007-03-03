
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IDiskStorage : IReferenced
  {
    /// <summary>
    /// Get the name of the disk storage.
    /// </summary>
    string Name { get; }

    /// <summary>
    /// By default any files used to store data are deleted when the object 
    /// gets deleted. Set to true in order to save these files.
    /// </summary>
    bool Persist { get; set; }
  }
}
