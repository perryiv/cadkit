
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IVertexBuffer<T>
  {
    /// <summary>
    /// Set/get the number of vertices.
    /// </summary>
    uint Count { get; set; }

    /// <summary>
    /// Get the vertex.
    /// </summary>
    void get(uint i, out T x, out T y, out T z);

    /// <summary>
    /// Set the vertex.
    /// </summary>
    void set(uint i, T x, T y, T z);
  }
}
