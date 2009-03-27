
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to transform vertices from one coordinate system to another.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_TRANSFORM_H__
#define __MINERVA_CORE_DATA_TRANSFORM_H__

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

#include "ogr_geometry.h"

#include <string>

namespace Minerva {
namespace Core {
namespace Data {


class Transform
{
public:
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  Transform ( const std::string& from, const std::string& to ) : 
    _transform ( 0x0 )
  {
    // Make the transform.
    OGRSpatialReference src ( from.c_str() ), dst ( to.c_str() );
    _transform = ::OGRCreateCoordinateTransformation ( &src, &dst );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Copy Constructor.
  //
  ///////////////////////////////////////////////////////////////////////////////

  Transform ( const Transform& rhs ) :
    _transform ( Transform::_clone ( rhs._transform ) )
  {
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  ///////////////////////////////////////////////////////////////////////////////

  Transform& operator= ( const Transform& rhs )
  {
    _transform = Transform::_clone ( rhs._transform );
    return *this;
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  ~Transform()
  {
    ::OCTDestroyCoordinateTransformation ( _transform );
  }

  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Transform Vec3.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  Usul::Math::Vec3d operator() ( const Usul::Math::Vec3d& vertex ) const
  {
    Usul::Math::Vec3d v ( vertex );
    if ( 0x0 != _transform )
    {
      _transform->Transform ( 1, &v[0], &v[1], &v[2] );
    }
    return v;
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Transform Vec2.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  Usul::Math::Vec2d operator() ( const Usul::Math::Vec2d& vertex ) const
  {
    Usul::Math::Vec2d v ( vertex );
    if ( 0x0 != _transform )
    {
      _transform->Transform ( 1, &v[0], &v[1] );
    }
    return v;
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return well known text for wgs 84.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  static std::string wgs84AsWellKnownText()
  {
    OGRSpatialReference srs;
    srs.SetWellKnownGeogCS ( "WGS84" );
    
    char *wkt ( 0x0 );
    srs.exportToWkt( &wkt );
    
    std::string s ( wkt );
    
    ::OGRFree ( wkt );
    return s;
  }

private:

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Clone the transform.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  static OGRCoordinateTransformation * _clone ( OGRCoordinateTransformation * rhs )
  {
    if ( 0x0 != rhs )
    {
      return ::OGRCreateCoordinateTransformation ( rhs->GetSourceCS(), rhs->GetTargetCS() );
    }

    return 0x0;
  }

  OGRCoordinateTransformation *_transform;

};
  
  
}
}
}

#endif // __MINERVA_CORE_DATA_TRANSFORM_H__
