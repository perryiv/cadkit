
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IOssimPlanetSettings : IReferenced
  {
    bool ElevationEnabled { get; set; }
    bool EphemerisEnabled { get; set; }
    bool HudEnabled { get; set; }
    bool LatLongGrid { get; set; }

    float HeightExageration { get; set; }
    float ElevationPatchSize { get; set; }
    float MaxLevelDetail { get; set; }

    string ElevationCacheDir { get; set; }
  }
}
