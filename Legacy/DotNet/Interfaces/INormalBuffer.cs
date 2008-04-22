
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface INormalBuffer<T>
  {
    /// <summary>
    /// Set/get the number of normal vectors.
    /// </summary>
    uint Count { get; set; }

    /// <summary>
    /// Get the normal.
    /// </summary>
    void get(uint i, out T x, out T y, out T z);

    /// <summary>
    /// Set the normal.
    /// </summary>
    void set(uint i, T x, T y, T z);

    /// <summary>
    /// Normalize the specified vector.
    /// </summary>
    void normalize(uint i);

    /// <summary>
    /// Normalize all the vectors.
    /// </summary>
    void normalize();
  }
}
