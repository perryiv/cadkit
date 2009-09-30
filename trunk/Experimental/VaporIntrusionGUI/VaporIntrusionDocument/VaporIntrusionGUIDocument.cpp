
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIDocument.h"
#include "VaporIntrusionGUI/Interfaces/IVPIDelegate.h"

#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Properties/Attribute.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/Strings/Trim.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/MinMax.h"
#include "Usul/CommandLine/Arguments.h"

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
#include "osgManipulator/TabBoxDragger"

#include <iterator>
#include <vector>
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
  _cubes(),
  _xValues(),
  _yValues(),
  _zValues(),
  _inputParameters(),
  _draggerState( false ),
  _gridMaterials(),
  _building( "1", "1", "1", "0", "0", "0", "0", "", "" ),
  _useBuilding( true ),
  _sources(),
  _contaminants(),
  _soils(),
  _cracks(),
  _originalToCurrentIndex(),
  _symmetricalGrid( false )
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
  case VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::IID:
    return static_cast < VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI* > ( this );
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
  
  this->_write( name, caller, progress );
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

 
#if 0
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
        cell.dimensions = Usul::Math::Vec3f( 1.0f, 1.0f, 1.0f );

        // Add a default cube
        _cubes.at( x ).at( y ).at( z ) = cell;
      }
    }
  }
#else
  // size the x values
  _xValues.resize( _dimensions[0] );

  // set the initial value and width
  _xValues.at( 0 ).first = 0.0f - ( static_cast< double > ( _dimensions[0] ) / 2 );
  _xValues.at( 0 ).second = 1.0f;

  // add to the map
  _originalToCurrentIndex[ "X0" ] = 0;

  // set the x values to defaults
  for( unsigned int i = 1; i < _dimensions[0]; ++i )
  {
    double position ( _xValues.at( i - 1 ).first + _xValues.at( i - 1 ).second );
    _xValues.at( i ).first = position;
    _xValues.at( i ).second = 1.0f;

    // key value for map
    std::string key ( Usul::Strings::format( "X", i ) );

    // add to the map
    _originalToCurrentIndex[ key ] = i;
  }

  // size the y values
  _yValues.resize( _dimensions[1] );

  // set the initial value and width
  _yValues.at( 0 ).first = 0.0f - ( static_cast< double > ( _dimensions[1] ) / 2 );
  _yValues.at( 0 ).second = 1.0f;

  // add to the map
  _originalToCurrentIndex[ "Y0" ] = 0;

  // set the x values to defaults
  for( unsigned int j = 1; j < _dimensions[1]; ++j )
  {
    double position ( _yValues.at( j - 1 ).first + _xValues.at( j - 1 ).second );
    _yValues.at( j ).first = position;
    _yValues.at( j ).second = 1.0f;
    
    // key value for map
    std::string key ( Usul::Strings::format( "Y", j ) );

    // add to the map
    _originalToCurrentIndex[ key ] = j;
  }

   // size the x values
  _zValues.resize( _dimensions[2] );

  // set the initial value and width
  _zValues.at( 0 ).first = 0.0f - ( static_cast< double > ( _dimensions[2] ) / 2 );
  _zValues.at( 0 ).second = 1.0f;

  // add to the map
  _originalToCurrentIndex[ "Z0" ] = 0;

  // set the x values to defaults
  for( unsigned int k = 1; k < _dimensions[2]; ++k )
  {
    double position ( _zValues.at( k - 1 ).first + _zValues.at( k - 1 ).second );
    _zValues.at( k ).first = position;
    _zValues.at( k ).second = 1.0f;
    
    // key value for map
    std::string key ( Usul::Strings::format( "Z", k ) );

    // add to the map
    _originalToCurrentIndex[ key ] = k;
  }

#endif

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

  // build the Grid 3D element
  this->_makeGrid();

  // build the building 3D element
  if( true == _useBuilding )
  {
    if( true == this->symmetricalGrid() )
    {
      this->_makeSymmetricalBuilding();
    }
    else
    {
      this->_makeBuilding();
    }
  }

  // make the cracks
  this->_makeCracks();

  // build the contaminant 3D element
  this->_makeSource3D();   
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the grid and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeGrid( )
{
  // get the size of the x domain
  unsigned int xsize ( _xValues.size() );

  // Loop through the dimensions and build the cube
  for( unsigned int x = 0; x < xsize; ++x )
  {
    // get the size of the y domain at x
    unsigned int ysize ( _yValues.size() );

    for( unsigned int y = 0; y < ysize; ++y )
    {
      // get the size of the z domain at xy
      unsigned int zsize ( _zValues.size() );

      for( unsigned int z = 0; z < zsize; ++z )
      {
        // Set the default ValueType
        Color c ( 0.5, 0.5, 0.5, 0.3 );
        osg::ref_ptr< osg::Material > material ( new osg::Material );
        material->setAmbient( osg::Material::FRONT_AND_BACK, c );
        material->setDiffuse( osg::Material::FRONT_AND_BACK, c );
        //_cubes.at( x ).at ( y ).at ( z ).color = c;
        
        // Position of the (0,0,0) corner
        Usul::Math::Vec3f pos ( _xValues.at( x ).first, _yValues.at( y ).first, _zValues.at( z ).first );

        // Offset
        Usul::Math::Vec3f offset ( _xValues.at( x ).second, _yValues.at( y ).second, _zValues.at( z ).second );
        
        // create the points for the cube
        osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
        p->push_back( osg::Vec3( float( pos[0] )            , float( pos[1] )            , float( pos[2]             ) ) );
        p->push_back( osg::Vec3( float( pos[0] + offset[0] ), float( pos[1] )            , float( pos[2]             ) ) );
        p->push_back( osg::Vec3( float( pos[0] )            , float( pos[1] + offset[1] ), float( pos[2]            ) ) );
        p->push_back( osg::Vec3( float( pos[0] + offset[0] ), float( pos[1] + offset[1] ), float( pos[2]             ) ) );
        p->push_back( osg::Vec3( float( pos[0] )            , float( pos[1] )            , float( pos[2] + offset[2] ) ) );
        p->push_back( osg::Vec3( float( pos[0] + offset[0] ), float( pos[1] )            , float( pos[2] + offset[2] ) ) );
        p->push_back( osg::Vec3( float( pos[0] )            , float( pos[1] + offset[1] ), float( pos[2] + offset[2] ) ) );
        p->push_back( osg::Vec3( float( pos[0] + offset[0] ), float( pos[1] + offset[1] ), float( pos[2] + offset[2] ) ) );

        // build the sub cube
        osg::ref_ptr< osg::Group > group ( new osg::Group );
        
        // Set the location of the cube
        Usul::Math::Vec3ui location( x, y, z );
        group->addChild( this->_buildCube( p.get(), c, location ) );

        // set the material of the cube
        OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
        OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

        // Set the cube
        //_cubes.at( x ).at ( y ).at ( z ).group = group.get();

        // Add the cubre to the scene
        _root->addChild( group.get() );
      }

    }

  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Build a plane from the given points
//
///////////////////////////////////////////////////////////////////////////////

osg::Node*	VaporIntrusionGUIDocument::_buildPlane ( osg::Vec3Array* points, osg::Vec4f color )
{
	Guard guard ( this );

	// create a group to hold the plane
	GroupPtr group ( new osg::Group );

   // Create the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( points );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );

  if( points->size() != 4 )
  {
    std::cout << "Invalid number of points passed to _buildPlane!" << std::endl;
    return group.release();
  }
    
  // Normals for each face n ( p3 - p2 ) ^ ( p1 - p2 )
  osg::Vec3 n( ( vertices->at( 3 ) - vertices->at( 1 ) ) ^ ( vertices->at( 0 ) - vertices->at( 1 ) ) );
  normals->push_back( n );normals->push_back( n );normals->push_back( n );normals->push_back( n );
  
  // Geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  
  // Set the vertices
  geometry->setVertexArray( vertices.get() );

  // set the normals and set binding to per vertex
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // Colors
  colors->push_back( color );

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

	// return the group
	return group.release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Create the cracks in the foundation and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeCracks()
{
	Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  // create the visuals for the x direction cracks first
	for( unsigned int i = 0; i < _cracks.first.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.first.at( i ) );

    // set the corners
    float sy( StrToFloat::convert( _building.z ) + StrToFloat::convert( c.value ) );
    float ey ( sy );

    // set the "z" corners
    float sx ( StrToFloat::convert( _building.x ) + StrToFloat::convert( c.start ) );
    float ex ( StrToFloat::convert( _building.x ) + StrToFloat::convert( c.end ) );

    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

    //set the points
    float sd ( StrToFloat::convert( _building.y  ) );
    float ed ( StrToFloat::convert( _building.y  ) + -1 * StrToFloat::convert( _building.h ) );

    points->push_back( osg::Vec3f ( sx, sd, sy ) );
    points->push_back( osg::Vec3f ( ex, sd, sy ) );
    points->push_back( osg::Vec3f ( ex, ed, ey ) );
    points->push_back( osg::Vec3f ( sx, ed, ey ) );

    _root->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 1.0f, 0.0f, 1.0f ) ) );
  }

  // create the visuals for the y direction cracks first
	for( unsigned int i = 0; i < _cracks.second.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.second.at( i ) );

    // set the corners
    float sy( StrToFloat::convert( _building.x ) + StrToFloat::convert( c.value ) );
    float ey ( sy );

    // set the "z" corners
    float sx ( StrToFloat::convert( _building.z ) + StrToFloat::convert( c.start ) );
    float ex ( StrToFloat::convert( _building.z ) + StrToFloat::convert( c.end ) );

    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

    //set the points
    float sd ( StrToFloat::convert( _building.y  ) );
    float ed ( StrToFloat::convert( _building.y  ) + -1 * StrToFloat::convert( _building.h ) );

    points->push_back( osg::Vec3f ( sy, sd, sx ) );
    points->push_back( osg::Vec3f ( sy, sd, ex ) );
    points->push_back( osg::Vec3f ( ey, ed, ex ) );
    points->push_back( osg::Vec3f ( ey, ed, sx ) );

    _root->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 1.0f, 0.0f, 1.0f ) ) );
  }

	
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the building foundation and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeFoundation( osg::Vec3f ll )
{
  Guard guard ( this );
 
  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  // slightly shorten ll
  ll = osg::Vec3f( ll.x() - 0.00001f, ll.y() - 0.00001f, ll.z() - 0.00001f );

   // color for the building
  Color c ( 1.0, 0.0, 1.0, 0.5 );
 
  // Material for the cube
  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setAmbient( osg::Material::FRONT_AND_BACK, c );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, c );

  // Building initial y position
  float ypos ( _yValues.at( _yValues.size() - 1 ).first + StrToFloat::convert( _building.h ) );

  // get the length, width, and height of the building
  osg::Vec3f lhw ( StrToFloat::convert( _building.l ), -1 * StrToFloat::convert( _building.h ), StrToFloat::convert( _building.w ) );

  // create the points for the building
  osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lhw.y(), ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y() + lhw.y(), ll.z()           ) );  
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y()          , ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lhw.y(), ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y() + lhw.y(), ll.z() + lhw.z() ) );

  // location (not used in this context)
  Usul::Math::Vec3ui location( 0, 0, 0 );

  // group
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  // build the cuve
  group->addChild( this->_buildCube( p.get(), c, location ) );

  // set the material of the cube
  OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
  OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

  // Add the cubre to the scene
  _root->addChild( group.get() );

}

///////////////////////////////////////////////////////////////////////////////
//
// Create the building and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeBuilding()
{
  Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

   // color for the building
  Color c ( 0.0, 0.0, 1.0, 1.0 );
 
  // Material for the cube
  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setAmbient( osg::Material::FRONT_AND_BACK, c );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, c );

  // Building initial y position
  float ypos ( _yValues.at( _yValues.size() - 1 ).first + StrToFloat::convert( _building.h ) );

  // get the lower left corner of the building
  osg::Vec3f ll  ( StrToFloat::convert( _building.x ), ypos, StrToFloat::convert( _building.z ) );

  // snap the lower left to the grid
  osg::Vec2f corner ( ll.x(), ll.z() );
  corner = this->_snapToGrid2D( corner );
  ll.x() = corner.x();
  ll.z() = corner.y();

  // update the building
  _building.x = Usul::Strings::format ( ll.x() );
  _building.z = Usul::Strings::format ( ll.z() );
  _building.y = Usul::Strings::format ( ypos );

  // get the length, width, and height of the building
  osg::Vec3f lhw ( StrToFloat::convert( _building.l ), StrToFloat::convert( _building.h ), StrToFloat::convert( _building.w ) );

  // create the points for the building
  osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lhw.y(), ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y() + lhw.y(), ll.z()           ) );  
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y()          , ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lhw.y(), ll.z() + lhw.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lhw.x(), ll.y() + lhw.y(), ll.z() + lhw.z() ) );

  // location (not used in this context)
  Usul::Math::Vec3ui location( 0, 0, 0 );

  // group
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  // build the cuve
  group->addChild( this->_buildCube( p.get(), c, location ) );

  // set the material of the cube
  OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
  OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

  // Add the cubre to the scene
  _root->addChild( group.get() );

  // make the foundation
  this->_makeFoundation( ll );

}
///////////////////////////////////////////////////////////////////////////////
//
// Create the building and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeSymmetricalBuilding( )
{
  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  // calculate the x index value 
  unsigned int xi ( 0 );
  if( _xValues.size() > 0 )
  {
    xi = ( _xValues.size() ) / 2;

    if( xi >= _xValues.size() )
    {
      xi = _xValues.size() - 1;
    }
  }

  // calculate the z index value 
  unsigned int zi ( 0 );
  if( _zValues.size() > 0 )
  {
    zi = ( _zValues.size() ) / 2;

    if( zi >= _zValues.size() )
    {
      zi = _zValues.size() - 1;
    }
  }

  // update the building based on the grid
  std::string newX ( Usul::Convert::Type< double, std::string >::convert( _xValues.at( xi ).first ) );
  std::string newY ( Usul::Convert::Type< double, std::string >::convert( _yValues.at( _yValues.size() - 1 ).first + _yValues.at( _yValues.size() - 1 ).second ) );
  std::string newZ ( Usul::Convert::Type< double, std::string >::convert( _zValues.at( zi ).first ) );
  std::string vol ( "1" );
  Building newB ( _building.l, _building.w, _building.h, newX, newY, newZ, vol, _building.xrate, _building.thickness );
  _building = newB;

  // color for the building
  Color c ( 0.0, 0.0, 1.0, 1.0 );
 
  // Material for the cube
  osg::ref_ptr< osg::Material > material ( new osg::Material );
  material->setAmbient( osg::Material::FRONT_AND_BACK, c );
  material->setDiffuse( osg::Material::FRONT_AND_BACK, c );

  // get the lower left corner of the building
  osg::Vec3f ll  ( StrToFloat::convert( _building.x ), StrToFloat::convert( _building.y ), StrToFloat::convert( _building.z ) );

  // get the length, width, and height of the building
  osg::Vec3f lwh ( StrToFloat::convert( _building.l ), StrToFloat::convert( _building.w ), StrToFloat::convert( _building.h ) );

  // create the points for the building
  osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y()          , ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lwh.y(), ll.z()           ) );
  p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y() + lwh.y(), ll.z()           ) );  
  p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z() + lwh.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y()          , ll.z() + lwh.z() ) );
  p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lwh.y(), ll.z() + lwh.z() ) );
  p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y() + lwh.y(), ll.z() + lwh.z() ) );

  // location (not used in this context)
  Usul::Math::Vec3ui location( 0, 0, 0 );

  // group
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  // build the cuve
  group->addChild( this->_buildCube( p.get(), c, location ) );

  // set the material of the cube
  OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
  OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

  // Add the cubre to the scene
  _root->addChild( group.get() );

  // make the foundation
  this->_makeFoundation( ll );
 
}

///////////////////////////////////////////////////////////////////////////////
//
// Add the contaminant 3D objects to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeSource3D()
{
  Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  for( unsigned int i = 0; i < _sources.size(); ++i )
  {
    // get the contaminant
    Source s ( _sources.at( i ) );

    // color for the contaminant
    Color c ( s.color[0], s.color[1], s.color[2], 0.8 );
   
    // Material for the cube
    osg::ref_ptr< osg::Material > material ( new osg::Material );
    material->setAmbient( osg::Material::FRONT_AND_BACK, c );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, c );

    // get the lower left corner of the contaminant
    osg::Vec3f ll  ( StrToFloat::convert( s.x ), StrToFloat::convert( s.y ), StrToFloat::convert( s.z ) );

    // snap to the grid
    ll = this->_snapToGrid3D( ll );

    // update the source
    _sources.at( i ).x = ll.x();
    _sources.at( i ).y = ll.y();
    _sources.at( i ).z = ll.z();

    // shrink
    ll = osg::Vec3f ( ll.x() - 0.00001, ll.y() - 0.00001, ll.z() - 0.00001 );

    // get the length, width, and height of the contaminant
    osg::Vec3f lwh ( StrToFloat::convert( s.l ), StrToFloat::convert( s.w ), StrToFloat::convert( s.h ) );

    // create the points for the contaminant
    osg::ref_ptr< osg::Vec3Array > p ( new osg::Vec3Array );
    p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z()           ) );
    p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y()          , ll.z()           ) );
    p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lwh.y(), ll.z()           ) );
    p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y() + lwh.y(), ll.z()           ) );  
    p->push_back( osg::Vec3f ( ll.x()          , ll.y()          , ll.z() + lwh.z() ) );
    p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y()          , ll.z() + lwh.z() ) );
    p->push_back( osg::Vec3f ( ll.x()          , ll.y() + lwh.y(), ll.z() + lwh.z() ) );
    p->push_back( osg::Vec3f ( ll.x() + lwh.x(), ll.y() + lwh.y(), ll.z() + lwh.z() ) );

    // location (not used in this context)
    Usul::Math::Vec3ui location( 0, 0, 0 );

    // group
    osg::ref_ptr< osg::Group > group ( new osg::Group );

    // build the cuve
    group->addChild( this->_buildCube( p.get(), c, location ) );

    // set the material of the cube
    OsgTools::State::StateSet::setMaterial( group.get(), material.get() );
    OsgTools::State::StateSet::setAlpha( group.get(), c.a() );

    // Add the cubre to the scene
    _root->addChild( group.get() );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Rebuild the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::rebuildScene()
{
  // rebuild the scene
  this->_buildScene();

  // request a redraw
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
// Test method for multiview verification
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_buildCube( osg::Vec3Array* points, Color c, Usul::Math::Vec3ui location )
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

  // build the cubes based on the current dimensions
  this->_initCubes();

  // Adjust the grid spacing and rebuild the cubes
  this->_adjustGridSpacing();

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

void VaporIntrusionGUIDocument::setValueAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& value )
{
  Guard guard( this );
  
  _cubes.at( x ).at( y ).at( z ).values.at( index ).value = value;
}



///////////////////////////////////////////////////////////////////////////////
//
// Set the material
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setNameAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& name )
{
  Guard guard( this );
  _cubes.at( x ).at( y ).at( z ).values.at( index ).name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a value to cell at xyz with <name> <value>
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name, const std::string& value )
{
  Guard guard( this );
  
  // Create a value to add
  Value v ( name, value );

  // add to the vector of values for the cube at xyz
  _cubes.at( x ).at( y ).at( z ).values.push_back( v );
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a value to cell at xyz with default name and <value>
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value )
{
  Guard guard( this );

  // Create a value to add
  Value v ( "default", value );

  // add to the vector of values for the cube at xyz
  _cubes.at( x ).at( y ).at( z ).values.push_back( v );
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


///////////////////////////////////////////////////////////////////////////////
//
// Write the necessary files for the Vapor Intrusion Process
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_write( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  Guard guard( this );

  // get the directory
  std::string directory ( Usul::File::directory( filename, true ) );

  // get the base
  std::string base ( Usul::File::base( filename ) );

  // Make sure it exists.
  // Usul::File::make ( directory );

  // write the Layers.txt file
  this->_writeLayerFile2( directory );

  // write the coordinates files
  this->_writeCoordinatesFiles( directory );
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the x grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_writeLayerFile2( const std::string& directory ) const
{
  /*
  Guard guard( this );

  // create the filename
  std::string filename ( directory + "/Layer.txt" );

  // create a file handle
  std::ofstream ofs;

  // open the file
  ofs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
   throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // current index for the input parameters
  unsigned int index( 0 );

  for( unsigned int i = 0; i < _categories.size(); ++i )
  {
    //get the category
    Category category = _categories.at( i );
  
    // write the category name header
    ofs << "************** " << category.name <<  " ***********************\n\n"; 

    // write the columns in this category
    for( unsigned int j = 0; j < category.columns.size(); ++j )
    {
      // get the activator column
      InputColumn column ( category.activators.at( j ) );

      // get the activator name
      std::string activatorName( category.activators.at( j ).name );

      // output the activator column
      ofs << Usul::Strings::format( column.value, "\t", column.description, "\n" );

      // find all the activatees for this activator and output their information
      for( unsigned int k = 0; k < category.activatees.size(); ++k )
      {
        // get the activatee name
        std::string activatedBy( category.activatees.at( k ).activatedBy );

        // if the activatee belongs to the activator, output the column
        if( activatorName == activatedBy )
        {
          // get the activatee column
          InputColumn activateeColumn( category.activatees.at( k ) );

          // output the activator column
          ofs << Usul::Strings::format( activateeColumn.value, "\t", activateeColumn.description, "\n" );
        }// end if( activatorName == activateeName )

      }// end for( unsigned int k = 0; k < category.activatees.size(); ++k )      

    }// end for( unsigned int j = 0; j < category.activators.size(); ++j )
    
    // output a newline for formating
    ofs << "\n";

  }// end for( unsigned int i = 0; i < _categories.size(); ++i )


  // close the output file
  ofs.close();
  */
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the x grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_writeLayerFile( const std::string& directory ) const
{
  // create the filename
  std::string filename ( directory + "/Layer.txt" );

  // create a file handle
  std::ofstream ofs;

  // open the file
  ofs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
   throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // current index for the input parameters
  unsigned int index( 0 );

  ofs << "**************  Model domain ***********************\n";
  ofs << "\n";
  ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
  ++index;
  ofs << "\n";
  ofs << "***********  Grid Size  *****************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 3; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "************** Building Parameters *******************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 18; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "******************** Soil Physical Characteristics ***************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 11; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "**************  Constituents Properties  **************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 41; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "******************** Pressure input ************************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 24; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "**************** Pressure boundary *************************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 2; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "**************** Algorithm parameters ***********************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 78; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";
  ofs << "**************** name of files to output data ***************************\n";
  ofs << "\n";
  for( unsigned int i = 0; i < 25; ++ i )
  {
    ofs << Usul::Strings::format( _inputParameters.at( index ).second.first, "\t", _inputParameters.at( index ).second.second, "\n" );
    ++index;
  }
  ofs << "\n";

  // end of output...close the file
  ofs.close();

}


///////////////////////////////////////////////////////////////////////////////
//
// Get the x grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_writeCoordinatesFiles( const std::string& directory ) const
{
  // create the filenames
  std::string filenameX ( directory + "/COORDx.txt" );
  std::string filenameY ( directory + "/COORDy.txt" );
  std::string filenameZ ( directory + "/COORDz.txt" );

  // create a file handle
  std::ofstream ofs;

  // open the file
  ofs.open( filenameX.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
   throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filenameX );

  // write the x coords
  for( unsigned int i = 0; i < _xValues.size(); ++ i )
  {
    ofs << Usul::Strings::format( _xValues.at( i ).first, "\t", _xValues.at( i ).second, "\n" );
  }

  // close the file
  ofs.close();

  //------------------------------------------------------------

  // open the file
  ofs.open( filenameY.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
   throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filenameY );

  // write the x coords
  for( unsigned int i = 0; i < _yValues.size(); ++ i )
  {
    ofs << Usul::Strings::format( _yValues.at( i ).first, "\t", _yValues.at( i ).second, "\n" );
  }

  // close the file
  ofs.close();

  //------------------------------------------------------------

   // open the file
  ofs.open( filenameZ.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
   throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filenameZ );

  // write the x coords
  for( unsigned int i = 0; i < _zValues.size(); ++ i )
  {
    ofs << Usul::Strings::format( _zValues.at( i ).first, "\t", _zValues.at( i ).second, "\n" );
  }

  // close the file
  ofs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the x grid points
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridPoints VaporIntrusionGUIDocument::getXGrid()
{
  Guard guard( this );
  return _xValues;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the y grid points
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridPoints VaporIntrusionGUIDocument::getYGrid()
{
  Guard guard( this );
  return _yValues;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the z grid points
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridPoints VaporIntrusionGUIDocument::getZGrid()
{
  Guard guard( this );
  return _zValues;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the x grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setXGrid( GridPoints points )
{
  Guard guard( this );
  _xValues = points;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the y grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setYGrid( GridPoints points )
{
  Guard guard( this );
  _yValues = points;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the z grid points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setZGrid( GridPoints points )
{
  Guard guard( this );
  _zValues = points;
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the "allof" values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readAllofValues( unsigned int& index, InputColumn& column, const StringVec& sv )
{
  // Check for end of section
  bool end ( false );

  while( false == end && index < sv.size() )
  {
    if( ( index + 2 ) < sv.size() )
    {
     // Get the next string and check for section keyword
      std::string keyword ( sv.at( index ) );

      if( keyword != "allof" && keyword != "oneof" )
      {
        // get the activator name and value
        std::string aName ( keyword );
        std::string aComp ( sv.at( index + 1 ) );
        std::string aValue ( sv.at( index + 2 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( aName, ' ' );
        Usul::Strings::trimRight( aName, ' ' );

        // get the comparitor conversion
        int comparitor ( this->_getComparitor( aComp ) );

        // create a activator pair and add it to the activator list on the column
        ActivatorValue aV ( comparitor, aValue );
        ActivatorPair aPair ( aName, aV );
        column.allofActivators.push_back( aPair );

        // increment the index
        index += 3;
      }
      else
      {
        end = true;
      }
    }
    else
    {
      // There is a mismatch in number of remaining values
      // Should probably throw an invalid input error here

      // TODO: throw here instead of warning message
      std::cout << "Warning: invalid number of activator pair combination(s)!" << std::endl;
      end = true;
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Read the "oneof" values
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readOneofValues( unsigned int& index, InputColumn& column, const StringVec& sv )
{
  // Check for end of section
  bool end ( false );

  while( false == end && index < sv.size() )
  {
    if( ( index + 2 ) < sv.size() )
    {
      // Get the next string and check for section keyword
      std::string keyword ( sv.at( index ) );

      if( keyword != "allof" && keyword != "oneof" )
      {
        // get the activator name and value
        std::string aName ( keyword );
        std::string aComp ( sv.at( index + 1 ) );
        std::string aValue ( sv.at( index + 2 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( aName, ' ' );
        Usul::Strings::trimRight( aName, ' ' );

        // get the comparitor conversion
        int comparitor ( this->_getComparitor( aComp ) );

        // create a activator pair and add it to the activator list on the column
        ActivatorValue aV ( comparitor, aValue );
        ActivatorPair aPair ( aName, aV );
        column.oneofActivators.push_back( aPair );

        // increment the index
        index += 3;
      }
      else
      {
        end = true;
      }
    }
    else
    {
      // There is a mismatch in number of remaining values
      // Should probably throw an invalid input error here

      // TODO: throw here instead of warning message
      std::cout << "Warning: invalid number of activator pair combination(s)!" << std::endl;
      end = true;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the config file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readConfigFile( const std::string& catName, const std::string& filename )
{
  // useful typedef
  typedef std::vector< std::string > StringVec;

  // Create a category for this input file
  Category category( catName, filename );

  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // feedback.
  std::cout << "Reading parameter file: " << filename << std::endl;

  // create a dock for this category
  // this->_createDock( catName );

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );
  {
    // create a buffer
    char buffer[bufSize+1];

    // Temp string
    std::string tStr;

    do
    {
      // get a line
      ifs.getline ( buffer, bufSize );

      

      // grab the string
      tStr = buffer;
    }
    while( tStr.at( 0 ) == '#' );

    // separate the strings
    
    StringVec sv;
    Usul::Strings::split( tStr, ",", false, sv );

    // debugging
    //std::cout << "Column Names: " << tStr << std::endl;

    // assign the headers
    category.columnNames = sv;
  }

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];

    // get a line
    ifs.getline ( buffer, bufSize );

    

    // create a string from the buffer
    std::string tStr ( buffer );

    if( tStr.at( 0 ) != '#' )
    {

      // separate the strings
      StringVec sv;
      Usul::Strings::split( tStr, ",", false, sv );

      // debugging
      //std::cout << "Reading: " << tStr << std::endl;
      
      // make sure all the columns are there
      if( sv.size() >= 4 )
      {
        // temp column to hold the input line
        // #Name,Value,Description,Type,Activators
        std::string name          ( sv.at( 0 ) );
        std::string value         ( sv.at( 1 ) );
        std::string description   ( sv.at( 2 ) );
        std::string type          ( sv.at( 3 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( name, ' ' );
        Usul::Strings::trimRight( name, ' ' );

        // if the type is a scalar then add it to the dock instead of the dialog
        if( type == "scalar" )
        {
          this->_addMaterialToDock( catName, name, value );
        }
        else
        {
          InputColumn column( name, value, description, type ); 

          
          // get the section descriptor
          if( sv.size() > 4 )
          {
            // index of the current column
            unsigned int index ( 4 );

            while( index < sv.size() )
            {
              // read the section identifier
              std::string section ( sv.at( index ) );

              // increment the index
              ++index;

              if( section == "allof" )
              {
                // read the allof section
                this->_readAllofValues( index, column, sv );
              }
              if( section == "oneof" )
              {
                // read the oneof section
                this->_readOneofValues( index, column, sv );
              }

            }

           
          }

          // add the column to the list of columns for this category
          category.columns[ name ] =  column;   
        }

      }// end for for activators read   

    }// end if for valid entry found

  }// end while read for file parsing

  // add the category to the list of categories
  _categories.push_back( category );

  ifs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the parameter file
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::ParameterList VaporIntrusionGUIDocument::parameters()
{
  Guard guard( this );
  return _inputParameters;
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the parameter file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::parameters( ParameterList plist )
{
  Guard guard( this );
  _inputParameters = plist;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the categories for the input file
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Categories VaporIntrusionGUIDocument::categories()
{
  Guard guard( this );
  return _categories;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the categories for the input file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::categories( Categories c )
{
  Guard guard( this );
  _categories = c;
}


///////////////////////////////////////////////////////////////////////////////
//
// Initialize the document
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::initialize()
{
  Guard guard( this );

  // initialize the grid materials
  this->_initializeGridMaterials();

  // set up the input variables
  _configFileName = Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "vpi_config.vpi";
  this->_readInitializationFile( _configFileName );

  // read the contaminants file
  std::string contamintantFilename ( Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "contaminants.vpi" );
  this->_readContaminants( contamintantFilename );

  // read the sources file
  std::string sourceFilename ( Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "sources.vpi" );
  this->_readSources( sourceFilename );

  // read the soils file
  std::string soilFilename ( Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "soils.vpi" );
  this->_readSoils( soilFilename );

  // read the cracks file
  std::string cracksFilename ( Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "cracks.vpi" );
  this->_readCracks( cracksFilename );
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the Sources file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readSources( const std::string& filename )
{
 Guard guard ( this );

  // useful typedef
  typedef std::vector< std::string > StringVec;

  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
  {
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No Presets loaded for Sources" ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Reading Sources file: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];

    // get a line
    ifs.getline ( buffer, bufSize );

    // create a string from the buffer
    std::string tStr ( buffer );

    if( tStr.at( 0 ) != '#' )
    {

      // separate the strings
      StringVec sv;
      Usul::Strings::split( tStr, ",", false, sv );

      // debugging
      //std::cout << "Reading: " << tStr << std::endl;
      
      // make sure all the columns are there
      if( sv.size() == 7 )
      {
        // temp column to hold the input line
        std::string name    ( sv.at( 0 ) );
        std::string xpos    ( sv.at( 1 ) );
        std::string ypos    ( sv.at( 2 ) );
        std::string zpos    ( sv.at( 3 ) );
        std::string l       ( sv.at( 4 ) );
        std::string w       ( sv.at( 5 ) );
        std::string h       ( sv.at( 6 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( name, ' ' );
        Usul::Strings::trimRight( name, ' ' );

        // create a temp contaminant
        Contaminants c;
        Source s ( l, w, h, xpos, ypos, zpos, name, c );

        // add to the list of contaminants
        _sources.push_back( s );

        // increment the number of contaminants read
        ++lineNumber;

      }// end for for activators read   

    }// end if for valid entry found

  }// end while read for file parsing

  ifs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the Soils file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readSoils( const std::string& filename )
{
 Guard guard ( this );

  // useful typedef
  typedef std::vector< std::string > StringVec;

  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
  {
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No Presets loaded for Soils" ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Reading Soils file: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];

    // get a line
    ifs.getline ( buffer, bufSize );

    // create a string from the buffer
    std::string tStr ( buffer );

    if( tStr.at( 0 ) != '#' )
    {

      // separate the strings
      StringVec sv;
      Usul::Strings::split( tStr, ",", false, sv );

      // debugging
      //std::cout << "Reading: " << tStr << std::endl;
      
      // make sure all the columns are there
      if( sv.size() == 7 )
      {
        // temp column to hold the input line
        // #Name,Value,Description,Type,Activators
        std::string name          ( sv.at( 0 ) );
        std::string elevation     ( sv.at( 1 ) );
        std::string porosity      ( sv.at( 2 ) );
        std::string H2OPorosity   ( sv.at( 3 ) );
        std::string carbon        ( sv.at( 4 ) );
        std::string permability   ( sv.at( 5 ) );
        std::string viscosity     ( sv.at( 6 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( name, ' ' );
        Usul::Strings::trimRight( name, ' ' );

        // create a temp contaminant
        Soil s ( name, elevation, porosity, H2OPorosity, carbon, permability, viscosity );

        // add to the list of contaminants
        _soils.push_back( s );

        // increment the number of contaminants read
        ++lineNumber;

      }// end for for activators read   

    }// end if for valid entry found

  }// end while read for file parsing

  ifs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the Cracks file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readCracks( const std::string& filename )
{
 //Guard guard ( this );

 // // useful typedef
 // typedef std::vector< std::string > StringVec;

 // // create a file handle
 // std::ifstream ifs;

 // // open the file
 // ifs.open( filename.c_str() );

 // /// make sure the file was opened
 // if( false == ifs.is_open() )
 // {
 //   std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No Presets loaded for Cracks" ) << std::endl;
 //   return;
 // }

 // // feedback.
 // std::cout << "Reading Cracks file: " << filename << std::endl;

 // // buffer size
 // const unsigned long int bufSize ( 4095 );

 // // line number
 // unsigned int lineNumber ( 0 );

 // // parse the file
 // while( EOF != ifs.peek() )
 // {
 //   // create a buffer
 //   char buffer[bufSize+1];

 //   // get a line
 //   ifs.getline ( buffer, bufSize );

 //   // create a string from the buffer
 //   std::string tStr ( buffer );

 //   if( tStr.at( 0 ) != '#' )
 //   {

 //     // separate the strings
 //     StringVec sv;
 //     Usul::Strings::split( tStr, ",", false, sv );

 //     // debugging
 //     //std::cout << "Reading: " << tStr << std::endl;
 //     
 //     // make sure all the columns are there
 //     if( sv.size() == 5 )
 //     {
 //       // temp column to hold the input line
 //       // #Name,Value,Description,Type,Activators
 //       std::string sx    ( sv.at( 0 ) );
 //       std::string sy    ( sv.at( 1 ) );
 //       std::string ex    ( sv.at( 2 ) );
 //       std::string ey    ( sv.at( 3 ) );
 //       std::string w     ( sv.at( 4 ) );

 //       // create a temp contaminant
 //       Crack c ( sx, sy, ex, ey, w );

 //       // add to the list of contaminants
 //       _cracks.push_back( c );

 //       // increment the number of contaminants read
 //       ++lineNumber;

 //     }// end for for activators read   

 //   }// end if for valid entry found

 // }// end while read for file parsing

 // ifs.close();

}



///////////////////////////////////////////////////////////////////////////////
//
// Read the contaminants file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readContaminants( const std::string& filename )
{
  Guard guard ( this );

  // useful typedef
  typedef std::vector< std::string > StringVec;

  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
  {
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No Presets loaded for Contaminants" ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Reading Contaminants file: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];

    // get a line
    ifs.getline ( buffer, bufSize );

    // create a string from the buffer
    std::string tStr ( buffer );

    if( tStr.at( 0 ) != '#' )
    {

      // separate the strings
      StringVec sv;
      Usul::Strings::split( tStr, ",", false, sv );

      // debugging
      //std::cout << "Reading: " << tStr << std::endl;
      
      // make sure all the columns are there
      if( sv.size() == 6 )
      {
        // temp column to hold the input line
        // #Name,Value,Description,Type,Activators
        std::string name      ( sv.at( 0 ) );
        std::string henry     ( sv.at( 1 ) );
        std::string koc       ( sv.at( 2 ) );
        std::string diffair   ( sv.at( 3 ) );
        std::string diffh2o   ( sv.at( 4 ) );
        std::string atmoconc  ( sv.at( 5 ) );

        // trim trailing and leading white space
        Usul::Strings::trimLeft( name, ' ' );
        Usul::Strings::trimRight( name, ' ' );

        // create a temp contaminant
        Contaminant c ( lineNumber, name, henry, koc, diffair, diffh2o, atmoconc );

        // add to the list of contaminants
        _contaminants.push_back( c );

        // increment the number of contaminants read
        ++lineNumber;

      }// end for for activators read   

    }// end if for valid entry found

  }// end while read for file parsing

  ifs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the initialization file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_initializeGridMaterials()
{
  Guard guard ( this );

  for( unsigned int i = 0; i < _dimensions[0]; ++i )
  {
    for( unsigned int j = 0; j < _dimensions[1]; ++j )
    {
      for( unsigned int k = 0; k < _dimensions[2]; ++k )
      {
        // map key string
        std::string key ( Usul::Strings::format( "x", i, "y", j, "z",k ) );

        // empty material at grid point ijk
        GridMaterial material ( i, j, k );

        // add the material to the grid map
        _gridMaterials[key] = material;
      }
    }
  }  
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the initialization file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readInitializationFile( const std::string& filename )
{
  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // Feedback
  std::cout << "Reading Initialization file: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];   

    // create a string from the buffer
    std::string tStr;
    
    do
    {
      // get a line
      ifs.getline ( buffer, bufSize );

      // grab the string
      tStr = buffer;
    }
    while( tStr.at( 0 ) == '#' );

    // separate the strings
    typedef std::vector< std::string > StringVec;
    StringVec sv;
    Usul::Strings::split( tStr, ",", false, sv );

    if( sv.size() >=2 )
    {
      this->_readConfigFile( sv.at( 0 ), sv.at( 1 ) );
    }

  }// end of while loop
}


///////////////////////////////////////////////////////////////////////////////
//
// Read the config file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::readConfigFile( const std::string& name, const std::string& filename )
{
  Guard guard( this );

  // read the file
  this->_readConfigFile( name, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the list of categories with the supplied category
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::updateCategory( Category category )
{
  Guard guard( this );

  for( unsigned int i = 0; i < _categories.size(); ++i )
  {
    if( _categories.at( i ).name == category.name )
    {
      _categories.at( i ) = category;
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the int value of the comparitor
//
///////////////////////////////////////////////////////////////////////////////

int VaporIntrusionGUIDocument::_getComparitor( const std::string& comparitor )
{
  Guard guard( this );

  // useful typedef
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IV;

  if( comparitor == "LESS_THAN" )
  {
    return IV::LESS_THAN;
  }

  if( comparitor == "GREATER_THAN" )
  {
    return IV::GREATER_THAN;
  }

  if( comparitor == "LESS_THAN_OR_EQUAL" )
  {
    return IV::LESS_THAN_OR_EQUAL;
  }

  if( comparitor == "GREATER_THAN_OR_EQUAL" )
  {
    return IV::GREATER_THAN_OR_EQUAL;
  }

  if( comparitor == "NOT_EQUAL" )
  {
    return IV::NOT_EQUAL;
  }

  if( comparitor == "SIZEOF_EQUAL" )
  {
    return IV::SIZEOF_EQUAL;
  }

  // default return equal
  return IV::EQUAL;
}


///////////////////////////////////////////////////////////////////////////////
//
// check to see if the given value matches
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::checkValue( int comparitor, const std::string& value1, const std::string& value2 )
{
  Guard guard( this );

  // useful typedef
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IV;

  switch( comparitor )
  {
    // values should be equal
    case IV::EQUAL:
    {
      if( value1 == value2 )
      {
        return true;
      }
      break;
    }    

    // values should not be equal
    case IV::NOT_EQUAL:
    {
      if( value1 != value2 )
      {
        return true;
      }
      break;
    }    

    // value1 is greater than value2
    case IV::GREATER_THAN:
    {
      // convert value1 to a double
      double v1 ( Usul::Convert::Type< std::string, double >::convert( value1 ) );

      // convert value2 to a double
      double v2 ( Usul::Convert::Type< std::string, double >::convert( value2 ) );

      if( v1 > v2 )
      {
        return true;
      }

      break;
    }    

    // values should not be equal
    case IV::LESS_THAN:
    {
      // convert value1 to a double
      double v1 ( Usul::Convert::Type< std::string, double >::convert( value1 ) );

      // convert value2 to a double
      double v2 ( Usul::Convert::Type< std::string, double >::convert( value2 ) );

      if( v1 < v2 )
      {
        return true;
      }
      break;
    }    

    // values should not be equal
    case IV::GREATER_THAN_OR_EQUAL:
    {
      // convert value1 to a double
      double v1 ( Usul::Convert::Type< std::string, double >::convert( value1 ) );

      // convert value2 to a double
      double v2 ( Usul::Convert::Type< std::string, double >::convert( value2 ) );

      if( v1 >= v2 )
      {
        return true;
      }

      break;
    }

    // values should not be equal
    case IV::LESS_THAN_OR_EQUAL:
    {

      // convert value1 to a double
      double v1 ( Usul::Convert::Type< std::string, double >::convert( value1 ) );

      // convert value2 to a double
      double v2 ( Usul::Convert::Type< std::string, double >::convert( value2 ) );

      if( v1 <= v2 )
      {
        return true;
      }

      break;
    }
      
    // default behavior -- do nothing
    default:
      break;


  };

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Is the dragger active?
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::draggerActive()
{
  return _draggerState;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the dragger activation state
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::draggerActive( bool value )
{
  Guard guard( this );

  //if( true == value )
  //{
  //  this->_activateDragger();
  //}
  //else
  //{
  //  this->_buildScene( 0x0 );
  //}

  _draggerState = value;
}


///////////////////////////////////////////////////////////////////////////////
//
// Create and add the dragger to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_activateDragger()
{
  Guard guard( this );
  
  //re-build the scene
  this->_buildScene( 0x0 );

  // create a dragger temp
  osg::ref_ptr< osgManipulator::TabBoxDragger > dragger ( new osgManipulator::TabBoxDragger );
  dragger->setupDefaultGeometry();

  //add the dragger to the scene
  _root->addChild( dragger.get() );

}


///////////////////////////////////////////////////////////////////////////////
//
// Create and add the dragger to the scene
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridMaterials VaporIntrusionGUIDocument::gridMaterials()
{
  Guard guard ( this );

  return _gridMaterials;
}


///////////////////////////////////////////////////////////////////////////////
//
// Create and add the dragger to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::gridMaterials( GridMaterials gm )
{
  Guard guard ( this );

  _gridMaterials = gm;
}


///////////////////////////////////////////////////////////////////////////////
//
// Create and add the dragger to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_createDock( const std::string& name )
{
  Guard guard ( this );

  // query for the delegate interface
  VaporIntrusionGUI::Interfaces::IVPIDelegate::QueryPtr delegatePtr ( this->delegate() );
  
  // check for valid
  if( false == delegatePtr.valid() )
    return;

  // add the dock
  delegatePtr->addDock( name );

}


///////////////////////////////////////////////////////////////////////////////
//
// Create and add the dragger to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_addMaterialToDock( const std::string& dock, const std::string& name, const std::string& value )
{
  Guard guard ( this );

   // query for the delegate interface
  VaporIntrusionGUI::Interfaces::IVPIDelegate::QueryPtr delegatePtr ( this->delegate() );
  
  // check for valid
  if( false == delegatePtr.valid() )
    return;

  // add to the dock
  delegatePtr->addToDock( dock, name, value );

}


///////////////////////////////////////////////////////////////////////////////
//
// Set the building 
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::building( Building b )
{
  Guard guard ( this );
  _building = b;

}


///////////////////////////////////////////////////////////////////////////////
//
// Get the building
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Building VaporIntrusionGUIDocument::building()
{
  Guard guard ( this );
  return _building;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set whether or not there is a building present
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::useBuilding( bool value )
{
  Guard guard ( this );

  _useBuilding = value;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get whether or not there is a building present
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::useBuilding()
{
  Guard guard ( this );

  return _useBuilding;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the sources
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::sources( Sources s )
{
  Guard guard ( this );

  _sources = s;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the sources
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Sources VaporIntrusionGUIDocument::sources()
{
  Guard guard ( this );

  return _sources;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the contaminants
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::contaminants( Contaminants c )
{
  Guard guard ( this );
  _contaminants = c;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the contaminants
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Contaminants VaporIntrusionGUIDocument::contaminants()
{
  Guard guard ( this );
  return _contaminants;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the soils
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::soils( Soils s )
{
  Guard guard ( this );
  _soils = s;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the soils
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Soils VaporIntrusionGUIDocument::soils()
{
  Guard guard ( this );
  return _soils;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the cracks
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::cracks( CracksPair c )
{
  Guard guard ( this );
  _cracks = c;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the cracks
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::CracksPair VaporIntrusionGUIDocument::cracks()
{
  Guard guard ( this );
  return _cracks;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the grid refinements
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::refinements( GridRefinements gr )
{
  Guard guard ( this );
  _refinements = gr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the grid refinements
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridRefinements VaporIntrusionGUIDocument::refinements()
{
  Guard guard ( this );
  return _refinements;
}


///////////////////////////////////////////////////////////////////////////////
//
// Insert <numToAdd> number of gridPoints from <startPoint> until <endPoint>
// for axis <axis>
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_insertGridSpacing( const std::string& axis, unsigned int startPoint, unsigned int endPoint, unsigned int numToAdd )
{
  Guard guard ( this );

  GridPoints axisGrid ( this->_getGridFromAxis( axis ) );
  GridPoints grid;

  float distance ( ( static_cast< float > ( endPoint ) - static_cast< float > ( startPoint ) ) / numToAdd );

  std::string key( Usul::Strings::format( axis, startPoint ) );
  unsigned int index ( _originalToCurrentIndex[ key ] );

  for( unsigned int i = 0; i < index; ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  float position ( axisGrid.at( index ).first );
  for( unsigned int i = 0; i < numToAdd; ++i )
  {
    grid.push_back( GridPoint( static_cast< double > ( position ), static_cast< double > ( distance ) ) );
  
    position += distance;
  }
  
  for( unsigned int i = index + 1; i < axisGrid.size(); ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  this->_setGridFromAxis( axis, grid );

  for( GridMap::iterator iter = _originalToCurrentIndex.begin(); iter != _originalToCurrentIndex.end(); ++iter )
  {
    std::string iterKey ( iter->first );
    std::string iterAxis ( iterKey.substr( 0, 1 ) );

    if( iterAxis == axis )
    {
      unsigned int iterKeyIndex ( Usul::Convert::Type< std::string, unsigned int >::convert( iterKey.substr( 1, iterKey.length() ) ) );
      unsigned int keyIndex ( Usul::Convert::Type< std::string, unsigned int >::convert( key.substr( 1, key.length() ) ) );

      if( iterKeyIndex > keyIndex  )
      {
        unsigned int newValue ( iter->second + numToAdd );
        
        _originalToCurrentIndex[ iterKey ] = newValue;
        
      }
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Return the grid points for the specified axis
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridPoints VaporIntrusionGUIDocument::_getGridFromAxis( const std::string& axis )
{
  GridPoints grid;

  if( axis == "X" )
    grid = _xValues;

  if( axis == "Y" )
    grid = _yValues;

  if( axis == "Z" )
    grid = _zValues;

  return grid;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the grid points for the specified axis
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_setGridFromAxis( const std::string& axis, GridPoints grid )
{
  
  if( axis == "X" )
  {
    _xValues = grid;
  }

  if( axis == "Y" )
  {
    _yValues = grid;
  }

  if( axis == "Z" )
  {
    _zValues = grid;
  }

}

///////////////////////////////////////////////////////////////////////////////
//
// Adjust the grid spacing based on the grid dimensions and the grid
// refinement options specified by the user.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_adjustGridSpacing()
{
  Guard guard ( this );

  // get the dimensions
  unsigned int xmax ( _dimensions[0] );
  unsigned int ymax ( _dimensions[1] );
  unsigned int zmax ( _dimensions[2] );

  // parse the refinements for changes that need to be made to the
  // grid spacing and dimensions

  for( unsigned int i = 0; i < _refinements.size(); ++i )
  {
    // get the refinement
    GridRefinement gr ( _refinements.at( i ) );
  
    // get the start and end values
    unsigned int start ( Usul::Convert::Type< std::string, unsigned int >::convert ( gr.start ) );
    unsigned int end   ( Usul::Convert::Type< std::string, unsigned int >::convert ( gr.end   ) );

    // get the difference and calculate the number of grid points to add
    float value ( Usul::Convert::Type< std::string, float >::convert ( gr.value ) );
    float diff ( 1.0 / value );
    int numToAdd ( static_cast< int > ( diff ) - 1 );

    // insert between start and end
    this->_insertGridSpacing( gr.axis, start, end, numToAdd );

    // add to the xmax axis
    if( gr.axis == "X" )
    {
      xmax += numToAdd;
    }

    // add to the ymax axis
    if( gr.axis == "Y" )
    {
      ymax += numToAdd;
    }

    // add to the xmax axis
    if( gr.axis == "Z" )
    {
      zmax += numToAdd;
    }
  }

  // adjust the dimensions
  _dimensions = Usul::Math::Vec3ui( xmax, ymax, zmax );

}


///////////////////////////////////////////////////////////////////////////////
//
// Get the grid symmetry
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::symmetricalGrid()
{
  Guard guard ( this );
  return _symmetricalGrid;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the grid symmetry
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::symmetricalGrid( bool value )
{
  Guard guard ( this );

  _symmetricalGrid = value;
}


///////////////////////////////////////////////////////////////////////////////
//
// Find the closest xy grid point to the given input corner
// TODO: Improve search speed by using better search algorithm
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec2f VaporIntrusionGUIDocument::_snapToGrid2D( osg::Vec2f corner )
{
  // get the starting value for the x values
  float minXDistance ( abs ( _xValues.at( 0 ).first - corner.x() ) );
  int currXIndex ( 0 );

  // check the rest of the x values to find the closest point to the corner
  for( unsigned int i = 1; i < _xValues.size(); ++i )
  {
    float temp ( abs ( _xValues.at( i ).first - corner.x() ) ); 

    if( temp < minXDistance )
    {
      minXDistance = temp;
      currXIndex = i;
    }

  }
  
  // get the starting value for the x values
  float minYDistance ( abs ( _yValues.at( 0 ).first - corner.y() ) );
  int currYIndex ( 0 );

  for( unsigned int j = 0; j < _yValues.size(); ++j )
  {
    float temp ( abs ( _yValues.at( j ).first - corner.y() ) ); 

    if( temp < minYDistance )
    {
      minYDistance = temp;
      currYIndex = j;
    }
  }

  osg::Vec2f value ( _xValues.at( currXIndex ).first, _yValues.at( currYIndex ).first );

  // feedback
  std::cout << "Corner fit to: ( " << value.x() << ", " << value.y() << " )." << std::endl;

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
// Find the closest xyz grid point to the given input corner
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f VaporIntrusionGUIDocument::_snapToGrid3D( osg::Vec3f corner )
{
  // get the starting value for the x values
  float minXDistance ( abs ( _xValues.at( 0 ).first - corner.x() ) );
  int currXIndex ( 0 );

  // check the rest of the x values to find the closest point to the corner
  for( unsigned int i = 1; i < _xValues.size(); ++i )
  {
    float temp ( abs ( _xValues.at( i ).first - corner.x() ) ); 

    if( temp < minXDistance )
    {
      minXDistance = temp;
      currXIndex = i;
    }

  }
  
  // get the starting value for the x values
  float minYDistance ( abs ( _yValues.at( 0 ).first - corner.y() ) );
  int currYIndex ( 0 );

  for( unsigned int j = 0; j < _yValues.size(); ++j )
  {
    float temp ( abs ( _yValues.at( j ).first - corner.y() ) ); 

    if( temp < minYDistance )
    {
      minYDistance = temp;
      currYIndex = j;
    }
  }

  // get the starting value for the x values
  float minZDistance ( abs ( _zValues.at( 0 ).first - corner.z() ) );
  int currZIndex ( 0 );

  for( unsigned int k = 0; k < _zValues.size(); ++k )
  {
    float temp ( abs ( _zValues.at( k ).first - corner.z() ) ); 

    if( temp < minZDistance )
    {
      minZDistance = temp;
      currZIndex = k;
    }
  }

  osg::Vec3f value ( _xValues.at( currXIndex ).first, _yValues.at( currYIndex ).first, _zValues.at( currZIndex ).first );

  // feedback
  std::cout << "Corner fit to: ( " << value.x() << ", " << value.y() << ", " << value.z() << " )." << std::endl;

  return value;
}