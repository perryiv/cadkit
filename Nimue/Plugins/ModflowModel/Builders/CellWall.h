
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
//  Geometry builder for cell wall.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_BUILDERS_CELL_WALL_BUILDER_H_
#define _MODFLOW_BUILDERS_CELL_WALL_BUILDER_H_

#include "ModflowModel/Builders/Geometry.h"

#include "ModflowModel/Model/Layer.h"
#include "ModflowModel/Model/Cell.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Trace/Trace.h"


namespace Modflow {
namespace Builders {


class CellWall : public Modflow::Builders::Geometry
{
public:

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CellWall );

  // Useful typedefs.
  typedef Modflow::Builders::Geometry BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef BaseClass::Layer Layer;
  typedef BaseClass::Cell Cell;
  typedef Usul::Math::Vec3d Vec3d;

  // Constructor.
  CellWall ( const std::string &name, Modflow::Geometry::ID geometry ) : BaseClass ( name, geometry )
  {
    USUL_TRACE_SCOPE;
  }

  // Build the geometry.
  void            build ( double halfSizeX, double halfSizeY, double x, double y, double top, double bottom, osg::Vec3Array *vertices, osg::Vec3Array *normals, const Vec3d &offset, double lengthScale );

protected:

  // Use reference counting.
  virtual ~CellWall()
  {
    USUL_TRACE_SCOPE;
  }

  // Build the geometry.
  virtual void    build ( const Layer *layer, const Cell *cell, osg::Vec3Array *vertices, osg::Vec3Array *normals, const Vec3d &offset, double lengthScale );

private:

  // No copying or assignment.
  CellWall ( const CellWall & );
  CellWall &operator = ( const CellWall & );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Build the cell wall.
//
///////////////////////////////////////////////////////////////////////////////

inline void CellWall::build ( double halfSizeX, double halfSizeY, double x, double y, double top, double bottom, osg::Vec3Array *vertices, osg::Vec3Array *normals, const Vec3d &origin, double scale )
{
  USUL_TRACE_SCOPE;
  // Re-entrant.

  // Check input.
  if ( 0x0 == vertices || 0x0 == normals )
    return;

  // Offset.
  const osg::Vec3f offset ( origin[0], origin[1], origin[2] );

  // Build proper geometry.
  switch ( this->geometry() )
  {
    case Modflow::Geometry::CELL_TOP:
    case Modflow::Geometry::CELL_HEAD_LEVEL:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, top ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, top ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, top ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, top ) * scale ) );

      const osg::Vec3f n ( 0, 0, 1 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    case Modflow::Geometry::CELL_BOTTOM:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, bottom ) * scale ) );

      const osg::Vec3f n ( 0, 0, -1 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    case Modflow::Geometry::CELL_EAST:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, top )    * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, top )    * scale ) );

      const osg::Vec3f n ( 1, 0, 0 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    case Modflow::Geometry::CELL_WEST:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, top )    * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, top )    * scale ) );

      const osg::Vec3f n ( -1, 0, 0 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    case Modflow::Geometry::CELL_NORTH:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y + halfSizeY, top )    * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y + halfSizeY, top )    * scale ) );

      const osg::Vec3f n ( 0, 1, 0 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    case Modflow::Geometry::CELL_SOUTH:
    {
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, bottom ) * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x + halfSizeX, y - halfSizeY, top )    * scale ) );
      vertices->push_back ( offset + ( osg::Vec3f ( x - halfSizeX, y - halfSizeY, top )    * scale ) );

      const osg::Vec3f n ( 0, -1, 0 );
      normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
    }
    break;

    default:
    {
      USUL_ASSERT ( 0 ); // Heads up... remove this if annoying.
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the cell wall.
//
///////////////////////////////////////////////////////////////////////////////

inline void CellWall::build ( const Layer *layer, const Cell *cell, osg::Vec3Array *vertices, osg::Vec3Array *normals, const Vec3d &origin, double scale )
{
  USUL_TRACE_SCOPE;
  // Re-entrant.

  // Check input.
  if ( 0x0 == layer || 0x0 == cell || 0x0 == vertices || 0x0 == normals )
    return;

  // Determine cell size.
  const Modflow::Model::Layer::Vec2d cellSize ( layer->cellSize() );
  const Modflow::Model::Layer::Vec2d margin ( layer->margin() );
  const double halfSizeX ( ( 0.5 * cellSize[0] ) - margin[0] );
  const double halfSizeY ( ( 0.5 * cellSize[1] ) - margin[1] );
  const double x ( cell->x() );
  const double y ( cell->y() );
  const double top    ( cell->top() );
  const double bottom ( cell->bottom() );

  // Call the other one.
  this->build ( halfSizeX, halfSizeY, x, y, top, bottom, vertices, normals, origin, scale );
}


} // namespace Builders
} // namespace Modflow


#endif // _MODFLOW_BUILDERS_CELL_WALL_BUILDER_H_
