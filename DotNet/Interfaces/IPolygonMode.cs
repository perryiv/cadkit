
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class PolygonMode
  {
    public enum Mode { NONE, POINT, LINE, FILL };
    public enum Face { NONE, FRONT, BACK, FRONT_AND_BACK };
  }

  public interface IPolygonMode
  {
    /// <summary>
    /// Set/get the polygon face.
    /// </summary>
    PolygonMode.Face Face { get; set; }

    /// <summary>
    /// Set/get the polygon mode.
    /// </summary>
    PolygonMode.Mode Mode { get; set; }
  }
}
