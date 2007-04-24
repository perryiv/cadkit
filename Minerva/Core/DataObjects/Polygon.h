
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

#include "osg/Group"

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
  virtual void          preBuildScene();

  /// Get/Set draw border flag.
  void                  showBorder( bool b );
  bool                  showBorder() const;

  /// Get/Set draw interior flag.
  void                  showInterior( bool b );
  bool                  showInterior() const;

protected:
  /// Use reference counting.
  virtual ~Polygon();

  osg::Node*    _buildPolygons();

private:
  osg::ref_ptr < osg::Group > _group;
  osg::ref_ptr < osg::Group > _preBuiltScene;

  bool _showBorder;
  bool _showInterior;
};

}
}
}

#endif // __DATA_OBJECTS_BOUNDARY_H__
