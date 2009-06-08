
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  // Camera placement flags.
  public enum CameraOption { FIT, RESET, FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };

  public interface ICamera : IReferenced
  {
    /// <summary>
    /// Set the camera.
    /// </summary>
    void camera(CameraOption option);
  }
}
