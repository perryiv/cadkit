
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent point data plus a date.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_POINT_TIME_H__
#define __DATA_OBJECT_POINT_TIME_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/Point.h"

#include "OsgTools/Animate/Date.h"

#include <string>

namespace Minerva {
namespace Core {
namespace DataObjects {

class MINERVA_EXPORT PointTime : public Point
{
public:
  typedef Minerva::Core::DataObjects::Point BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PointTime );

  PointTime( const std::string& first, const std::string& last );

  /// Get the first date.
  const OsgTools::Animate::Date&    firstDate() const;

  /// Get the last date.
  const OsgTools::Animate::Date&    lastDate() const;

protected:
  /// Use reference counting
  virtual ~PointTime();

private:
  OsgTools::Animate::Date _firstDate;
  OsgTools::Animate::Date _lastDate;
};

}
}
}

#endif // __DATA_OBJECT_POINT_TIME_H__
