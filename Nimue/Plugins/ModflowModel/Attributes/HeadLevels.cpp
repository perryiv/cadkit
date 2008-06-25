
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
//  Head levels attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/HeadLevels.h"
#include "ModflowModel/Builders/CellWall.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Model/Layer.h"

#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( HeadLevels );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

HeadLevels::HeadLevels ( const std::string &name, Modflow::Model::Layer *layer, const RegistryPath &regPath ) : 
  BaseClass ( name, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ), regPath ),
  _switch ( new osg::Switch ),
  _steps ( new osg::Group )
{
  USUL_TRACE_SCOPE;
  _switch->setAllChildrenOff();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

HeadLevels::~HeadLevels()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &HeadLevels::_destroy ), "3042942102" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevels::_destroy()
{
  USUL_TRACE_SCOPE;
  _switch = 0x0;
  _steps = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevels::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  OsgTools::Group::removeAllChildren ( _switch.get() );
  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevels::init ( Modflow::ModflowDocument *document, Modflow::Model::Layer *layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no document.
  if ( 0x0 == document )
    return;

  // Get the "no data" value.
  const Modflow::ModflowDocument::NoDataValue noData ( document->noDataGet ( Modflow::Names::HEAD_LEVELS ) );

  // Get the length scale and offset.
  const double lengthScale ( document->lengthConversion() );
  const Usul::Math::Vec3d offset ( document->offsetGet ( Modflow::Attributes::CELL_GRID_ORIGIN ) );

  // Loop through the time steps.
  const unsigned int timeSteps ( layer->numTimeSteps ( Modflow::Names::HEAD_LEVELS ) );
  for ( unsigned int t = 0; t < timeSteps; ++t )
  {
    // Add the time step.
    _steps->addChild ( this->_buildTimeStep ( t, document, layer, noData.first, noData.second, offset, lengthScale, &std::cout ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *HeadLevels::buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *layer, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no document.
  if ( 0x0 == document )
    return new osg::Group;

  // Just return existing switch if we're not dirty.
  if ( false == this->dirtyState() )
    return _switch.get();

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // Clear the switch.
  OsgTools::Group::removeAllChildren ( _switch.get() );

  // If the conditions are right...
  if ( ( 0x0 != layer ) && ( true == this->visible() ) )
  {
    // Add all the children from the group to the switch.
    OsgTools::Group::addAllChildren ( _steps.get(), _switch.get() );
  }

  // Set new scene and return it.
  this->_setScene ( _switch.get() );
  return BaseClass::buildScene ( document, layer, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *HeadLevels::_buildTimeStep ( unsigned int timeStep, 
                                        Modflow::ModflowDocument *document, 
                                        Modflow::Model::Layer *layer, 
                                        bool hasNoData, 
                                        long noData, 
                                        const Vec3d &offset, 
                                        double scale,
                                        std::ostream *out ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make a new group.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Handle no document.
  if ( 0x0 == document )
    return group.release();

  // Add geode to group.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  group->addChild ( geode.get() );

  // Always show both sides.
  OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

  const Modflow::Model::Layer::Vec2ui gridSize ( layer->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Make vertices.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  vertices->reserve ( numCells * 4 );

  // Need same number of normals for "fast path".
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
  normals->reserve ( vertices->size() );

  // Make the builders.
  typedef std::vector<Modflow::Builders::CellWall::RefPtr> BuilderVector;
  BuilderVector builders;
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_TOP ) );
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_BOTTOM ) );
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_NORTH ) );
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_SOUTH ) );
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_EAST ) );
  builders.push_back ( new Modflow::Builders::CellWall ( "Builder for Head Levels", Modflow::Geometry::CELL_WEST ) );

  // If the conditions are right...
  if ( 0x0 != layer )
  {
    // Determine cell size.
    const Modflow::Model::Layer::Vec2d cellSize ( layer->cellSize() );
    const Modflow::Model::Layer::Vec2d margin ( layer->margin() );
    const double halfSizeX ( ( 0.5 * cellSize[0] ) - margin[0] );
    const double halfSizeY ( ( 0.5 * cellSize[1] ) - margin[1] );

    // Loop through the cells.
    for ( unsigned int i = 0; i < gridSize[0]; ++i )
    {
      for ( unsigned int j = 0; j < gridSize[1]; ++j )
      {
        // Is the cell valid?
        Modflow::Model::Cell::RefPtr cell ( layer->cell ( i, j ) );
        if ( true == cell.valid() )
        {
          // Get the cell's properties.
          const Modflow::Model::Cell::Vector &heads ( cell->vector ( Modflow::Names::HEAD_LEVELS ) );
          if ( timeStep < heads.size() )
          {
            // Get the head value.
            const double h ( heads.at ( timeStep ) );

            // See if it's the registered "no data" value.
            const bool isNoData ( ( hasNoData ) ? ( noData == ( static_cast<long> ( h * Modflow::Constants::NO_DATA_MULTIPLIER ) ) ) : false );
            if ( false == isNoData )
            {
              const double x ( cell->x() );
              const double y ( cell->y() );
              const double bottom ( cell->bottom() );

              // Add the sides.
              for ( BuilderVector::iterator b = builders.begin(); b != builders.end(); ++b )
              {
                Modflow::Builders::CellWall::RefPtr builder ( *b );
                if ( true == builder.valid() )
                {
                  // Add the side.
                  builder->build ( halfSizeX, halfSizeY, x, y, h, bottom, vertices.get(), normals.get(), offset, scale );
                }
              }
            }
          }
        }
      }
    }
  }

  // We often skip many cells above.
  vertices->trim();
  normals->trim();
  
  // Transform the vertices.
  document->transformCoordinates ( *vertices );

  // Set the color for each vertex. Same number for "fast path".
  const unsigned int numVertices ( vertices->size() );
  osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( numVertices ) );
  const Usul::Math::Vec2d zRange ( document->zRange() );
  typedef Modflow::ModflowDocument::Color Color;
  const Color minColor ( document->color ( Modflow::Names::MIN_HEAD_LEVEL ) );
  const Color maxColor ( document->color ( Modflow::Names::MAX_HEAD_LEVEL ) );
  const Color dir ( maxColor - minColor );
  for ( unsigned int k = 0; k < numVertices; ++k )
  {
    osg::Vec3f &v ( vertices->at(k) );
    osg::Vec4f &c ( colors->at(k) );
    const float fraction ( ( v[2] - zRange[0] ) / ( zRange[1] - zRange[0] ) );
    const Color color ( minColor + ( dir * fraction ) );
    c.set ( color[0], color[1], color[2], color[3] );
  }

  // Make geometry.
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );
  geom->setColorArray ( colors.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

  // Turn both of these on because vertex buffer, when available, 
  // overrides display-list settings.
  geom->setUseDisplayList ( true );
  geom->setUseVertexBufferObjects ( true );

  // Add primitive set.
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::DrawArrays::QUADS, 0, vertices->size() ) );
  geode->addDrawable ( geom.get() );

  // Return the new group.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update this attribute.
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevels::update ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the interface we need.
  typedef Usul::Interfaces::ITimeVaryingData Data;
  Data::QueryPtr data ( this->_getParent() );
  if ( false == data.valid() )
    return;

  // Handle no children.
  const unsigned int numChildren ( _switch->getNumChildren() );
  if ( 0 == numChildren )
    return;

  // Handle out of range steps.
  const unsigned int step ( Usul::Math::minimum ( data->getCurrentTimeStep(), numChildren - 1 ) );

  // Set the child.
  _switch->setSingleChildOn ( step );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevels::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->visible ( state );
  _switch->setNodeMask ( ( true == state ) ? 0xFFFFFFFF : 0 );
  BaseClass::requestActiveDocumentRedraw();
}
