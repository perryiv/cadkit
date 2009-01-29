///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;

namespace VaporIntrusionDotNet
{
  public class Helper
  {
    // 4D vector
    public struct Vector4
    {
      public int x, y, z, w;
    };

    // 3D vector
    public struct Vector3
    {
      public int x, y, z;
    };

    // 2D vector
    public struct Vector2
    {
      public int x, y;
    };
  }
}
