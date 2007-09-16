
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single modflow cell.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CELL_CLASS_H_
#define _MODFLOW_MODEL_CELL_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#include <map>
#include <string>
#include <vector>

namespace osg { class Node; }


class Cell : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef std::vector<double> Vector;
  typedef std::map<std::string,Vector> NamedVectors;
  typedef std::map<std::string,double> NamedValues;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Cell );

  // What to draw.
  struct Draw
  {
    enum
    {
      TOP    = 0x00000001,
      BOTTOM = 0x00000002,
      EAST   = 0x00000004,
      WEST   = 0x00000008,
      NORTH  = 0x00000010,
      SOUTH  = 0x00000020,
      SIDES  = EAST | WEST | NORTH | SOUTH,
      CUBE   = SIDES | TOP | BOTTOM,
      BOUNDS = 0x00000040,
      HEADS  = 0x00000080,
    };
  };

  // Construction.
  Cell ( double x, double y );

  // Set/get the bottom.
  void                    bottom ( double );
  double                  bottom() const;

  // Get the center.
  Vec3d                   center() const;

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
  double _bottom;
  double _top;
  NamedVectors _vectors;
  NamedValues _values;
};


#endif // _MODFLOW_MODEL_CELL_CLASS_H_
