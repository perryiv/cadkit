
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IColorBuffer<T>
  {
    /// <summary>
    /// Set/get the number of colors.
    /// </summary>
    uint Count { get; set; }

    /// <summary>
    /// Get the color.
    /// </summary>
    void get(uint i, out T r, out T g, out T b, out T a);

    /// <summary>
    /// Get the color as a System.Drawing.Color object.
    /// </summary>
    object get(uint i);

    /// <summary>
    /// Set the normal.
    /// </summary>
    void set(uint i, T r, T g, T b, T a);

    /// <summary>
    /// Set the color from a System.Drawing.Color object.
    /// </summary>
    void set(uint i, object color);
  }
}
