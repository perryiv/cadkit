
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
//  Class to represent boundary data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECTS_BOUNDARY_H__
#define __DATA_OBJECTS_BOUNDARY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/Line.h"

#include "osg/ref_ptr"
#include "osg/Array"

#include <map>

namespace Minerva {
namespace Core {
namespace DataObjects {


class MINERVA_EXPORT Polygon : public Line
{
public:
  typedef Minerva::Core::DataObjects::Line BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Polygon );

  Polygon();

  /// Build the scene branch for the data object.
  virtual osg::Node*    buildScene();

protected:
  /// Use reference counting.
  virtual ~Polygon();

private:
  osg::ref_ptr < osg::Group > _group;
};

}
}
}

#endif // __DATA_OBJECTS_BOUNDARY_H__
