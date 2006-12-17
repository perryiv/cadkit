
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a grid.
//
///////////////////////////////////////////////////////////////////////////////

#include "Grid.h"

#include "Usul/Exceptions/Thrower.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/PolygonMode"
#include "osg/LineWidth"
#include "osg/MatrixTransform"
#include "osg/CullFace"
#include "osg/Depth"

#include <stdexcept>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Grid::Grid() :
  _numBlocks   ( 10, 10 ),
  _color       ( 1, 0, 0, 1 ),
  _fillColor       ( 0, 0, 0, 0 ),
  _size        ( 1, 1 ),
  _center      ( 0, 0, 0 ),
  _orientation ( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ),
  _lineWidth   ( 1 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build coordinate axes.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Grid::operator()() const
{
  // Check size.
  if ( 0 == _size[0] || 0 == _size[1] )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1368737500, invalid grid size: ", _size[0], ", ", _size[1] );
  }

  // Check number of blocks.
  if ( 0 == _numBlocks[0] || 0 == _numBlocks[1] )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 4160525306, invalid number of grid blocks: ", _numBlocks[0], ", ", _numBlocks[1] );
  }

  // For readability.
  float cx ( _center[0] );
  float cy ( _center[1] );
  float cz ( _center[2] );

  // Set the origin.
  float halhSizeX ( _size[0] / 2 );
  float halhSizeZ ( _size[1] / 2 );
  float originX ( cx - halhSizeX );
  float originY ( cy );
  float originZ ( cz - halhSizeZ );

  // Allocate the vertices.
  unsigned int numRows ( _numBlocks[0] + 1 );
  unsigned int numColumns ( _numBlocks[1] + 1 );
  unsigned int numVertices ( ( 2 * numRows - 2 ) * numColumns );
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( numVertices ) );

  // There is one quad-strip for each adjacent pair of row.
  osg::Geometry::PrimitiveSetList primSetList ( numRows - 1 );

  // Used in the loop.
  float rowPercentDenominator ( numRows - 1 );
  float columnPercentDenominator ( numColumns - 1 );
  float sizeX ( _size[0] );
  float sizeZ ( _size[1] );
  float percentTopZ ( 0 );
  unsigned int index ( 0 );
  unsigned int length ( 2 * numColumns ), start ( 0 );

  // Account for grid orientation
  Vec3f position ( 0, 0, 0 );
  Matrix44f translation ( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
  translation.makeTranslation ( _center );
  Matrix44f translationInv ( translation );
  translationInv.invert();

  // Set all the vertices.
  for ( unsigned int i = 0; i < numRows - 1; ++i )
  {
    float percentBotZ ( float ( i + 1 ) / rowPercentDenominator );
    float zTop ( originZ + percentTopZ * sizeZ );
    float zBot ( originZ + percentBotZ * sizeZ );

    for ( unsigned int j = 0; j < numColumns; ++j )
    {
      float percentX ( float ( j ) / columnPercentDenominator );
      float currentX ( originX + percentX * sizeX );

	  // Apply orientation
      USUL_ASSERT ( index < vertices->size() );
      position.set ( currentX, originY, zTop );
      position = translation * _orientation * translationInv * position;
      vertices->at ( index++ ).set ( position[0], position[1], position[2] );
      position.set ( currentX, originY, zBot );
      position = translation * _orientation * translationInv * position;
      vertices->at ( index++ ).set ( position[0], position[1], position[2]);
    }

    primSetList[i] = new osg::DrawArrays ( osg::PrimitiveSet::QUAD_STRIP, start, length );
    start += length;
    percentTopZ = percentBotZ;
  }

  // Should be true.
  USUL_ASSERT ( vertices->size() == index );
  USUL_ASSERT ( index == start );

  // Allocate geometry and set the vertices.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );

  // There is only one normal.
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( 1 ) );
  normals->at ( 0 ).set ( 0, 1, 0 );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

  // Set the primitive-set list.
  geometry->setPrimitiveSetList ( primSetList );

  // Make the geode for the grid.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Set the line-width.
  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( _lineWidth );
  osg::ref_ptr<osg::StateSet> ss = geometry->getOrCreateStateSet();
  ss->setAttribute ( lw.get() );

  // Tell the grid to draw wire-frame.
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
  mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  ss->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Tell the grid to draw both sides
  ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );

  // Tell it to use color instead of materials.
  ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
  osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( 1 ) );
  colors->at ( 0 ).set ( _color[0], _color[1], _color[2], _color[3] );
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
  
  if( _fillColor[3] != 0 ) // if not completely transparent, fill the grid with a color
  {
    // Allocate a new geometry with the same vertices & normals
    osg::ref_ptr<osg::Geometry> geometry2 ( new osg::Geometry );
    geometry2->setVertexArray ( vertices.get() );
    geometry2->setNormalArray ( normals.get() );
    geometry2->setNormalBinding ( osg::Geometry::BIND_OVERALL );
    geometry2->setPrimitiveSetList ( primSetList );
    
    // add geometry to geode
    geode->addDrawable ( geometry2.get() );
    
    // create a new StateSet for this geometry
    osg::ref_ptr<osg::StateSet> ss2 ( new osg::StateSet );
    geometry2->setStateSet( ss2.get() );
    
    // Tell the grid to draw filled.
    osg::ref_ptr<osg::PolygonMode> mode2 ( new osg::PolygonMode );
    mode2->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
    ss2->setAttributeAndModes ( mode2.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    
    // Tell the grid to draw both sides
    ss2->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
    
    // Tell it to use color instead of materials.
    ss2->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
    ss2->setMode ( GL_BLEND, osg::StateAttribute::ON );
    osg::ref_ptr<osg::Depth> depth (new osg::Depth);
    depth->setWriteMask( false );
    ss2->setAttribute( depth.get() );
    osg::ref_ptr<osg::Vec4Array> colors2 ( new osg::Vec4Array ( 1 ) );
    colors2->at ( 0 ).set ( _fillColor[0], _fillColor[1], _fillColor[2], _fillColor[3] );
    geometry2->setColorArray ( colors2.get() );
    geometry2->setColorBinding ( osg::Geometry::BIND_OVERALL );
  }  
  
  // Return the geode.
  return geode.release();
}
