
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IBuildScene : IReferenced
  {
    /// <summary>
    /// Get the scene.
    /// </summary>
    object Scene { get; }
  }
}
