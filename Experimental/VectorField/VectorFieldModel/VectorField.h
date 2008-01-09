
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_FIELD_H__
#define __VECTOR_FIELD_H__

#include "Grid2D.h"

namespace osg { class Node; }

class VectorField
{
public:
  typedef Grid2D Grid;

  VectorField();

  // Get/Set the grid.
  void              grid( const Grid& grid );
  Grid&             grid();
  const Grid&       grid() const;

  osg::Node*        advectParticle ( double x, double y ) const;

  osg::Node*        buildTopographyGraph () const;

private:
  Grid _grid;
};

#endif
