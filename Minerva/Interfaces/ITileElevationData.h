
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to retrieve elevation data for given extents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ITILE_ELEVATION_DATA_H__
#define __MINERVA_INTERFACES_ITILE_ELEVATION_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Minerva/Interfaces/IElevationData.h"

namespace Usul { namespace Jobs { class Job; } }

namespace Minerva {
namespace Interfaces {


struct ITileElevationData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITileElevationData );

  /// Id for this interface.
  enum { IID = 1004009685u };

  virtual IElevationData::RefPtr elevationData ( 
    double minLon,
    double minLat,
    double maxLon,
    double maxLat,
    unsigned int width,
    unsigned int height,
    unsigned int level,
    Usul::Jobs::Job* job,
    Usul::Interfaces::IUnknown* caller ) = 0;

}; // struct ITileElevationData


} // End namespace Interfaces
} // End namespace Minerva

#endif // __MINERVA_INTERFACES_ITILE_ELEVATION_DATA_H__
