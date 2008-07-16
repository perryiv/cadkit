
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to use Yahoo! to geocode data.
//  http://developer.yahoo.com/maps/rest/V1/geocode.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_UTILITIES_GEOCODE_H__
#define __MINERVA_CORE_UTILITIES_GEOCODE_H__

#include "Minerva/Core/Export.h"

#include "Usul/Math/Vector2.h"

#include <string>

namespace Minerva {
namespace Core {
namespace Utilities {
  
class MINERVA_EXPORT GeoCode
{
public:
  GeoCode();
  ~GeoCode();
  
  /// Set/get the Yahoo! application id.
  void                applicationId ( const std::string& );
  const std::string&  applicationId() const;
  
  /// Get a lat,lon based on free form location.
  Usul::Math::Vec2d   operator() ( const std::string& location ) const;
  
private:
    
  std::string _appid;
};

}
}
}

#endif // __MINERVA_CORE_UTILITIES_GEOCODE_H__
