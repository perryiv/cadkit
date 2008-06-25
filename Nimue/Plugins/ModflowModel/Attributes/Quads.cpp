
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
//  Quads attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/Quads.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Builders/Geometry.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/Model/Layer.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( Quads );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Quads::Quads ( const std::string &name, IUnknown *parent, Modflow::Geometry::ID geometry, const RegistryPath &regPath ) : 
  BaseClass ( name, parent, regPath ),
  _geometry ( geometry ),
  _root ( new osg::Group )
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

  OsgTools::Group::removeAllChildren ( _root.get() );
  _root = 0x0;

  BaseClass::clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Quads::buildScene ( Modflow::ModflowDocument *document, Modflow::Model::Layer *layer, IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Handle bad state.
  if ( false == _root.valid() )
    return new osg::Group;

  // Handle no document.
  if ( 0x0 == document )
    return _root.get();

  // No longer dirty. Do this before we can return.
  this->dirtyState ( false );

  // Clear the cell weak pointers.
  this->_cellWeakPointersClear();

  // Copy the geometry.
  Modflow::Geometry::ID geometry ( Usul::Threads::Safe::get ( this->mutex(), _geometry ) );

  // Get the length scale and offset.
  const double lengthScale ( document->lengthConversion() );
  const Usul::Math::Vec3d offset ( document->offsetGet ( Modflow::Attributes::CELL_GRID_ORIGIN ) );

  // Get the builder.
  Modflow::Builders::Geometry::RefPtr builder ( document->builder ( geometry ) );

  // If we have a builder, the layer is valid, and we are visible...
  if ( ( 0x0 != layer ) && ( true == this->visible() ) && ( true == builder.valid() ) )
  {
    // Add geode to group.
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
    _root->addChild ( geode.get() );

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
          Modflow::Model::Cell::RefPtr cell ( layer->cell ( i, j ) );
          if ( true == cell.valid() )
          {
            // Get current number of vertices.
            const unsigned int originalNumVertices ( vertices->size() );

            // Add the quads.
            builder->build ( layer, cell, vertices.get(), normals.get(), offset, lengthScale );

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
      
      // Transform the vertices.
      document->transformCoordinates ( *vertices );

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
        c = minColor + ( dir * fraction );
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
  this->_setScene ( _root.get() );
  return BaseClass::buildScene ( document, layer, caller );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Quads::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->visible ( state );
  _root->setNodeMask ( ( true == state ) ? 0xFFFFFFFF : 0 );
  BaseClass::requestActiveDocumentRedraw();
}
