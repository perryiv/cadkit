
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_PROJECT_COORDINATES_H__
#define __USUL_PROJECT_COORDINATES_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

#include <vector>
#include <map>
#include <string>

namespace Usul {
namespace Interfaces {


struct IProjectCoordinates : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProjectCoordinates );

  /// Id for this interface.
  enum { IID = 1081536333u };

  struct ProjectionInfo
  {
    ProjectionInfo ( const std::string& name_, int code_ ) : name ( name_ ), code ( code_ )
    {
    }

    std::string name;
    int code;
  };

  typedef std::vector<ProjectionInfo> ProjectionInfos;
  typedef std::map<std::string, ProjectionInfos> Projections;

  /// Get a list of projection names.
  virtual Projections        projectionList() const = 0;

  /// Project to lat/lon with elevation using given spatial reference id.
  virtual void               projectToSpherical ( const Usul::Math::Vec3d& orginal, unsigned int srid, Usul::Math::Vec3d& latLonPoint ) const = 0;

}; // struct IProjectCoordinates


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_PROJECT_COORDINATES_H__ */

