
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
//  A single modflow cell.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CELL_CLASS_H_
#define _MODFLOW_MODEL_CELL_CLASS_H_

#include "ModflowModel/Base/BaseObject.h"

#include "Usul/Base/Observed.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Properties/Attribute.h"

#include "osg/Referenced"

#include <map>
#include <string>
#include <vector>

namespace osg { class Node; }


namespace Modflow {
namespace Model {


class Cell : public Modflow::Base::BaseObject
{
public:

  // Useful typedefs.
  typedef Modflow::Base::BaseObject BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef std::vector < double > Vector;
  typedef std::map < std::string, Vector > NamedVectors;
  typedef std::map < std::string, double > NamedValues;
  typedef Usul::Pointers::WeakPointer < Cell > WeakPtr;
  typedef BaseClass::IStringGridGet IStringGridGet;
  typedef BaseClass::StringRow StringRow;
  typedef BaseClass::StringGrid StringGrid;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Cell );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Cell );

  // Construction.
  Cell ( double x, double y, unsigned int i, unsigned int j );

  // Set/get the bottom.
  void                    bottom ( double );
  double                  bottom() const;

  // Get the center.
  Vec3d                   center() const;

  // Clear the cell.
  virtual void            clear();

  // Return the indices in the grid.
  Vec2ui                  indices() const;

  // Set/get the top.
  void                    top ( double z );
  double                  top() const;

  // Get/set the named value.
  double                  value ( const std::string &name ) const;
  void                    value ( const std::string &name, double v );

  // Get/set the named vector.
  const Vector &          vector ( const std::string &name ) const;
  Vector &                vector ( const std::string &name );
  void                    vector ( const std::string &name, const Vector &v );

  // Set/get the x and y coordinates.
  void                    x ( double );
  double                  x() const;
  void                    y ( double );
  double                  y() const;

private:

  // Use reference counting.
  virtual ~Cell();

private:

  // Do not copy.
  Cell ( const Cell & );
  Cell &operator = ( const Cell & );

  double _x;
  double _y;
  Vec2ui _indices;
  double _bottom;
  double _top;
  NamedVectors _vectors;
  NamedValues _values;
};


} // namespace Model
} // namespace Modflow


#endif // _MODFLOW_MODEL_CELL_CLASS_H_
