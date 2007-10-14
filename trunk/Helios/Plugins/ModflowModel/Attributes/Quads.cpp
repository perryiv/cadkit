
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Quads attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Attributes/Quads.h"
#include "Helios/Plugins/ModflowModel/Builders/BuildScene.h"
#include "Helios/Plugins/ModflowModel/Constants.h"
#include "Helios/Plugins/ModflowModel/Model/Layer.h"

#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( Quads );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Quads::Quads ( const std::string &name, IUnknown *parent, unsigned int sides ) : 
  BaseClass ( name, parent ),
  _sides ( sides )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Quads::~Quads()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Quads::_destroy ), "3568525110" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Quads::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Quads::buildScene ( Modflow::Model::Layer *layer )
{
  USUL_TRACE_SCOPE;

  // Make a new group.
  GroupPtr group ( new osg::Group );

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // If the layer is valid and we are visible...
  if ( ( 0x0 != layer ) && ( true == this->visible() ) )
  {
    // Determine cell size.
    const Modflow::Model::Layer::Vec2d cellSize ( layer->cellSize() );
    const Modflow::Model::Layer::Vec3d margin ( layer->margin() );
    const osg::Vec2d halfSize ( ( 0.5 * cellSize[0] ) - margin[0], ( 0.5 * cellSize[1] ) - margin[1] );

    // Add geode to group.
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    group->addChild ( geode.get() );

    // Always show both sides.
    OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

    // Proceed if we have at least a 2x2 grid.
    const Modflow::Model::Layer::Vec2ui gridSize ( layer->gridSize() );
    const unsigned int numCells ( gridSize[0] * gridSize[1] );
    if ( gridSize[0] > 1 && gridSize[1] > 1 )
    {
      // Make vertices and normals.
      osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
      vertices->reserve ( numCells * 24 ); // 4 on each face.

      osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
      normals->reserve ( vertices->size() ); // Need same number for "fast path".

      // Copy the sides.
      unsigned int sides ( 0 );
      {
        Guard guard ( this->mutex() );
        sides = _sides;
      }

      // Loop through the cells.
      for ( unsigned int i = 0; i < gridSize[0]; ++i )
      {
        for ( unsigned int j = 0; j < gridSize[1]; ++j )
        {
          // Is the cell valid?
          const Modflow::Model::Cell *cell ( layer->cell ( i, j ) );
          if ( 0x0 != cell )
          {
            // Lock the cell for performance.
            Guard guard ( cell->mutex() );

            // Add the quads.
            Modflow::Builders::BuildScene::addQuads 
              ( sides, cell->x(), cell->y(), cell->top(), cell->bottom(), halfSize, margin[2], vertices.get(), normals.get() );
          }
        }
      }

      // We often skip many cells above.
      vertices->trim();
      normals->trim();

      // Make geometry.
      osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
      geom->setVertexArray ( vertices.get() );
      geom->setNormalArray ( normals.get() );
      geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

      // Turn both of these on because vertex buffer, when available, 
      // overrides display-list settings.
      geom->setUseDisplayList ( true );
      geom->setUseVertexBufferObjects ( true );

      // Add primitive set.
      geom->addPrimitiveSet ( new osg::DrawArrays ( osg::DrawArrays::QUADS, 0, vertices->size() ) );
      geode->addDrawable ( geom.get() );
    }
  }

  // Set new scene and return it.
  this->_setScene ( group.get() );
  return BaseClass::buildScene ( layer );
}
