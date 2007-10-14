
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
#include "Helios/Plugins/ModflowModel/ModflowDocument.h"
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
//  Clear the attribute.
//
///////////////////////////////////////////////////////////////////////////////

void Quads::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Quads::buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *layer )
{
  USUL_TRACE_SCOPE;

  // Make a new group.
  GroupPtr group ( new osg::Group );

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // Clear the cell weak pointers.
  this->_cellWeakPointersClear();

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
      // Make vertices and normals, 4 on each face.
      osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
      vertices->reserve ( numCells * 24 );

      // Need same number for "fast path".
      osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array );
      normals->reserve ( vertices->size() );

      // Copy the sides.
      unsigned int sides ( 0 );
      {
        Guard guard ( this->mutex() );
        sides = _sides;
      }

      // Save room for the weak pointers, up to all 6 sides.
      this->_cellWeakPointersReserve ( numCells * 6 );

      // Get min and max colors.
      const osg::Vec4f minColor ( this->minColor()[0], this->minColor()[1], this->minColor()[2], this->minColor()[3] );
      const osg::Vec4f maxColor ( this->maxColor()[0], this->maxColor()[1], this->maxColor()[2], this->maxColor()[3] );

      // Loop through the cells.
      for ( unsigned int i = 0; i < gridSize[0]; ++i )
      {
        for ( unsigned int j = 0; j < gridSize[1]; ++j )
        {
          // Is the cell valid?
          Modflow::Model::Cell *cell ( layer->cell ( i, j ) );
          if ( 0x0 != cell )
          {
            // Lock the cell for performance.
            Guard guard ( cell->mutex() );

            // Get current number of vertices.
            const unsigned int originalNumVertices ( vertices->size() );

            // Add the quads.
            Modflow::Builders::BuildScene::addQuads 
              ( sides, cell->x(), cell->y(), cell->top(), cell->bottom(), 
                halfSize, margin[2], 
                vertices.get(), normals.get() );

            // Get number of vertices used for this cell.
            const unsigned int numVerticesForCell ( vertices->size() - originalNumVertices );

            // Save in sequence for each vertex.
            this->_cellWeakPointersAppend ( cell, numVerticesForCell );
          }
        }
      }

      // We often skip many cells above.
      vertices->trim();
      normals->trim();
      this->_cellWeakPointersTrim();

      // Set the color for each vertex. Same number for "fast path".
      const unsigned int numVertices ( vertices->size() );
      osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( numVertices ) );
      const Usul::Math::Vec2d zRange ( document->zRange() );
      const osg::Vec4f dir ( maxColor - minColor );
      for ( unsigned int k = 0; k < numVertices; ++k )
      {
        osg::Vec3f &v ( vertices->at(k) );
        osg::Vec4f &c ( colors->at(k) );
        const float fraction ( ( v[2] - zRange[0] ) / ( zRange[1] - zRange[0] ) );
        c[0] = minColor[0] + ( dir[0] * fraction );
        c[1] = minColor[1] + ( dir[1] * fraction );
        c[2] = minColor[2] + ( dir[2] * fraction );
        c[3] = minColor[3] + ( dir[3] * fraction );
#if 0
        std::cout << "fraction = " << fraction;
        std::cout << ", c = ( " << c[0] << ' ' << c[1] << ' ' << c[2] << ' ' << c[3] << " )";
        std::cout << ", minColor = ( " << minColor[0] << ' ' << minColor[1] << ' ' << minColor[2] << ' ' << minColor[3] << " )";
        std::cout << ", maxColor = ( " << maxColor[0] << ' ' << maxColor[1] << ' ' << maxColor[2] << ' ' << maxColor[3] << " )";
        std::cout << ", dir = ( " << dir[0] << ' ' << dir[1] << ' ' << dir[2] << ' ' << dir[3] << " )";
        std::cout << std::endl;
#endif
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
    }
  }

  // Set new scene and return it.
  this->_setScene ( group.get() );
  return BaseClass::buildScene ( document, layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append internal state to list.
//
///////////////////////////////////////////////////////////////////////////////

void Quads::getStringGrid ( Usul::Interfaces::IStringGridGet::StringGrid &data ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  data.push_back ( BaseClass::makeStringRow ( "Side", this->name() ) );
}
