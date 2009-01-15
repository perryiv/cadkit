
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIDocument.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Properties/Attribute.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/MinMax.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Box.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osgDB/WriteFile"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/Texture2D"
#include "osg/BoundingBox"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VaporIntrusionGUIDocument, VaporIntrusionGUIDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::VaporIntrusionGUIDocument() :   BaseClass ( "Vapor Intrusion GUI" ),
  _root( 0x0 ),
  _dimensions( 10, 10, 10 ),
  _cubes()
{
  USUL_TRACE_SCOPE;

  // Setup the cubes
  this->_initCubes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::~VaporIntrusionGUIDocument()
{
  USUL_TRACE_SCOPE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VaporIntrusionGUIDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IVaporIntrusionGUI::IID:
    return static_cast < Usul::Interfaces::IVaporIntrusionGUI* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "vig" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "vig" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  //std::string filename = Usul::File::directory( name, true );
  //filename = filename + Usul::File::base( name );
  //std::cout << "Writing file " << filename << "..." << std::endl;
  //osgDB::writeNodeFile( *_root.get(), filename+ ".ive" );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Filters VaporIntrusionGUIDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Vapor Intrusion Files (*.vig)", "*.vig" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Filters VaporIntrusionGUIDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Filters VaporIntrusionGUIDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Vapor Intrusion Files (*.vig)", "*.vig" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Filters VaporIntrusionGUIDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VaporIntrusionGUIDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  Guard guard( this );
  USUL_TRACE_SCOPE;
  if( false == _root.valid() )
  {
    this->_buildScene( caller );
  }
  return _root.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{

}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cubes
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_initCubes()
{
  Guard guard( this );

  // Clear the cubes
  _cubes.clear();

  // Set the X size to the proper dimensions
  _cubes.resize( _dimensions[0] );
  for( unsigned int x = 0; x < _dimensions[0]; ++x )
  {
    // Set the X size to the proper dimensions
    _cubes.at( x ).resize( _dimensions[1] );
    for( unsigned int y = 0; y < _dimensions[1]; ++y )
    {
      // Set the X size to the proper dimensions
      _cubes.at( x ).at( y ).resize( _dimensions[2] );
      for( unsigned int z = 0; z < _dimensions[2]; ++z )
      {
        Cell cell;

        cell.position = Usul::Math::Vec3f( float( x ), float( y ), float( z ) );
        // Add a default cube
        _cubes.at( x ).at( y ).at( z ) = cell;
      }
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_buildScene ( Unknown *caller )
{
  Guard guard( this );
  if( false == _root.valid() )
  {
    _root = new osg::Group;
  }

  // Remove all the children
  _root->removeChildren( 0, _root->getNumChildren() );

  // Loop through the dimensions and build the cube
  for( unsigned int x = 0; x < _dimensions[0]; ++x )
  {

    for( unsigned int y = 0; y < _dimensions[1]; ++y )
    {
      for( unsigned int z = 0; z < _dimensions[2]; ++z )
      {
        // Set the default ValueType
        Color c ( 0.5, 0.5, 0.5, 0.1 );
        osg::ref_ptr< osg::Material > material ( new osg::Material );
        material->setAmbient( osg::Material::FRONT_AND_BACK, c );
        material->setDiffuse( osg::Material::FRONT_AND_BACK, c );
        _cubes.at( x ).at ( y ).at ( z ).color = c;
        
        // Position of the (0,0,0) corner
        Usul::Math::Vec3f pos ( _cubes.at( x ).at( y ).at( z ).position );

        // Offset
        Usul::Math::Vec3f offset ( _cubes.at( x ).at( y ).at( z ).dimensions );

        // Points
        float px( pos[0] + offset[0] ), py( pos[1] + offset[1] ), pz( pos[2] + offset[2] );
        
        // create the points for the cube
        osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
        p->push_back( osg::Vec3( float( px )    , float( py )    , float( pz ) ) );
        p->push_back( osg::Vec3( float( px + 1 ), float( py )    , float( pz ) ) );
        p->push_back( osg::Vec3( float( px )    , float( py + 1 ), float( pz ) ) );
        p->push_back( osg::Vec3( float( px + 1 ), float( py + 1 ), float( pz ) ) );
        p->push_back( osg::Vec3( float( px )    , float( py )    , float( pz + 1 ) ) );
        p->push_back( osg::Vec3( float( px + 1 ), float( py )    , float( pz + 1 ) ) );
        p->push_back( osg::Vec3( float( px )    , float( py + 1 ), float( pz + 1 ) ) );
        p->push_back( osg::Vec3( float( px + 1 ), float( py + 1 ), float( pz + 1 ) ) );

        // build the sub cube
        osg::ref_ptr< osg::Group > group ( new osg::Group );
        
        // Set the location of the cube
        Usul::Math::Vec3ui location( x, y, z );
        group->addChild( this->_buildTestCube( p.get(), c, location ) );

        // set the material of the cube
        OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
        OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

        // Set the cube
        _cubes.at( x ).at ( y ).at ( z ).group = group.get();

        // Add the cubre to the scene
        _root->addChild( _cubes.at( x ).at( y ).at( z ).group.get() );
      }

    }

  }

  
   
}


///////////////////////////////////////////////////////////////////////////////
//
// Test method for multiview verification
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_buildTestCube( osg::Vec3Array* points, Color c, Usul::Math::Vec3ui location )
{
  Guard guard( this );
  GroupPtr group ( new osg::Group );

  // Create the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );

  // Populate the vertices with a unit cube
  // front face
  vertices->push_back( points->at( 4 ) );vertices->push_back( points->at( 5 ) );vertices->push_back( points->at( 7 ) );vertices->push_back( points->at( 6 ) );
  // back face
  vertices->push_back( points->at( 0 ) );vertices->push_back( points->at( 2 ) );vertices->push_back( points->at( 3 ) );vertices->push_back( points->at( 1 ) );
  // top face
  vertices->push_back( points->at( 6 ) );vertices->push_back( points->at( 7 ) );vertices->push_back( points->at( 3 ) );vertices->push_back( points->at( 2 ) );
  // bottom face
  vertices->push_back( points->at( 4 ) );vertices->push_back( points->at( 0 ) );vertices->push_back( points->at( 1 ) );vertices->push_back( points->at( 5 ) );
  // right face
  vertices->push_back( points->at( 7 ) );vertices->push_back( points->at( 5 ) );vertices->push_back( points->at( 1 ) );vertices->push_back( points->at( 3 ) );
  // left face
  vertices->push_back( points->at( 6 ) );vertices->push_back( points->at( 2 ) );vertices->push_back( points->at( 0 ) );vertices->push_back( points->at( 4 ) );

  // Normals for each face
  osg::Vec3 nfr ( 0.0, 0.0, 1.0 );
  osg::Vec3 nba ( 0.0, 0.0, -1.0 );
  osg::Vec3 nto ( 0.0, 1.0, 0.0 );
  osg::Vec3 nbo ( 0.0, -1.0, 0.0 );
  osg::Vec3 nri ( 1.0, 0.0, 0.0 );
  osg::Vec3 nle ( -1.0, 0.0, 0.0 );

  normals->push_back( nfr );normals->push_back( nfr );normals->push_back( nfr );normals->push_back( nfr );
  normals->push_back( nba );normals->push_back( nba );normals->push_back( nba );normals->push_back( nba ); 
  normals->push_back( nto );normals->push_back( nto );normals->push_back( nto );normals->push_back( nto );
  normals->push_back( nbo );normals->push_back( nbo );normals->push_back( nbo );normals->push_back( nbo );
  normals->push_back( nri );normals->push_back( nri );normals->push_back( nri );normals->push_back( nri );
  normals->push_back( nle );normals->push_back( nle );normals->push_back( nle );normals->push_back( nle );
  
  typedef Usul::Properties::Attribute<Usul::Math::Vec3ui,osg::Referenced> Vec3UserData;
  Vec3UserData::RefPtr ud ( new Vec3UserData );
  ud->value( location );

  // Geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  

  // Set the vertices
  geometry->setVertexArray( vertices.get() );

  // set the normals and set binding to per vertex
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // Colors

  colors->push_back( c );
  // set the colors and set binding to per vertex
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  
  // Add the primitive
  geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );
  
  // Create the geode and add the geometry
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable( geometry.get() );

  // Add the geode to the group
  group->addChild( geode.get() );

  // Set the user data with the cube location
  group->setUserData( ud.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Test method for multiview verification
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::highlightCells( Usul::Math::Vec3ui set, unsigned int depth )
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the transparency
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setAlpha( unsigned int x, unsigned int y, unsigned int z, float alpha )
{
  Guard guard( this );
  OsgTools::State::StateSet::setAlpha( _cubes.at( x ).at( y ).at( z ).group.get(), alpha );
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the transparency
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setAlpha( float alpha )
{
  Guard guard( this );
  OsgTools::State::StateSet::setAlpha( _root.get(), alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the dimensions of the space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::dimensions( Usul::Math::Vec3ui d )
{
  Guard guard( this );
  _dimensions = d;
  this->_initCubes();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the dimensions of the space
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3ui VaporIntrusionGUIDocument::dimensions()
{
  Guard guard( this );
  return _dimensions;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the material
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value )
{
  Guard guard( this );
  _cubes.at( x ).at( y ).at( z ).value = value;
}



///////////////////////////////////////////////////////////////////////////////
//
// Set the material
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setNameAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name )
{
  Guard guard( this );
  _cubes.at( x ).at( y ).at( z ).name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the material
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setMaterial( unsigned int x, unsigned int y, unsigned int z, Usul::Math::Vec4f c )
{
  Guard guard( this );
  osg::Vec4 color ( c[0], c[1], c[2], c[3] );
  OsgTools::State::StateSet::setMaterial( _cubes.at( x ).at( y ).at( z ).group.get(), color, color, 1.0f );
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the material
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f VaporIntrusionGUIDocument::getMaterial( unsigned int x, unsigned int y, unsigned int z )
{
  Guard guard( this );
  return Usul::Math::Vec4f( 0.0, 0.0, 0.0, 1.0 );
}



///////////////////////////////////////////////////////////////////////////////
//
// Request a redraw
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::requestRedraw()
{
  BaseClass::requestRedraw();
}