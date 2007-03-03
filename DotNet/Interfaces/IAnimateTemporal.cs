
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IAnimateTemporal : IReferenced
  {
    void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);
    void stopAnimation();
  }
}
