
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all geometry builders.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_BUILDERS_BASE_GEOMETRY_BUILDER_H_
#define _MODFLOW_BUILDERS_BASE_GEOMETRY_BUILDER_H_

#include "ModflowModel/CompileGuard.h"
#include "ModflowModel/Constants.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector3.h"

#include "osg/Array"

namespace Modflow { namespace Model { class Layer; class Cell; } }


namespace Modflow {
namespace Builders {


class Geometry : public Usul::Base::Object
{
public:

  // Type information.
  USUL_DECLARE_TYPE_ID ( Geometry );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Geometry );

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef Modflow::Model::Layer Layer;
  typedef Modflow::Model::Cell Cell;
  typedef Usul::Math::Vec3d Vec3d;

  // Build the geometry.
  virtual void              build ( const Layer *layer, const Cell *cell, osg::Vec3Array *vertices, osg::Vec3Array *normals, const Vec3d &offset, double scale );

  // Get the geometry id.
  Modflow::Geometry::ID     geometry() const;

protected:

  // Constructor and destructor.
  Geometry ( const std::string &name, Modflow::Geometry::ID geometry );
  virtual ~Geometry();

private:

  // No copying or assignment.
  Geometry ( const Geometry & );
  Geometry &operator = ( const Geometry & );

  // Data members.
  Modflow::Geometry::ID _geometry;
};


} // namespace Builders
} // namespace Modflow


#endif // _MODFLOW_BUILDERS_BASE_GEOMETRY_BUILDER_H_
