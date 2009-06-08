
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_CAMERA_H__
#define __MINERVA_CORE_DATA_CAMERA_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/AbstractView.h"

#include "Usul/Math/Matrix44.h"
#include "Usul/Interfaces/IAnimateMatrices.h"

namespace Minerva { namespace Core { namespace TileEngine { class LandModel; } } }

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Camera : public Minerva::Core::Data::AbstractView
{
public:
  typedef Minerva::Core::Data::AbstractView BaseClass;
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Interfaces::IAnimateMatrices::Matrices Matrices;

  USUL_DECLARE_REF_POINTERS ( Camera );

  Camera();

  static void generateAnimatePath ( 
    Camera::RefPtr start, 
    Camera::RefPtr end, 
    double percentMidpointHeightAtTransition,
    unsigned int numPoints,
    Minerva::Core::TileEngine::LandModel*, 
    Matrices& matrices );

  /// Set/get the longitude.
  void        longitude ( double );
  double      longitude() const;
  
  /// Set/get the latitude.
  void        latitude ( double );
  double      latitude() const;
  
  /// Set/get the altitude.
  void        altitude ( double );
  double      altitude() const;
  
  /// Set/get the heading.
  void        heading ( double );
  double      heading() const;

  /// Set/get the tilt.
  void        tilt ( double );
  double      tilt() const;
  
  /// Set/get the roll.
  void        roll ( double );
  double      roll() const;

  Matrix      viewMatrix ( Minerva::Core::TileEngine::LandModel* ) const;

protected:

  virtual ~Camera();

private:

  Camera ( const Camera& rhs );
  Camera& operator= ( const Camera& rhs );

  double _longitude;
  double _latitude;
  double _altitude;
  double _heading;
  double _tilt;
  double _roll;
};

  
}
}
}


#endif // __MINERVA_CORE_DATA_CAMERA_H__
