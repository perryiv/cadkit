
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IJitterAntialias : IReferenced
  {
    System.UInt32[] AvailableRenderingPasses { get; }
    double ScatterScale { get; set; }
    uint RenderingPasses { get; set; }
  }
}
