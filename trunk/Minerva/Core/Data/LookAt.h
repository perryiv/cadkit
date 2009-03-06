
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_LOOK_AT_H__
#define __MINERVA_CORE_DATA_LOOK_AT_H__

#include "Minerva/Core/Data/Object.h"

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT LookAt : public Minerva::Core::Data::Object
{
public:
  typedef Minerva::Core::Data::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( LookAt );

  LookAt();

  /// Set/get the longitude.
  void        longitude ( double );
  double      longitude() const;
  
  /// Set/get the latitude.
  void        latitude ( double );
  double      latitude() const;
  
  /// Set/get the altitude.
  void        altitude ( double );
  double      altitude() const;
  
  /// Set/get the range.
  void        range ( double );
  double      range() const;
  
  /// Set/get the heading.
  void        heading ( double );
  double      heading() const;
  
protected:

  virtual ~LookAt();

private:
  double _longitude;
  double _latitude;
  double _altitude;
  double _range;
  double _heading;
};

  
}
}
}


#endif // __MINERVA_CORE_DATA_LOOK_AT_H__
