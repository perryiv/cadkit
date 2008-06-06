
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public enum AnimateTimestep
  {
    DAY,
    MONTH,
    YEAR
  }

  public interface IAnimateTemporal : IReferenced
  {
    AnimateTimestep TimestepType { get; set; }
    void startAnimation(float speed, bool accumulate, bool timeWindow, int numDays);
    void stopAnimation();
    void pauseAnimation();

    double AnimationSpeed { get; set; }

    bool ShowPastEvents { get; set; }

    bool TimeWindow { get; set; }

    bool TimeWindowLength { get; set; }
  }
}
