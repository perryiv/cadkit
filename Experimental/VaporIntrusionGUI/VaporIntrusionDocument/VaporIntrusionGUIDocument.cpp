

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
#include "GenericIndexToggle.h"

// Dialogs
#include "VaporIntrusionGUI/VaporIntrusionDelegate/SourcePropertiesDialog.h"

#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Exceptions/Canceled.h"

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
#include "OsgTools/State/PolygonMode.h"
#include "OsgTools/Box.h"
#include "OsgTools/Render/Viewer.h"

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
#include "osg/PolygonMode"

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
  _root2D( 0x0 ),
  _dimensions( 10, 10, 10 ),
  _initialSpacing( 1.0f, 1.0f, 1.0f ),
  _cubes(),
  _xValues(),
  _yValues(),
  _zValues(),
  _inputParameters(),
  _draggerState( false ),
  _gridMaterials(),
  _building( "1", "1", "1", "0", "0", "0", "0", "0", "", "" ),
  _useBuilding( false ),
  _sources(),
  _chemicals(),
  _soils(),
  _cracks(),
  _axisPoints(),
  _originalToCurrentIndex(),
  _symmetricalGrid( false ),
  _showGrid( true ),
  _showBuilding( true ),
  _showFoundation( true ),
  _showSources( true ),
  _showCracks( true ),
  _showLabels( true ),
  _maxCrackGridDistance( 0.2f ),
  _buildMode2D( IVPI::BUILD_MODE_GRID_EDIT ),
  _editGridMode2D( IVPI::EDIT_MODE_IDLE ),
  _objectMode( IVPI::OBJECT_NOTHING ),
  _viewMode2D( IVPI::VIEW_MODE_2D_XY ),
  _currentObject(),
  _minimumGridDistance( 0.1f ),
  _root2DHi( new osg::Group ),
  _object2D( new osg::Group ),
  _cracks2D( new osg::Group ),
  _labels2D( new osg::Group ),
  _mouseXCoord( 0.0f ),
  _mouseYCoord( 0.0f ),
  _textXPos( 0 ),
  _textYPos( 0 ),
  _placementCrack()
{
  USUL_TRACE_SCOPE;
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
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast<Usul::Interfaces::IMenuAdd*> ( this );
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

  // get the build options
  std::string renderOption ( options.find( "Dimension" )->second );
 
  if( renderOption == "3D" )
  {
    if( false == _root.valid())
    {
      this->_build3DScene( caller );      
    }
    return _root;
  }
  else
  {
    if( false == _root2D.valid() )
    {
      this->_build2DScene( caller );      
    }

    // setting everything not otherwise protected to wireframe mode
    OsgTools::State::StateSet::setLighting( _root2D.get(), false );
    OsgTools::State::PolygonMode::set( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE, _root2D->getOrCreateStateSet() );
    
    // setting everything not otherwise protected to wireframe mode
    OsgTools::State::StateSet::setLighting( _object2D.get(), false );
    OsgTools::State::PolygonMode::set( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE, _object2D->getOrCreateStateSet() );
    
     // setting everything not otherwise protected to wireframe mode
    OsgTools::State::StateSet::setLighting( _cracks2D.get(), false );
    OsgTools::State::PolygonMode::set( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE, _cracks2D->getOrCreateStateSet() );
    

    // add the 2d root (grid information)
    _root2DHi->addChild( _root2D.get() );

    // add the object information
    _root2DHi->addChild( _object2D.get() );

    // add the cracks
    _root2DHi->addChild( _cracks2D.get() );

    // add the labels
    _root2DHi->addChild( _labels2D.get() );

    return _root2DHi;
  }
  
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  //std::string message ( Usul::Strings::format( "Cursor: ( ", _mouseXCoord, ", ", _mouseYCoord, " )" )  );
  //this->_setStatusText( message, _textXPos, _textYPos, 0.20, 0.05, caller );
  //this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  set the initial grid spacing
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setInitialSpacing( Usul::Math::Vec3f spacing )
{
  Guard guard ( this );

  _initialSpacing = spacing;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cubes
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_initCubes()
{
  Guard guard( this );

  // Get and store the dimensions
  float trueDepth ( static_cast< float > ( _dimensions[1] / 100 ) );
  float depth (  trueDepth / 0.8f );
  unsigned int depthui ( static_cast< unsigned int > ( depth ) );

  float yRemainder ( trueDepth - static_cast< float > ( depthui * 0.8f ) );
  
 
  // get the sizes of the axis
  unsigned int xsize ( _dimensions[0] );
  unsigned int ysize ( depthui + 1 );
  unsigned int zsize ( _dimensions[2] );

  // size the x values
  _xValues.resize( xsize );

  // set the initial value and width
  _xValues.at( 0 ).first = 0.0f /*- ( static_cast< double > ( xsize ) / 2 )*/;
  _xValues.at( 0 ).second = _initialSpacing[0];

  // add to the map
  _originalToCurrentIndex[ "X0" ] = 0;

  // set the x values to defaults
  for( unsigned int i = 1; i < xsize; ++i )
  {
    double position ( _xValues.at( i - 1 ).first + _xValues.at( i - 1 ).second );
    _xValues.at( i ).first = position;
    _xValues.at( i ).second = _initialSpacing[0];

    // key value for map
    std::string key ( Usul::Strings::format( "X", i ) );

    // add to the map
    _originalToCurrentIndex[ key ] = i;
  }

  // add the end point
  GridPoint lxp;
  lxp.first = _xValues.at( xsize - 1 ).first + _xValues.at( xsize - 1 ).second;
  lxp.second = 0.0f;
  _xValues.push_back( lxp );
  _originalToCurrentIndex[ Usul::Strings::format( "X", xsize - 1 ) ] = xsize - 1;

  // size the y values
  _yValues.resize( ysize - 1);

  // set the initial value and width
  _yValues.at( 0 ).first = 0.0f;
  _yValues.at( 0 ).second = -_initialSpacing[1];

  // add to the map
  _originalToCurrentIndex[ "Y0" ] = 0;

  // set the x values to defaults
  for( unsigned int j = 1; j < ysize - 1; ++j )
  {
    double position ( _yValues.at( j - 1 ).first + _yValues.at( j - 1 ).second );
    _yValues.at( j ).first = position;
    _yValues.at( j ).second = -_initialSpacing[1];
    
    // key value for map
    std::string key ( Usul::Strings::format( "Y", j ) );

    // add to the map
    _originalToCurrentIndex[ key ] = j;
  }

  // add the remainder
  GridPoint yRem;
  yRem.first = _yValues.at( ysize - 2 ).first + _yValues.at( ysize - 2 ).second;
  yRem.second = -yRemainder;
  _yValues.push_back( yRem );
  _originalToCurrentIndex[ Usul::Strings::format( "Y", ysize - 2 ) ] = ysize - 2;

   // add the end point
  GridPoint lyp;
  lyp.first = _yValues.at( ysize - 1 ).first + _yValues.at( ysize - 1 ).second;
  lyp.second = 0.0f;
  _yValues.push_back( lyp );
  _originalToCurrentIndex[ Usul::Strings::format( "Y", ysize - 1 ) ] = ysize - 1;


   // size the x values
  _zValues.resize( zsize );

  // set the initial value and width
  _zValues.at( 0 ).first = 0.0f /*- ( static_cast< double > ( zsize ) / 2 )*/;
  _zValues.at( 0 ).second = _initialSpacing[2];

  // add to the map
  _originalToCurrentIndex[ "Z0" ] = 0;

  // set the x values to defaults
  for( unsigned int k = 1; k < zsize; ++k )
  {
    double position ( _zValues.at( k - 1 ).first + _zValues.at( k - 1 ).second );
    _zValues.at( k ).first = position;
    _zValues.at( k ).second = _initialSpacing[2];
    
    // key value for map
    std::string key ( Usul::Strings::format( "Z", k ) );

    // add to the map
    _originalToCurrentIndex[ key ] = k;
  }

   // add the end point
  GridPoint lzp;
  lzp.first = _zValues.at( zsize - 1 ).first + _zValues.at( zsize - 1 ).second;
  lzp.second = 0.0f;
  _zValues.push_back( lzp );
  _originalToCurrentIndex[ Usul::Strings::format( "Z", zsize - 1 ) ] = zsize - 1;

  // save the original state
  _originalXValues = _xValues;
  _originalYValues = _yValues;
  _originalZValues = _zValues;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 2D scene.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_build2DScene( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( this );

  if( false == _root2D.valid() )
  {
    _root2D = new osg::Group;
  }

  // Remove all the children
  _root2D->removeChildren( 0, _root2D->getNumChildren() );  
  
  // restore the grid values
  this->_restoreGrid();

  // add the grid padding points
  this->_addGridPadding();

  // add the additional points
  this->_addPoints();

  // get the view mode
  int viewMode ( this->getViewMode2D() );

  if( viewMode == IVPI::VIEW_MODE_2D_XY )
  {
    _root2D->addChild( this->_buildXYScene() );
  }
  
  if( viewMode == IVPI::VIEW_MODE_2D_XZ )
  {
    _root2D->addChild( this->_buildZScene() );
  }

  //if( _editGridMode2D == IVPI::OBJECT_PLACEMENT_2D || 
  //    _editGridMode2D == IVPI::OBJECT_SIZE_XY      ||
  //    _editGridMode2D == IVPI::OBJECT_SIZE_XZ        )
  //{
  //  _root2D->addChild( this->_buildObject() );
  //}

  // build the 2d objects and add them to the scene
  // Building, sources, soils, etc.
  this->_build2DObjects();

  // add any foundation cracks
  // _root2D->addChild( this->_drawCracks2D() );

  // build the grid labels
  _root2D->addChild( this->_createGridLabels2D() );

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 2D XY scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_buildXYScene()
{
  Guard guard ( this );

  GroupPtr group ( new osg::Group );

  for( unsigned int i = 0; i < _xValues.size() - 1; ++i )
  {
    for( unsigned int j = 0; j < _zValues.size() - 1; ++j )
    {
       // points of the plane
      osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

      // points
      float sx ( _xValues.at( i ).first );
      float ex ( _xValues.at( i + 1 ).first );
      float sz ( _zValues.at( j ).first );
      float ez ( _zValues.at( j + 1 ).first );

      // add the points to the list of points
      points->push_back( osg::Vec3f ( sx, 0, sz ) );
      points->push_back( osg::Vec3f ( ex, 0, sz ) );
      points->push_back( osg::Vec3f ( ex, 0, ez ) );
      points->push_back( osg::Vec3f ( sx, 0, ez ) );

      // add the plane
      group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.5f, 0.5f, 0.5f, 1.0f ) ) );

    }
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 2D Z scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_buildZScene()
{
  Guard guard ( this );

  GroupPtr group ( new osg::Group );

  for( unsigned int i = 0; i < _xValues.size() - 1; ++i )
  {
    for( unsigned int j = 0; j < _yValues.size() - 1; ++j )
    {
       // points of the plane
      osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

      // points
      float sx ( _xValues.at( i ).first );
      float ex ( _xValues.at( i + 1 ).first );
      float sz ( _yValues.at( j ).first );
      float ez ( _yValues.at( j + 1 ).first );

      // add the points to the list of points
#if 1
      points->push_back( osg::Vec3f ( sx, sz, 0 ) );
      points->push_back( osg::Vec3f ( ex, sz, 0 ) );
      points->push_back( osg::Vec3f ( ex, ez, 0 ) );
      points->push_back( osg::Vec3f ( sx, ez, 0 ) );
#else
      points->push_back( osg::Vec3f ( sx, 0, sz ) );
      points->push_back( osg::Vec3f ( ex, 0, sz ) );
      points->push_back( osg::Vec3f ( ex, 0, ez ) );
      points->push_back( osg::Vec3f ( sx, 0, ez ) );
#endif
      // add the plane
      group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.5f, 0.5f, 0.5f, 1.0f ) ) );

    }
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the original grid
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_restoreGrid()
{
  Guard guard ( this );

  // reset values to the original state
  _xValues = _originalXValues;
  _yValues = _originalYValues;
  _zValues = _originalZValues;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 3D scene.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_build3DScene ( Unknown *caller )
{
  Guard guard( this );

  if( false == _root.valid() )
  {
    _root = new osg::Group;
  }

  // Remove all the children
  _root->removeChildren( 0, _root->getNumChildren() );  

  // build the building 3D element
  if( true == _useBuilding && true == _showBuilding )
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

  // restore the original grid points
  this->_restoreGrid();

  if( true == _showCracks )
  {
    // make the cracks
    this->_makeCracks();
  }

  if( true == _showSources )
  {
    // build the chemical 3D element
    this->_makeSource3D();  
  }

  if( true == _showGrid )
  {
    // add the grid padding points
    this->_addGridPadding();

    // add the additional points
    this->_addPoints();

    // build the Grid 3D element
    this->_makeGrid();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the grid and add it to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeGrid( )
{
  // get the size of the x domain
  unsigned int xsize ( _xValues.size() - 1 );

  // Loop through the dimensions and build the cube
  for( unsigned int x = 0; x < xsize; ++x )
  {
    // get the size of the y domain at x
    unsigned int ysize ( _yValues.size() - 1 );

    for( unsigned int y = 0; y < ysize; ++y )
    {
      // get the size of the z domain at xy
      unsigned int zsize ( _zValues.size() - 1);

      for( unsigned int z = 0; z < zsize; ++z )
      {
        // Set the default ValueType
        Color c ( 0.2, 0.2, 0.2, 0.2 );
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
  normals->push_back( -n );normals->push_back( -n );normals->push_back( -n );normals->push_back( -n );
  
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

  // get the z axis grid
  GridPoints zgrid ( this->_getGridFromAxis( "Z" ) );
  // create the visuals for the x direction cracks first
	for( unsigned int i = 0; i < _cracks.first.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.first.at( i ) );

    // convert the crack values
    float value ( StrToFloat::convert( c.value ) );
    float start ( StrToFloat::convert( c.start ) );
    float end   ( StrToFloat::convert( c.end   ) );

    // set the corners
    float y( /*StrToFloat::convert( _building.z ) +*/ value );

    // set the "z" corners
    //float sx ( StrToFloat::convert( _building.x ) /*+ start*/ );
    //float ex ( StrToFloat::convert( _building.x ) + StrToFloat::convert( _building.w ) );
    float sx ( start );
    float ex ( end );

    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

    //set the points
    float sd ( StrToFloat::convert( _building.y  ) );
    float ed ( StrToFloat::convert( _building.y  ) + StrToFloat::convert( _building.h ) );

    //points->push_back( osg::Vec3f ( sx, sd, y ) );
    //points->push_back( osg::Vec3f ( ex, sd, y ) );
    //points->push_back( osg::Vec3f ( ex, ed, y ) );
    //points->push_back( osg::Vec3f ( sx, ed, y ) );

#if 0
    points->push_back( osg::Vec3f ( y, sd, sx ) );
    points->push_back( osg::Vec3f ( y, sd, ex ) );
    points->push_back( osg::Vec3f ( y, ed, ex ) );
    points->push_back( osg::Vec3f ( y, ed, sx ) );
#else
    points->push_back( osg::Vec3f ( sx, sd, y ) );
    points->push_back( osg::Vec3f ( ex, sd, y ) );
    points->push_back( osg::Vec3f ( ex, ed, y ) );
    points->push_back( osg::Vec3f ( sx, ed, y ) );
#endif

    _root->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

    //// get the nearest grid point to the crack
    //Usul::Math::Vec2ui ind ( this->_snapToGrid( y, zgrid ) );

    //unsigned int nearIndex ( ind[0] );
    //unsigned int farIndex ( ind[1] );

    //// the point where the crack is
    //float p ( y );
    //float d ( Usul::Math::minimum<float>( abs ( y - zgrid.at( nearIndex ).first ), _maxCrackGridDistance ) );

    //// p1 will be 2 grid units away from p toward the nearest grid point
    //float p1 ( p + d );

    //// P2 is 2 grid units away from p toward the farthest grid point
    //float p2 ( p - d );

    ////float d2 ( Usul::Math::minimum<float> ( zgrid.at( farIndex ).first - p2, _maxCrackGridDistance ) );  
    //
    //// half of the distance d
    //float hd ( d / 2 );

    //// p3 is halfway between p and p1
    //float p3 ( p + hd );

    //// p4 is halfway between p and p2
    //float p4 ( p - hd );

    //// add a grid point on the crack and sam distance from the crack and nearest point to the next
    //// nearest point and the crack on the Y axis
    //this->_insertGridPoint( "Z", p );
    //this->_insertGridPoint( "Z", p1 );
    //this->_insertGridPoint( "Z", p2 );
    //this->_insertGridPoint( "Z", p3 );
    //this->_insertGridPoint( "Z", p4 );

  }

  // get the x axis grid
  GridPoints xgrid ( this->_getGridFromAxis( "X" ) );

  // create the visuals for the y direction cracks first
	for( unsigned int i = 0; i < _cracks.second.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.second.at( i ) );

    // convert the crack values
    float value ( StrToFloat::convert( c.value ) );
    float start ( StrToFloat::convert( c.start ) );
    float end   ( StrToFloat::convert( c.end   ) );

    // set the corners
    float y( /*StrToFloat::convert( _building.x ) +*/ value );

    // set the "z" corners
#if 0
    float sx ( StrToFloat::convert( _building.z )  );
    float ex ( StrToFloat::convert( _building.z ) + StrToFloat::convert( _building.l ) );
#else
    float sx ( start  );
    float ex ( end );
#endif
    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

    //set the points
    float sd ( StrToFloat::convert( _building.y  ) );
    float ed ( StrToFloat::convert( _building.y  ) + StrToFloat::convert( _building.h ) );

#if 1
    points->push_back( osg::Vec3f ( y, sd, sx ) );
    points->push_back( osg::Vec3f ( y, sd, ex ) );
    points->push_back( osg::Vec3f ( y, ed, ex ) );
    points->push_back( osg::Vec3f ( y, ed, sx ) );
#else
    points->push_back( osg::Vec3f ( sx, sd, y ) );
    points->push_back( osg::Vec3f ( ex, sd, y ) );
    points->push_back( osg::Vec3f ( ex, ed, y ) );
    points->push_back( osg::Vec3f ( sx, ed, y ) );
#endif

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

  // get the lower left corner of the building
  osg::Vec3f ll  ( StrToFloat::convert( _building.x ), StrToFloat::convert( _building.y ), StrToFloat::convert( _building.z ) );

  // snap the lower left to the grid
  osg::Vec2f corner ( ll.x(), ll.z() );

  // get the length, width, and height of the building
  osg::Vec3f lhw ( StrToFloat::convert( _building.l ), StrToFloat::convert( "0.5" ), StrToFloat::convert( _building.w ) );

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

  if( true == _showFoundation )
  {
    // make the foundation
    this->_makeFoundation( ll );
  }

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
  Building newB ( _building.l, _building.w, _building.h, newX, newY, newZ, _building.depth, _building.v , _building.xrate, _building.thickness );
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

  if( true == _showFoundation )
  {
    // make the foundation
    this->_makeFoundation( ll );
  }
 
}

///////////////////////////////////////////////////////////////////////////////
//
// Add the chemical 3D objects to the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_makeSource3D()
{
  Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  for( unsigned int i = 0; i < _sources.size(); ++i )
  {
    // get the chemical
    Source s ( _sources.at( i ) );

    // color for the chemical
    Color c ( s.color[0], s.color[1], s.color[2], 0.8 );
   
    // Material for the cube
    osg::ref_ptr< osg::Material > material ( new osg::Material );
    material->setAmbient( osg::Material::FRONT_AND_BACK, c );
    material->setDiffuse( osg::Material::FRONT_AND_BACK, c );

    // get the lower left corner of the chemical
    osg::Vec3f ll  ( StrToFloat::convert( s.x ), StrToFloat::convert( s.y ), StrToFloat::convert( s.z ) );

    // snap to the grid
    //ll = this->_snapToGrid3D( ll );

    // update the source
    //_sources.at( i ).x = ll.x();
    //_sources.at( i ).y = ll.y();
    //_sources.at( i ).z = ll.z();

    // shrink
    ll = osg::Vec3f ( ll.x() - 0.00001, ll.y() - 0.00001, ll.z() - 0.00001 );

    // get the length, width, and height of the chemical
    osg::Vec3f lwh ( StrToFloat::convert( s.l ), StrToFloat::convert( s.h ), StrToFloat::convert( s.w ) );

    // create the points for the chemical
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
// Rebuild the 3D scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_rebuildScene3D()
{
  Guard guard ( this );

  // if the 3D scene is already valid then...
  if( true == _root.valid() )
  {
    // rebuild the 3D scene
    this->_build3DScene();
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Rebuild the scene
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::rebuildScene()
{
  // if the 3D scene is already valid then...
  if( true == _root.valid() )
  {
    // rebuild the 3D scene
    this->_build3DScene();
  }
  // if the 2D scene is already valid then...
  if( true == _root2D.valid() )
  {
    // rebuild the 2D scene
    this->_build2DScene();
  }

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

  // set the colors and set binding
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

  // build the cubes based on the current dimensions
  this->_initCubes();

  // initialize the grid materials
  this->_initializeGridMaterials();

  // set up the input variables
  _configFileName = Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "vpi_config.vpi";
  this->_readInitializationFile( _configFileName );

  // read the chemicals file
  std::string contamintantFilename ( Usul::CommandLine::Arguments::instance().directory() + "/../configs/" + "chemicals.vpi" );
  this->_readChemicals( contamintantFilename );

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

        // create a temp chemical
        Chemicals c;
        Source s ( l, w, h, xpos, ypos, zpos, name, c );

        // add to the list of chemicals
        _sources.push_back( s );

        // increment the number of chemicals read
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

        // create a temp chemical
        Soil s ( name, elevation, porosity, H2OPorosity, carbon, permability, viscosity );

        // add to the list of chemicals
        _soils.push_back( s );

        // increment the number of chemicals read
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

 //       // create a temp chemical
 //       Crack c ( sx, sy, ex, ey, w );

 //       // add to the list of chemicals
 //       _cracks.push_back( c );

 //       // increment the number of chemicals read
 //       ++lineNumber;

 //     }// end for for activators read   

 //   }// end if for valid entry found

 // }// end while read for file parsing

 // ifs.close();

}



///////////////////////////////////////////////////////////////////////////////
//
// Read the chemicals file
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_readChemicals( const std::string& filename )
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
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No Presets loaded for Chemicals" ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Reading Chemicals file: " << filename << std::endl;

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

        // create a temp chemical
        Chemical c ( lineNumber, name, henry, koc, diffair, diffh2o, atmoconc );

        // add to the list of chemicals
        _chemicals.push_back( c );

        // increment the number of chemicals read
        ++lineNumber;

      }// end for for activators read   

    }// end if for valid entry found

  }// end while read for file parsing

  // seed the library
  _chemicalLibrary = _chemicals;

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
  //Guard guard( this );
  //
  ////re-build the scene
  //this->_buildScene( 0x0 );

  //// create a dragger temp
  //osg::ref_ptr< osgManipulator::TabBoxDragger > dragger ( new osgManipulator::TabBoxDragger );
  //dragger->setupDefaultGeometry();

  ////add the dragger to the scene
  //_root->addChild( dragger.get() );

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
// Get the chemical library
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Chemicals VaporIntrusionGUIDocument::library()
{
  Guard guard ( this );

  return _chemicalLibrary;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the chemical library
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::library( Chemicals l )
{
  Guard guard ( this );
  _chemicalLibrary = l;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the chemicals
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::chemicals( Chemicals c )
{
  Guard guard ( this );
  _chemicals = c;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the chemicals
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::Chemicals VaporIntrusionGUIDocument::chemicals()
{
  Guard guard ( this );
  return _chemicals;
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
// Insert a grid point into the given axis
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_insertGridPoint( const std::string& axis, float pos )
{
  Guard guard ( this ); 

  // get the grid points
  GridPoints axisGrid ( this->_getGridFromAxis( axis ) );

  // check for the existance of the point already
  if( true == this->_gridHasPoint( axis, pos ) )
  {
    return;
  }

  // the new grid for this axis
  GridPoints grid;

  // make sure there are points
  if( axisGrid.size() == 0 )
  {
    std::cout << "No points found for axis: " << axis << std::endl;
    return;
  }

  // get the indices of the point before and after value
  Usul::Math::Vec2ui indices ( this->_snapToGrid( pos, axisGrid ) );

  // get the lesser index
  

  if( indices[0] > indices[1] )
  {
    unsigned int index ( indices[0] );
    indices[0] = indices[1];
    indices[1] = index;
  }

  for( unsigned int i = 0; i < indices[0]; ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  // direction in the positive or negative
  float pnd ( 1 );

  // get the position and distance of the first index
  float p1 ( axisGrid.at( indices[0] ).first );

  // if the point before has a position that is greater than the positon to add
  if( p1 > pos )
  {
    pnd = -1;
  }

  // distance to the point to insert
  float d1 ( sqrt ( ( pos - p1 ) * ( pos - p1 ) ) );

  // +- correction
  d1 *= pnd;

  // get the position and distance of the second index
  float p2 ( axisGrid.at( indices[1] ).first );
  float d2 ( sqrt ( ( p2 - pos ) * ( p2 - pos ) ) );
  d2 *= pnd;

  // update the distance on the point before the new entry
  grid.push_back( GridPoint( static_cast< double > ( p1 ), static_cast< double > ( d1 ) ) );

  // add the new point
  grid.push_back( GridPoint( static_cast< double > ( pos ), static_cast< double > ( static_cast< double > ( d2 ) ) ) );
 
  for( unsigned int i = indices[1]; i < axisGrid.size(); ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  this->_setGridFromAxis( axis, grid );

  this->_fixDimensions();
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  reset the dimensions
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_fixDimensions()
{
  _dimensions = Usul::Math::Vec3ui ( _xValues.size(), _yValues.size(), _zValues.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
// Check the axis for the point at <pos>
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::_gridHasPoint( const std::string& axis, float pos )
{
  Guard guard ( this );

   // get the grid points
  GridPoints axisGrid ( this->_getGridFromAxis( axis ) );

  // make sure there are points
  if( axisGrid.size() == 0 )
  {
    std::cout << "No points found for axis: " << axis << std::endl;
    return false;
  }

  unsigned int index ( 0 );

  while( index < axisGrid.size() )
  {
    GridPoint p ( axisGrid.at( index ) );

    if( p.first == pos )
    {
      return true;
    }
    ++ index;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Insert a grid point into the given axis
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_removeGridPoint( const std::string& axis, float pos )
{
  Guard guard ( this );

  // the new grid for this axis
  GridAxisPoints grid;

  // make sure there are points
  if( _axisPoints.size() == 0 )
  {
    std::cout << "No points found for axis: " << axis << std::endl;
    return;
  }

  // current index
  unsigned int index ( 0 );

  while( index < _axisPoints.size() )
  {
    // get the next point
    GridAxisPoint p ( _axisPoints.at( index ) );

    // convert to a float
    float ppos ( StrToFloat::convert( p.value ) );

    // found a match
    if( p.axis == axis && ppos == pos )
    {

    }
    // match not found
    else
    {
      // add to the temp grid list
      grid.push_back( _axisPoints.at( index ) );
    }

    // increment the index
    ++index;
  }

  // set the axis grid
  _axisPoints = grid;


}


///////////////////////////////////////////////////////////////////////////////
//
// Insert a new grid space at between <startPoint> and <endPoint> at 
// <startPoint> + <value> for axis <axis>
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_insertSingleGridSpace( const std::string& axis, unsigned int startPoint, unsigned int endPoint, float value )
{
  Guard guard ( this );

  GridPoints axisGrid ( this->_getGridFromAxis( axis ) );
  GridPoints grid;

  std::string key( Usul::Strings::format( axis, startPoint ) );
  unsigned int index ( _originalToCurrentIndex[ key ] );

  for( unsigned int i = 0; i < index; ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  float position ( axisGrid.at( index ).first );
  float distance ( axisGrid.at( index ).second );

  // update the distance on the point before the new entry
  grid.push_back( GridPoint( static_cast< double > ( position ), static_cast< double > ( value ) ) );

  // add the new point
  grid.push_back( GridPoint( static_cast< double > ( position + value ), static_cast< double > ( distance - value ) ) );
 
  for( unsigned int i = index + 1; i < axisGrid.size(); ++i )
  {
    grid.push_back( axisGrid.at( i ) );
  }

  this->_setGridFromAxis( axis, grid );

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
// Return the index of the closest grid point to <value>
//
///////////////////////////////////////////////////////////////////////////////

unsigned int VaporIntrusionGUIDocument::_closestGridPoint( float value, GridPoints grid )
{
   Guard guard ( this );

  // get the starting value for the x values
  float minDistance ( abs ( grid.at( 0 ).first - value ) );
  unsigned int currIndex ( 0 );

  // check the rest of the x values to find the closest point to the corner
  for( unsigned int i = 1; i < grid.size(); ++i )
  {
    float temp ( abs ( grid.at( i ).first - value ) ); 

    if( temp < minDistance )
    {
      minDistance = temp;
      currIndex = i;
    }

  }

  return currIndex;
}

///////////////////////////////////////////////////////////////////////////////
//
// Find indices that the grid point lies between
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2ui VaporIntrusionGUIDocument::_snapToGrid( float value, GridPoints grid )
{
  Guard guard ( this );

  // the indices
  Usul::Math::Vec2ui indices ( 0, 0 );

  // get the starting value for the x values
  float minDistance ( abs ( grid.at( 0 ).first - value ) );
  unsigned int currIndex ( 0 );

  // check the rest of the x values to find the closest point to the corner
  for( unsigned int i = 1; i < grid.size(); ++i )
  {
    float temp ( abs ( grid.at( i ).first - value ) ); 

    if( temp < minDistance )
    {
      minDistance = temp;
      currIndex = i;
    }

  }

  indices[0] = currIndex;
  indices[1] = currIndex;

  float v1 ( grid.at( currIndex ).first );

  if( currIndex < grid.size() - 1 )
  {
    float v2 ( grid.at( currIndex + 1 ).first );

    if( v1 > v2 )
    {
      float t ( v2 );
      v2 = v1;
      v1 = t;
    }

    if( value >= v1 && value <= v2 )
    {
      indices[1] = currIndex + 1;
    }
    else
    {
      if( currIndex > 0 )
      {
        indices[1] = currIndex - 1;
      }
    }
  }
  else
  {
    indices[0] = currIndex - 1;
    indices[1] = currIndex;
  }
    
  // returned indices are guaranteed to be [0] < [1]
  return indices;  
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


/////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu
//
/////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller)
{
  typedef MenuKit::ToggleButton ToggleButton;
  
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;

  // Add to the view menu.
  MenuKit::Menu::RefPtr view ( menu.find ( "&View", true ) );

  view->append ( ToggleButton::create ( "Building", boost::bind ( &VaporIntrusionGUIDocument::showBuilding, this, _1 ), boost::bind ( &VaporIntrusionGUIDocument::isShowBuilding, this ) ) );
  view->append ( ToggleButton::create ( "Grid", boost::bind ( &VaporIntrusionGUIDocument::showGrid, this, _1 ), boost::bind ( &VaporIntrusionGUIDocument::isShowGrid, this ) ) );
  view->append ( ToggleButton::create ( "Cracks", boost::bind ( &VaporIntrusionGUIDocument::showCracks, this, _1 ), boost::bind ( &VaporIntrusionGUIDocument::isShowCracks, this ) ) );
  view->append ( ToggleButton::create ( "Foundation", boost::bind ( &VaporIntrusionGUIDocument::showFoundation, this, _1 ), boost::bind ( &VaporIntrusionGUIDocument::isShowFoundation, this ) ) );
  view->append ( ToggleButton::create ( "Sources", boost::bind ( &VaporIntrusionGUIDocument::showSources, this, _1 ), boost::bind ( &VaporIntrusionGUIDocument::isShowSources, this ) ) );

  // Add to the objects menu.
  MenuKit::Menu::RefPtr objectMenu ( menu.find ( "&Objects", true ) );
  objectMenu->append ( MenuKit::Button::create ( "Building (B)", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::objectMenuAddBuilding ) ) );
  objectMenu->append ( MenuKit::Button::create ( "Source (S)", Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::objectMenuAddSource ) ) );

  //----------------------------------------------------------------------------------------------

  // Create the tools menu.
  MenuKit::Menu::RefPtr toolsMenu ( menu.find ( "&Tools", true ) );

  // Add the view sub menu
  MenuKit::Menu::RefPtr viewMenu  ( toolsMenu->find ( "&Views", true ) );
  viewMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isViewMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsViewMode2D ),
                                                                    "XY Plane (,)", IVPI::VIEW_MODE_2D_XY ) ) );
  viewMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isViewMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsViewMode2D ),
                                                                    "XZ Plane (.)", IVPI::VIEW_MODE_2D_XZ ) ) );


  // Add the build sub men
  MenuKit::Menu::RefPtr buildMenu ( toolsMenu->find ( "&Builds", true ) );
  buildMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isBuildMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsBuildMode2D ),
                                                                    "Grid (G)", IVPI::BUILD_MODE_GRID_EDIT ) ) );
  buildMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isBuildMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsBuildMode2D ),
                                                                    "Crack (C)", IVPI::BUILD_MODE_CRACK_PLACE1 ) ) );

  // Add the edit sub menu
  MenuKit::Menu::RefPtr editMenu  ( toolsMenu->find ( "&Edits", true ) );
  editMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isEditMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsEditMode2D ),
                                                                    "Local X Axis (X)", IVPI::EDIT_X_GRID_2D ) ) );
  editMenu->append ( new ToggleButton ( genericIndexToggle( caller, Usul::Adaptors::memberFunction<bool> ( this, &VaporIntrusionGUIDocument::isEditMode ), 
                                                                    Usul::Adaptors::memberFunction<void> ( this, &VaporIntrusionGUIDocument::setIsEditMode2D ),
                                                                    "Local Y Axis (Y)", IVPI::EDIT_Y_GRID_2D ) ) );

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the edit mode state
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setIsEditMode2D( int mode, bool value )
{
  Guard guard ( this );

  if( true == value )
  {
    this->setEditMode2D( mode );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the edit mode state
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isEditMode( int mode )
{
  Guard guard ( this );

  // get the build mode
  int editMode ( this->getEditMode2D() );

  if ( editMode == mode )
  {
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the build mode state
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setIsBuildMode2D( int mode, bool value )
{
  Guard guard ( this );

  if( true == value )
  {
    this->setBuildMode2D( mode );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the build mode state
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isBuildMode( int mode )
{
  Guard guard ( this );

  // get the build mode
  int buildMode ( this->getBuildMode2D() );

  if ( buildMode == mode )
  {
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view mode state
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setIsViewMode2D( int mode, bool value )
{
  Guard guard ( this );

  if( true == value )
  {
    this->setViewMode2D( mode );

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view mode state
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isViewMode( int mode )
{
  Guard guard ( this );
  
  // get the view mode
  int viewMode ( this->getViewMode2D() );

  if ( viewMode == mode )
  {
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the building
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::showBuilding ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _showBuilding )
  {
    _showBuilding = b;

    this->rebuildScene();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the show building flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isShowBuilding() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _showBuilding;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Draw the grid
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::showGrid ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _showGrid )
  {
    _showGrid = b;

    this->rebuildScene();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the show grid flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isShowGrid() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _showGrid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the cracks
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::showCracks ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _showCracks )
  {
    _showCracks = b;

    this->rebuildScene();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the show cracks flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isShowCracks() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _showCracks;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the foundation
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::showFoundation ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _showFoundation )
  {
    _showFoundation = b;

    this->rebuildScene();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the show foundation flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isShowFoundation() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _showFoundation;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the sources
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::showSources ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( b != _showSources )
  {
    _showSources = b;

    this->rebuildScene();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the show Sources flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::isShowSources() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _showSources;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the additional grid axis points
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIDocument::GridAxisPoints VaporIntrusionGUIDocument::gridAxisPoints()
{
  Guard guard ( this );

  return _axisPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the additional grid axis points
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::gridAxisPoints( GridAxisPoints ap )
{
  Guard guard ( this );

  _axisPoints = ap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add 40cm and 80cm padding to grid edges
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_addGridPadding()
{
  // get the first and last x point
  float fxp ( _xValues.at( 0 ).first );
  float lxp ( _xValues.at( _xValues.size() - 1 ).first );

  // get the first and last z point
  float fzp ( _zValues.at( 0 ).first );
  float lzp ( _zValues.at( _zValues.size() - 1 ).first );

  // insert padding at the beginning of the x grid
  this->_insertGridPoint( "X", fxp + 0.4 );
  this->_insertGridPoint( "X", fxp + 0.8 );

  // insert padding at the end of the x grid
  this->_insertGridPoint( "X", lxp - 0.4 );
  this->_insertGridPoint( "X", lxp - 0.8 );

  // insert padding at the beginning of the x grid
  this->_insertGridPoint( "Z", fzp + 0.4 );
  this->_insertGridPoint( "Z", fzp + 0.8 );

  // insert padding at the end of the x grid
  this->_insertGridPoint( "Z", lzp - 0.4 );
  this->_insertGridPoint( "Z", lzp - 0.8 );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add additional points to the grid
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_addPoints()
{
  Guard guard ( this );

  for( unsigned int i = 0; i < _axisPoints.size(); ++i )
  {
    // get the point
    GridAxisPoint p ( _axisPoints.at( i ) );

    // convert the value to a float
    float value ( Usul::Convert::Type< std::string, float >::convert( p.value ) );

    // check for existance of the point first
    if( false == this->_gridHasPoint( p.axis, value ) )
    {
      // add the point
      this->_insertGridPoint( p.axis, value );
    }
      
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle adding cracks
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_handleCrackAdd( Usul::Math::Vec3f point )
{
  Guard guard ( this );
  
  // get the closest grid point
  unsigned int index;
  
  // working grid
  GridPoints paraGrid;
  GridPoints perpGrid;

  // working axis
  std::string axis;

  // get the edit grid mode
  int editMode ( this->getEditMode2D() );

  // get the build mode
  int buildMode ( this->getBuildMode2D() );

  // X axis selected
  if( editMode == IVPI::EDIT_Y_GRID_2D )
  {
    // get the parallel and perpendicular grids
    paraGrid = this->_getGridFromAxis( "X" );
    perpGrid = this->_getGridFromAxis( "Z" );

    if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE1 )
    {
      index = this->_closestGridPoint( point[0], paraGrid );
      _placementCrack.value = Usul::Strings::format( paraGrid.at( index ).first );

      index = this->_closestGridPoint( point[2], perpGrid );
      _placementCrack.start = Usul::Strings::format( perpGrid.at( index ).first );
    }
    if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE2 )
    {
      // get the grid and index
      index = this->_closestGridPoint( point[2], perpGrid );

      // set the crack end point
      _placementCrack.end = Usul::Strings::format( perpGrid.at( index ).first );

      // Building side walls 
      float paraW1 ( StrToFloat::convert( _building.x ) );
      float paraW2 ( paraW1 + StrToFloat::convert( _building.w ) );
      float perpW1 ( StrToFloat::convert( _building.z ) );
      float perpW2 ( perpW1 + StrToFloat::convert( _building.l ) );

      // indices of the sidewalls
      unsigned int paraWI1( this->_closestGridPoint( paraW1, paraGrid ) );
      unsigned int paraWI2( this->_closestGridPoint( paraW2, paraGrid ) );
      unsigned int perpWI1( this->_closestGridPoint( perpW1, perpGrid ) );
      unsigned int perpWI2( this->_closestGridPoint( perpW2, perpGrid ) );

      // add the crack, rebuild the cracks visual, rebuild the scene
      this->_addCrack( "X", "Z", _placementCrack, paraWI1, paraWI2, perpWI1, perpWI2 );
      this->_rebuildCracks();
      this->_rebuildScene3D();

      // clear out the placement crack
      _placementCrack = Crack();
    }    
  }


  // "Y" axis selected
  if ( editMode == IVPI::EDIT_X_GRID_2D )
  {
    // get the parallel and perpendicular grids
    paraGrid = this->_getGridFromAxis( "Z" );
    perpGrid = this->_getGridFromAxis( "X" );

    if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE1 )
    {
      //grid = this->_getGridFromAxis( "Z" );
      index = this->_closestGridPoint( point[2], paraGrid );
      _placementCrack.value = Usul::Strings::format( paraGrid.at( index ).first );

      //grid = this->_getGridFromAxis( "X" );
      index = this->_closestGridPoint( point[0], perpGrid );
      _placementCrack.start = Usul::Strings::format( perpGrid.at( index ).first );
    }
    if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE2 )
    {
      // get the grid and index
      //grid = this->_getGridFromAxis( "X" );
      index = this->_closestGridPoint( point[0], perpGrid );

      // set the crack end point
      _placementCrack.end = Usul::Strings::format( perpGrid.at( index ).first );

      // get the z grid
      //grid = this->_getGridFromAxis( "Z" );

      //// Building side walls 
      //float bsw1 ( StrToFloat::convert( _building.x ) );
      //float bsw2 ( bsw1 + StrToFloat::convert( _building.w ) );

      //// indices of the sidewalls
      //unsigned int bswi1( this->_closestGridPoint( bsw1, grid ) );
      //unsigned int bswi2( this->_closestGridPoint( bsw2, grid ) );

      //// add the crack, rebuild the cracks visual, rebuild the scene
      //this->_addCrack( "Z", _placementCrack, bswi1, bswi2 );

      // Building side walls 
      float paraW1 ( StrToFloat::convert( _building.z ) );
      float paraW2 ( paraW1 + StrToFloat::convert( _building.l ) );
      float perpW1 ( StrToFloat::convert( _building.x ) );
      float perpW2 ( perpW1 + StrToFloat::convert( _building.w ) );

      // indices of the sidewalls
      unsigned int paraWI1( this->_closestGridPoint( paraW1, paraGrid ) );
      unsigned int paraWI2( this->_closestGridPoint( paraW2, paraGrid ) );
      unsigned int perpWI1( this->_closestGridPoint( perpW1, perpGrid ) );
      unsigned int perpWI2( this->_closestGridPoint( perpW2, perpGrid ) );

      // add the crack, rebuild the cracks visual, rebuild the scene
      this->_addCrack( "Z", "X", _placementCrack, paraWI1, paraWI2, perpWI1, perpWI2 );
      this->_rebuildCracks();
      this->_rebuildScene3D();

      // clear out the placement crack
      _placementCrack = Crack();
    }
  }
  
  // change the crack build mode
  if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE1 )
  {
    this->setBuildMode2D( IVPI::BUILD_MODE_CRACK_PLACE2 );
  }
  else
  {
    if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE2 )
    {
      this->setBuildMode2D( IVPI::BUILD_MODE_CRACK_PLACE1 );
    }
  }


  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle removing cracks
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_handleCrackRemove( Usul::Math::Vec3f point )
{
  Guard guard ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle left mouse clicks from the viewer
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::handleLeftMouseClick( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  // get the build mode
  int buildMode ( this->getBuildMode2D() );

  // grid line editing
  if( buildMode == IVPI::BUILD_MODE_GRID_EDIT )
  {
    this->_addGridPointFromViewer( point );
    this->rebuildScene();
  }

  // foundation crack editing
  if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE1 || 
      buildMode == IVPI::BUILD_MODE_CRACK_PLACE2    )
  {
    this->_handleCrackAdd( point );
  }
  
  // object placement
  if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY )
  {
    // set the building location
    this->_setBuildingLocationFromClick( point );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle right mouse clicks from the viewer
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::handleRightMouseClick( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  // get the build mode
  int buildMode ( this->getBuildMode2D() );

  // grid line editing
  if( buildMode == IVPI::BUILD_MODE_GRID_EDIT )
  {
    this->_removeGridPointFromViewer( point );
    this->rebuildScene();
  }

  // foundation crack editing
  if( buildMode == IVPI::BUILD_MODE_CRACK_PLACE1 )
  {
    this->_removeCrack( point );
    this->_rebuildCracks();
    this->_rebuildScene3D();
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a crack
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_addCrack( const std::string& axis1, const std::string& axis2, Crack crack, 
                                           unsigned int parallelWall1, unsigned int parallelWall2, 
                                           unsigned int perpendicularWall1, unsigned int perpendicularWall2 )
{
  Guard guard ( this );

  // make sure there is a building in the experiment
  if( false == this->useBuilding() )
  {
    std::cout << "There must be a building in the experiment to perform this operation!" << std::cout;
    return;
  }

  // working perpendicular grid
  GridPoints perpGrid ( this->_getGridFromAxis( axis2 ) );

  // get the closest grid point for the crack to lie on 
  // in the perpendicular direction for the...
  
  // ...start point
  unsigned int startIndex ( this->_closestGridPoint( StrToFloat::convert( crack.start ), perpGrid ) );

  // ...end point
  unsigned int endIndex ( this->_closestGridPoint( StrToFloat::convert( crack.end ), perpGrid ) );
  
  // make sure there is 2 grid points between the start and end points and the sidewalls of the building
  // on either side of the crack  
  if( !( startIndex > perpendicularWall1 + 2 ) || !( startIndex < perpendicularWall2 - 2 ) ||
      !( endIndex > perpendicularWall1 + 2 ) || !( endIndex < perpendicularWall2 - 2 ))
  {
    std::cout << "Invalid location for a crack.  There must be at least 2 grid points between the start/end point and the building sidewalls." << std::endl;
    return;
  }
  
  // working parallel grid
  GridPoints paraGrid ( this->_getGridFromAxis( axis1 ) );

  // get the closest grid point for the crack to lie on in the parallel direction
  unsigned int index ( this->_closestGridPoint( StrToFloat::convert( crack.value ), paraGrid ) );
  
  // make sure there is 2 grid points between the crack and the sidewalls of the building
  // on either side of the crack  
  if( !( index > parallelWall1 + 2 ) || !( index < parallelWall2 - 2 ) )
  {
    std::cout << "Invalid location for a crack.  There must be at least 2 grid points between the crack and the building sidewalls." << std::endl;
    return;
  }

  if( "Z" == axis1 )
  {
    // add the cracks to the list of cracks for the X axis
    if( false == this->_crackExists( _cracks.first, crack ) )
    {
      _cracks.first.push_back( crack );
    }
  }
  if( "X" == axis1 )
  {
    // add the cracks to the list of cracks for the Z axis
    if( false == this->_crackExists( _cracks.second, crack ) )
    {
      _cracks.second.push_back( crack );
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update and rebuild cracks for 2D
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_rebuildCracks()
{
  Guard guard ( this );

  if( false == _cracks2D.valid() )
  {
    _cracks2D = new osg::Group();
  }

  //remove all children
  _cracks2D->removeChildren( 0, _cracks2D->getNumChildren() );

  // rebuild the cracks
  _cracks2D->addChild( this->_drawCracks2D() );

  // redraw the scene
  this->requestRedraw();
    
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if a grid point lies within the building x and z bounds.
//  This will make sure that foundation crack rules are followed when
//  removing a grid point.
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::_checkBuildingInterior( const std::string& axis, float point )
{
  Guard guard ( this );

  // we don't care about the y axis since cracks are only placed in x and z
  if( "Y" == axis )
  {
    return false;
  }

  // get the building
  Building b ( this->building() );

  // get the dimensions in the x and z axises
  float sx ( StrToFloat::convert( b.x ) );
  float ex ( sx + StrToFloat::convert( b.l ) );
  float sz ( StrToFloat::convert( b.z ) );
  float ez ( sx + StrToFloat::convert( b.w ) );

  if( "Z" == axis && _cracks.first.size() > 0 )
  {
    if( point <= ez && point >= sz )
    {
      std::cout << "Error: cannot remove a grid point from the building interior while cracks exist in this axis." << std::endl;
      return true;
    }
  }

  if( "X" == axis && _cracks.second.size() > 0 )
  {
    if( point <= ex && point >= sx )
    {
      std::cout << "Error: cannot remove a grid point from the building interior while cracks exist in this axis." << std::endl;
      return true;
    }
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  check to see if a building edge lies on the <point> in the <axis>
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::_checkBuildingEdges( const std::string& axis, float point )
{
  Guard guard ( this );

  Building b (  this->building() );

  if( "X" == axis )
  {
    float p1 ( StrToFloat::convert( b.x ) );
    float p2 ( p1 + StrToFloat::convert( b.l ) );
    
    if( point > p1 - 0.000001 && point < p1 + 0.000001 )
      return true;

    if( point > p2 - 0.000001 && point < p2 + 0.000001 )
      return true;
    
  }

  if( "Y" == axis )
  {
    float p1 ( StrToFloat::convert( b.y ) );
    float p2 ( p1 + StrToFloat::convert( b.h ) );
    
    if( point > p1 - 0.000001 && point < p1 + 0.000001 )
      return true;

    if( point > p2 - 0.000001 && point < p2 + 0.000001 )
      return true;
  }

  if( "Z" == axis )
  {

    float p1 ( StrToFloat::convert( b.z ) );
    float p2 ( p1 + StrToFloat::convert( b.w ) );
    
    if( point > p1 - 0.000001 && point < p1 + 0.000001 )
      return true;

    if( point > p2 - 0.000001 && point < p2 + 0.000001 )
      return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Verify whether or not an object occupies the selected grid point
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::_checkGridForObject( const std::string& axis, float point )
{
  Guard guard ( this );

  bool useBuilding ( this->useBuilding() );

  // check the building
  if( true == useBuilding && true == this->_checkBuildingEdges( axis, point ) )
  {
    std::cout << "Error: cannot remove a grid point from the building edge." << std::endl;
    return true;
  }

  // check the building cracks
  if( true == useBuilding && true == this->_checkBuildingInterior( axis, point ) )
  {
    return true;
  }



  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check if <crack> exists in <cracks>
//
///////////////////////////////////////////////////////////////////////////////

bool VaporIntrusionGUIDocument::_crackExists( Cracks cracks, Crack crack )
{
  Guard guard ( this );

  // Value of the input crack
  float v ( StrToFloat::convert ( crack.value ) );

  for( unsigned int i = 0; i < cracks.size(); ++i )
  {
    Crack c ( cracks.at( i ) );

    // Value of the crack at position i
    float vi ( StrToFloat::convert( c.value ) );

    // check to see if the two values are close to being equal
    if( v <= vi + 0.0000001f && v >= vi - 0.0000001f )
    {
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove a crack
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_removeCrack( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  // make sure there is a building in the experiment
  if( false == this->useBuilding() )
  {
    std::cout << "There must be a building in the experiment to perform this operation!" << std::cout;
    return;
  }

  // The closest grid point
  float cPoint ( 0.0f );
  
  // working axis
  std::string axis;

  // Selected Cracks
  Cracks cracks;

  // get the edit mode
  int editMode ( this->getEditMode2D() );

  // X axis selected
  if( editMode == IVPI::EDIT_Y_GRID_2D )
  {
    // set the axis
    axis = "X";

    // get the X axis cracks
    cracks = _cracks.second;  

    // get the X grid
    GridPoints grid ( this->_getGridFromAxis( axis ) );

    // get the X component point from the input value
    cPoint = grid.at ( this->_closestGridPoint( point[0], grid ) ).first;
  }

  // "Y" axis selected
  if ( editMode == IVPI::EDIT_X_GRID_2D )
  {
    // set the axis
    axis = "Z";

    // get the Z axis cracks
    cracks = _cracks.first;

    // get the Z grid
    GridPoints grid ( this->_getGridFromAxis( axis ) );

    // get the Z component point from the input value
    cPoint = grid.at ( this->_closestGridPoint( point[2], grid ) ).first;
  }

  // new cracks
  Cracks newCracks;

  for( unsigned int i = 0; i < cracks.size(); ++i )
  {
    // get the current crack
    Crack crack ( cracks.at( i ) );

    // get the float value of the crack
    float v ( StrToFloat::convert( crack.value ) );

    // if the crack is a match do not add it to the list of new cracks
    if( !( v >= cPoint - 0.00001) || !( v <= cPoint + 0.00001 ) )
    {
      newCracks.push_back( crack );
    }

  }

  if( "X" == axis )
  {
    _cracks.second = newCracks;
  }

  if( "Z" == axis )
  {
    _cracks.first = newCracks;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Called from the viewer to add a grid point where the user clicked in the 
//  window.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_addGridPointFromViewer( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  GridPoints grid;

  float value ( 0.0f );

  std::string axis;

  // get teh edit mode
  int editMode ( this->getEditMode2D() );

  // get the view mode
  int viewMode ( this->getViewMode2D() );

  if( editMode == IVPI::EDIT_X_GRID_2D )
  {
    grid = this->_getGridFromAxis( "X" );
    value = point[0];
    axis = "X";
  }

  else if( editMode == IVPI::EDIT_Y_GRID_2D && viewMode == IVPI::VIEW_MODE_2D_XY )
  {
    grid = this->_getGridFromAxis( "Z" );
    value = point[2];
    axis = "Z";
  }
  else if( editMode == IVPI::EDIT_Y_GRID_2D && viewMode == IVPI::VIEW_MODE_2D_XZ )
  {
    grid = this->_getGridFromAxis( "Y" );
    value = point[1];
    axis = "Y";
  }
  else
  {
    return;
  }

  // get the nearest grid point to the crack
  Usul::Math::Vec2ui ind ( this->_snapToGrid( value, grid ) );

  // get the near and far index
  unsigned int nearIndex ( ind[0] );
  unsigned int farIndex ( ind[1] );  

  // get the distance
  float d ( abs ( grid.at( nearIndex ).first - grid.at( farIndex ).first ) );

  // check to see that the distance is > than the minimum distance * 2 
  if( d < ( _minimumGridDistance * 2.0f ) - 0.001 )
  {
    std::cout << "Unable to subdivide! The resulting grid distance would be smaller than the minimum allowed distance" << std::endl;
    return;
  }

  // get the half way point
  float p ( ( grid.at( nearIndex ).first + grid.at( farIndex ).first ) / 2.0f );

  // insert the new grid point
  GridAxisPoint gap ( axis, Usul::Convert::Type< float, std::string >::convert( p ) );
  _axisPoints.push_back( gap );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Called from the viewer to add a grid point where the user clicked in the 
//  window.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_removeGridPointFromViewer( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  GridPoints grid;

  float value ( 0.0f );

  std::string axis;

  // get the edit mode
  int editMode ( this->getEditMode2D() );

  // get the view mode
  int viewMode ( this->getViewMode2D() );

  if( editMode == IVPI::EDIT_X_GRID_2D )
  {
    grid = this->_getGridFromAxis( "X" );
    value = point[0];
    axis = "X";
  }

  else if( editMode == IVPI::EDIT_Y_GRID_2D && viewMode == IVPI::VIEW_MODE_2D_XY )
  {
    grid = this->_getGridFromAxis( "Z" );
    value = point[2];
    axis = "Z";
  }
  else if( editMode == IVPI::EDIT_Y_GRID_2D && viewMode == IVPI::VIEW_MODE_2D_XZ )
  {
    grid = this->_getGridFromAxis( "Y" );
    value = point[1];
    axis = "Y";
  }
  else
  {
    return;
  }

  // get the value of the nearest grid point
  unsigned int index ( this->_closestGridPoint( value, grid ) );
  float gridPointValue ( grid.at( index ).first );

  // make sure there aren't any objects on the point 
  if( true == this->_checkGridForObject( axis, gridPointValue ) )
  {
    return;
  }

  // get the nearest grid point to the crack
  Usul::Math::Vec2ui ind ( this->_snapToGrid( value, grid ) );

  // get the near and far index
  unsigned int nearIndex ( ind[0] );
  unsigned int farIndex ( ind[1] );  

  // get the grid points closest to the input point and
  // calculate the distances to both
  float p1 ( grid.at( nearIndex ).first );
  float p2 ( grid.at( farIndex ).first );
  float d1 ( abs ( value - p1 ) );
  float d2 ( abs ( value - p2 ) );

  if( d1 < d2 )
  {
    this->_removeGridPoint( axis, p1 );
  }
  else
  {
    this->_removeGridPoint( axis, p2 );
  }


}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 2D grid edit mode
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setEditMode2D( int mode )
{
  Guard guard ( this );
  _editGridMode2D = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 2D grid build Mode
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setBuildMode2D( int mode )
{
   Guard guard ( this );

   _buildMode2D = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 2D grid edit Mode
//
///////////////////////////////////////////////////////////////////////////////

int VaporIntrusionGUIDocument::getEditMode2D()
{
  Guard guard ( this );

  return _editGridMode2D;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 2D grid build Mode
//
///////////////////////////////////////////////////////////////////////////////

int VaporIntrusionGUIDocument::getBuildMode2D()
{
  Guard guard ( this );

  return _buildMode2D;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 2D grid view Mode
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setViewMode2D( int mode )
{
  Guard guard ( this );
  _viewMode2D = mode;
  
  // rebuild the scene
  this->rebuildScene();

  // set the view
  this->_setCameraFromViewMode( mode );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 2D grid view Mode
//
///////////////////////////////////////////////////////////////////////////////

int VaporIntrusionGUIDocument::getViewMode2D()
{
  Guard guard ( this );
  return _viewMode2D;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 2D grid object Mode
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setObjectMode( int mode )
{
  Guard guard ( this );

  // set the object mode
  _objectMode = mode;

  // clear out the current object
  Object2D object ( 0, 0, 0, 1, 0, 1 );
  _currentObject = object;

  // rebuild the scene
  this->_rebuildObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 2D grid object Mode
//
///////////////////////////////////////////////////////////////////////////////

int VaporIntrusionGUIDocument::getObjectMode()
{
  Guard guard ( this );

  return _objectMode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the object
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::clearObject()
{
  Guard guard ( this );

  // Remove all the children
  _object2D->removeChildren( 0, _root2D->getNumChildren() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the building location
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_setBuildingLocationFromClick( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  // get the X and Z grids
  GridPoints xgrid ( this->_getGridFromAxis( "X" ) );
  GridPoints zgrid ( this->_getGridFromAxis( "Z" ) );

  // get the closest X and Z grid indices
  unsigned int xi ( this->_closestGridPoint( point[0], xgrid ) );
  unsigned int zi ( this->_closestGridPoint( point[2], zgrid ) );

  // get the grid sizes
  int xsize ( static_cast< int > ( _xValues.size() ) );
  int zsize ( static_cast< int > ( _zValues.size() ) );

  // place the grid point
  {
    // update the position of the object
    _currentObject.sx = xi;
    _currentObject.sz = zi;
    _currentObject.ex = xi + 1;
    _currentObject.ez = zi + 1;

    // make sure the object is within the grid bounds still
    // First check the x values
    if( _currentObject.sx < 0 )
    {
      _currentObject.sx = 0;
      _currentObject.ex = 1;
    }
    if( _currentObject.sx > xsize - 2 )
    {
      _currentObject.sx = xsize - 2;
      _currentObject.ex = xsize - 1;
    }

    // Check the y(z) values
    if( _currentObject.sz < 0 )
    {
      _currentObject.sz = 0;
      _currentObject.ez = 1;
    }
    if( _currentObject.sz > zsize - 2 )
    {
      _currentObject.sz = zsize - 2;
      _currentObject.ez = zsize - 1;
    }

  }

  // rebuild the scene
  this->_rebuildObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the movement change
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::keyMovementChange( int x, int y )
{
  Guard guard ( this );

  int xsize ( static_cast< int > ( _xValues.size() ) );
  int ysize ( static_cast< int > ( _yValues.size() ) );
  int zsize ( static_cast< int > ( _zValues.size() ) );

  int buildMode ( this->getBuildMode2D() );

  if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY )
  {
    // update the position of the object
    _currentObject.sx += x;
    _currentObject.sz += y;
    _currentObject.ex += x;
    _currentObject.ez += y;

    // make sure the object is within the grid bounds still
    // First check the x values
    if( _currentObject.sx < 0 )
    {
      _currentObject.sx = 0;
      _currentObject.ex = 1;
    }
    if( _currentObject.sx > xsize - 2 )
    {
      _currentObject.sx = xsize - 2;
      _currentObject.ex = xsize - 1;
    }

    // Check the y(z) values
    if( _currentObject.sz < 0 )
    {
      _currentObject.sz = 0;
      _currentObject.ez = 1;
    }
    if( _currentObject.sz > zsize - 2 )
    {
      _currentObject.sz = zsize - 2;
      _currentObject.ez = zsize - 1;
    }

  }

  if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ )
  {
    // update the position of the object
    _currentObject.sx += x;
    _currentObject.sy += y;
    _currentObject.ex += x;
    _currentObject.ey += y;

    // make sure the object is within the grid bounds still
    // First check the x values
    if( _currentObject.sx < 0 )
    {
      _currentObject.sx = 0;
      _currentObject.ex = 1;
    }
    if( _currentObject.sx > xsize - 2 )
    {
      _currentObject.sx = xsize - 2;
      _currentObject.ex = xsize - 1;
    }

    // Check the y(z) values
    if( _currentObject.sy < 0 )
    {
      _currentObject.sy = 0;
      _currentObject.ey = 1;
    }
    if( _currentObject.sy > ysize - 2 )
    {
      _currentObject.sy = ysize - 2;
      _currentObject.ey = ysize - 1;
    }

  }

  if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XY )
  {

    // update the position of the object
    _currentObject.ex += x;
    _currentObject.ez += y;

    // make sure the object is within the grid bounds still
    // First check the x values
    if( _currentObject.ex <= _currentObject.sx )
    {
      _currentObject.ex = _currentObject.sx + 1;
    }
    if( _currentObject.ex > xsize - 1 )
    {
      _currentObject.ex = xsize - 1;
    }

    // Check the y(z) values
    if( _currentObject.ez <= _currentObject.sz )
    {
      _currentObject.ez = _currentObject.sz + 1;
    }
    if( _currentObject.ez > zsize - 1 )
    {
      _currentObject.ez = zsize - 1;
    }

  }

  if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XZ )
  {

    // update the position of the object
    _currentObject.ex += x;
    _currentObject.ey += y;

    // make sure the object is within the grid bounds still
    // First check the x values
    if( _currentObject.ex <= _currentObject.sx )
    {
      _currentObject.ex = _currentObject.sx + 1;
    }
    if( _currentObject.ex > xsize - 1 )
    {
      _currentObject.ex = xsize - 1;
    }

    // Check the y(z) values
    if( _currentObject.ey <= _currentObject.sy )
    {
      _currentObject.ey = _currentObject.sy + 1;
    }
    if( _currentObject.ey > ysize - 1 )
    {
      _currentObject.ey = ysize - 1;
    }

  }

  // rebuild the scene
  this->_rebuildObject();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Redraw the current object
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_rebuildObject()
{
  Guard guard( this );

  // Remove all the children
  _object2D->removeChildren( 0, _root2D->getNumChildren() );

  // add the object
  _object2D->addChild( this->_buildObject() );

  this->requestRedraw();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Draw the cracks on the 2D grid
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_drawCracks2D()
{
  Guard guard ( this );

  GroupPtr group ( new osg::Group );

  // Z Axis cracks
	for( unsigned int i = 0; i < _cracks.first.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.first.at( i ) );

    // convert the crack values
    float value ( StrToFloat::convert( c.value ) );
    float start ( StrToFloat::convert( c.start ) );
    float end   ( StrToFloat::convert( c.end   ) );

    // set the corners
    float y( value );

    // set the "z" corners
    float sx ( start );
    float ex ( end );

    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

#if 1
    points->push_back( osg::Vec3f ( sx, 0.0001, y ) );
    points->push_back( osg::Vec3f ( ex, 0.0001, y ) );
    points->push_back( osg::Vec3f ( ex, 0.0001, y ) );
    points->push_back( osg::Vec3f ( sx, 0.0001, y ) );
#else
    points->push_back( osg::Vec3f ( y, 0.0001, sx ) );
    points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    points->push_back( osg::Vec3f ( y, 0.0001, sx ) );
#endif

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

  }

  // X axis cracks
	for( unsigned int i = 0; i < _cracks.second.size(); ++ i )
	{
    // get the crack
    Crack c ( _cracks.second.at( i ) );

    // convert the crack values
    float value ( StrToFloat::convert( c.value ) );
    float start ( StrToFloat::convert( c.start ) );
    float end   ( StrToFloat::convert( c.end   ) );

    // set the corners
    float y( value );

    // set the "z" corners
    float sx ( start  );
    float ex ( end );

    // points of the plane
    osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

#if 0
    points->push_back( osg::Vec3f ( sx, 0.0001, y ) );
    points->push_back( osg::Vec3f ( ex, 0.0001, y ) );
    points->push_back( osg::Vec3f ( ex, 0.0001, y ) );
    points->push_back( osg::Vec3f ( sx, 0.0001, y ) );
#else
    points->push_back( osg::Vec3f ( y, 0.0001, sx ) );
    points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    points->push_back( osg::Vec3f ( y, 0.0001, sx ) );
#endif

    //points->push_back( osg::Vec3f ( y, 0.0001, sx ) );
    //points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    //points->push_back( osg::Vec3f ( y, 0.0001, ex ) );
    //points->push_back( osg::Vec3f ( y, 0.0001, sx ) );

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 1.0f, 0.0f, 1.0f ) ) );

  }

  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the building on the 2D grid
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_drawBuilding2D()
{
  Guard guard( this );
  
  GroupPtr group ( new osg::Group );

  // points of the plane
  osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

  // get the view mode
  int viewMode ( this->getViewMode2D() );

  if( IVPI::VIEW_MODE_2D_XY == viewMode )
  {
    Building b ( this-> building() );
    //set the points

    float sx ( StrToFloat::convert( b.x ) );
    float sz ( StrToFloat::convert( b.z ) );
    float ex ( StrToFloat::convert( b.x ) + StrToFloat::convert( b.l ) );
    float ez ( StrToFloat::convert( b.z ) + StrToFloat::convert( b.w ) );
    
    points->push_back( osg::Vec3f ( sx, 0.001, sz ) );
    points->push_back( osg::Vec3f ( ex, 0.001, sz ) );
    points->push_back( osg::Vec3f ( ex, 0.001, ez ) );
    points->push_back( osg::Vec3f ( sx, 0.001, ez ) );

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
  }

  if( IVPI::VIEW_MODE_2D_XZ == viewMode )
  {
    Building b ( this-> building() );
    //set the points

    float sx ( StrToFloat::convert( b.x ) );
    float sy ( StrToFloat::convert( b.y ) );
    float ex ( StrToFloat::convert( b.x ) + StrToFloat::convert( b.l ) );
    float ey ( StrToFloat::convert( b.y ) + StrToFloat::convert( b.h ) );
    
    points->push_back( osg::Vec3f ( sx, sy, 0.001 ) );
    points->push_back( osg::Vec3f ( ex, sy, 0.001 ) );
    points->push_back( osg::Vec3f ( ex, ey, 0.001 ) );
    points->push_back( osg::Vec3f ( sx, ey, 0.001 ) );

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 0.0f, 0.0f, 1.0f, 1.0f ) ) );
  }
  
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the 2D objects
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_build2DObjects()
{
   Guard guard ( this );

  if( true == _useBuilding )
  {
    // add the building
    _root2D->addChild( this->_drawBuilding2D() );

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the object
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VaporIntrusionGUIDocument::_buildObject()
{
  Guard guard ( this );

  GroupPtr group ( new osg::Group );

  // points of the plane
  osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );

  // Get the current build mode
  int buildMode ( this->getBuildMode2D() );

  // If we are building the XY plane
  if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XY || buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY )
  {
    //set the points

    float sx ( _xValues.at( _currentObject.sx ).first );
    float sz ( _zValues.at( _currentObject.sz ).first );
    float ex ( _xValues.at( _currentObject.ex ).first );
    float ez ( _zValues.at( _currentObject.ez ).first );
    
    points->push_back( osg::Vec3f ( sx, 0.001, sz ) );
    points->push_back( osg::Vec3f ( ex, 0.001, sz ) );
    points->push_back( osg::Vec3f ( ex, 0.001, ez ) );
    points->push_back( osg::Vec3f ( sx, 0.001, ez ) );

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
  }

  if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XZ || buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ )
  {
    //set the points

    float sx ( _xValues.at( _currentObject.sx ).first );
    float sy ( _yValues.at( _currentObject.sy ).first );
    float ex ( _xValues.at( _currentObject.ex ).first );
    float ey ( _yValues.at( _currentObject.ey ).first );
    
    points->push_back( osg::Vec3f ( sx, sy, 0.001 ) );
    points->push_back( osg::Vec3f ( ex, sy, 0.001 ) );
    points->push_back( osg::Vec3f ( ex, ey, 0.001 ) );
    points->push_back( osg::Vec3f ( sx, ey, 0.001 ) );

    group->addChild ( this->_buildPlane( points.get(), osg::Vec4f ( 1.0f, 0.0f, 0.0f, 1.0f ) ) );
  }


  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the camera position from the view mode
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_setCameraFromViewMode( int mode )
{
  Guard guard ( this );

  switch( mode )
  {
    case IVPI::VIEW_MODE_2D_XY :
    {
      // set the camera mode to top
      Usul::Interfaces::ICamera::QueryPtr camera ( Usul::Documents::Manager::instance().activeView() );
      if( true == camera.valid() )
      {
        camera->camera( OsgTools::Render::Viewer::TOP );
      }
      
      // feedback
      std::cout << "Setting 2D Grid Domain to the XY Grid" << std::endl; 
    }
    break;
    case IVPI::VIEW_MODE_2D_XZ :
    {
      // set the camera mode to top
      Usul::Interfaces::ICamera::QueryPtr camera ( Usul::Documents::Manager::instance().activeView() );
      if( true == camera.valid() )
      {
        camera->camera( OsgTools::Render::Viewer::FRONT );
      }
      
      // feedback
      std::cout << "Setting 2D Grid Domain Mode to the XZ (Basement/Soil) Grid" << std::endl;
    }
    break;
    default:
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a source to the experiment
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::objectMenuAddSource()
{
  Guard guard ( this );

   // set the edit mode to object placement
  this->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );

  // set the correct build mode
  this->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

  // set the object type to SOURCE
  this->setObjectMode( IVPI::OBJECT_SOURCE );

}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the object
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::objectMenuAddBuilding()
{
  Guard guard ( this );

  // set the edit mode to object placement
  this->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );

  // set the correct build mode
  this->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

  // set the object type to building
  this->setObjectMode( IVPI::OBJECT_BUILDING );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create a new object
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::handleNewObject()
{
  Guard guard ( this );

  // create a new building
  if( _objectMode == IVPI::OBJECT_BUILDING )
  {
    this->_createNewBuilding();
  }

  // create a new source
  if( _objectMode == IVPI::OBJECT_SOURCE )
  {
    // create the source
    this->_createNewSource();

    //// launch properties dialog for newly made source
    //SourcePropertiesDialog spd;

    //// set the default source 
    //spd.source( _sources.at( _sources.size() - 1 ) );

    //// Show the dialog.
    //if ( QDialog::Accepted != spd.exec() )
    //throw Usul::Exceptions::Canceled();

    //// get the modified source
    //Source s ( spd.source() );

    //// update the source
    //_sources.at( _sources.size() - 1 ) = s;
    
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new source
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_createNewSource()
{
  Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< float, std::string > FTS;

  // get the corners
  float sx ( _xValues.at( _currentObject.sx ).first );
  float ex ( _xValues.at( _currentObject.ex ).first );
  float sy ( _yValues.at( _currentObject.sy ).first );
  float ey ( _yValues.at( _currentObject.ey ).first );
  float sz ( _zValues.at( _currentObject.sz ).first );
  float ez ( _zValues.at( _currentObject.ez ).first );

  // Calculate the lwh
  float l ( ex - sx );
  float w ( ez - sz );
  float h ( ey - sy );

  IVPI::Chemicals c;

  // create a building object with the parameters entered in the 2D window
  IVPI::Source s ( Usul::Strings::format ( l ),
                   Usul::Strings::format ( w ),
                   Usul::Strings::format ( h ),
                   Usul::Strings::format ( sx ),
                   Usul::Strings::format ( sy ),
                   Usul::Strings::format ( sz ),
                   "Untitled", c );

  // generate a random color for the source
  s.color = this->_randomColor();

  // add the source to the list of sources
  _sources.push_back( s );

  //rebuild the scene
  this->rebuildScene();

}

///////////////////////////////////////////////////////////////////////////////
//
//  Generate a random color
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f VaporIntrusionGUIDocument::_randomColor()
{
 
  // seed the random number generator
  srand( time( NULL ) );

  // generate the channels of the color
  float r ( static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) );
  float red   ( ( r / 3 ) / 100.0f );
 
  float g ( static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) ); 
  float green ( ( g / 3 ) / 100.0f );

  float b ( static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) + static_cast< float > ( 1 + rand() % 100 ) ); 
  float blue  ( ( b / 3 ) / 100.0f );
  

  // generate the color
  Usul::Math::Vec4f color ( red, green, blue, 1.0f ); 

  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new building
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_createNewBuilding()
{
  Guard guard ( this );

  // useful typedefs
  typedef Usul::Convert::Type< float, std::string > FTS;

  // get the building corners
  float sx ( _xValues.at( _currentObject.sx ).first );
  float ex ( _xValues.at( _currentObject.ex ).first );
  float sy ( _yValues.at( _currentObject.sy ).first );
  float ey ( _yValues.at( _currentObject.ey ).first );
  float sz ( _zValues.at( _currentObject.sz ).first );
  float ez ( _zValues.at( _currentObject.ez ).first );

  // Calculate the lwh
  float l ( ex - sx );
  float w ( ez - sz );
  float h ( ey - sy );

  // create a building object with the parameters entered in the 2D window
  Building b ( FTS::convert( l ), FTS::convert( w ), FTS::convert( h ), FTS::convert( sx ), FTS::convert( sy ), FTS::convert( sz ), "0", "1", "1", "1" );

  // set the building
  _building = b;

  // tell the document that there is a building
  this->useBuilding( true );

  //rebuild the scene
  this->rebuildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create labels for the grid points.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * VaporIntrusionGUIDocument::_createGridLabels2D()
{
  Guard guard( this );

  // group to store the labels
  GroupPtr group ( new osg::Group );

  // get the view mode
  int viewMode ( this->getViewMode2D() );  

  if( viewMode == IVPI::VIEW_MODE_2D_XY )
  {    
    // Create the X axis labels
    {
      // create a vec3array to hold the positions
      osg::ref_ptr< osg::Vec3Array > positions ( new osg::Vec3Array );

      // create the labels
      StringVec labels;

      // get the component values
      float x ( _originalXValues.at( 0 ).first );        
      float y ( 0.0f );
      float z ( _originalZValues.at( _originalZValues.size() - 1 ).first );

      // adjust z position modifier
      float zMod ( 1.05 );

      // adjust the x so it is outside the grid
      float xMod (  0.25 * abs( _originalXValues.at( 1 ).first - x ) );

      // add the vector of positions
      positions->push_back( osg::Vec3 ( x - xMod, y, z * zMod ) );

      // add the label
      labels.push_back( Usul::Strings::format( "(", z, ")(", x, ")" ) );
      
      for( unsigned int i = 1; i < _originalXValues.size(); ++i )
      {
        // get the component values
        x = _originalXValues.at( i ).first; 

        // add the vector of positions
        positions->push_back( osg::Vec3 ( x, y, z * zMod ) );

        // add the label
        labels.push_back( Usul::Strings::format( x ) );
        
      }

      // create the labels
      group->addChild( this->_createText( positions, labels ) );
    }
    {
      // create a vec3array to hold the positions
      osg::ref_ptr< osg::Vec3Array > positions ( new osg::Vec3Array );

      // create the labels
      StringVec labels;

      // get the x component
      float x ( _originalXValues.at( 0 ).first ); 

      // adjust the x so it is outside the grid
      float xMod (  0.25 * abs( _originalXValues.at( 1 ).first - x ) );

      // set the y component
      float y ( 0.0f );
      
      for( unsigned int i = 0; i < _originalZValues.size(); ++i )
      {
        // get the component values
        float z ( _originalZValues.at( i ).first );     

        // add the vector of positions
        positions->push_back( osg::Vec3 ( x - xMod, y, z ) );

        // add the label
        labels.push_back( Usul::Strings::format( z ) );
      }

      // create the labels
      group->addChild( this->_createText( positions, labels ) );
    }
  }

  if( viewMode == IVPI::VIEW_MODE_2D_XZ )
  {
    // Create the X axis labels
    {
      // create a vec3array to hold the positions
      osg::ref_ptr< osg::Vec3Array > positions ( new osg::Vec3Array );

      // create the labels
      StringVec labels;

      // get the component values
      float x ( _originalXValues.at( 0 ).first );        
      float y ( _originalYValues.at( _originalYValues.size() - 1 ).first );
      float z ( 0.0f );

      // adjust z position modifier
      float yMod ( 1.05 );

      // adjust the x so it is outside the grid
      float xMod (  0.25 * abs( _originalXValues.at( 1 ).first - x ) );

      // add the vector of positions
      positions->push_back( osg::Vec3 ( x - xMod, y * yMod, z ) );

      // add the label
      labels.push_back( Usul::Strings::format( "(", y, ")(", x, ")" ) );
      
      for( unsigned int i = 1; i < _originalXValues.size(); ++i )
      {
        // get the component values
        x = _originalXValues.at( i ).first;        

        // add the vector of positions
        positions->push_back( osg::Vec3 ( x, y * yMod, z ) );

        // add the label
        labels.push_back( Usul::Strings::format( x ) );
      }

      // create the labels
      group->addChild( this->_createText( positions, labels ) );
    }
    {
      // create a vec3array to hold the positions
      osg::ref_ptr< osg::Vec3Array > positions ( new osg::Vec3Array );

      // create the labels
      StringVec labels;

      // x and z values
      float x ( _originalXValues.at( 0 ).first );
      float z ( 0.0f );  

      // adjust the x so it is outside the grid
      float xMod (  0.25 * abs( _originalXValues.at( 1 ).first - x ) );
      
      for( unsigned int i = 0; i < _originalYValues.size(); ++i )
      {
        // get the y value
        float y ( _originalYValues.at( i ).first );

        // add the vector of positions
        positions->push_back( osg::Vec3 ( x - xMod, y, z ) );

        // add the label
        labels.push_back( Usul::Strings::format( y ) );
      }

      // create the labels
      group->addChild( this->_createText( positions, labels ) );
    }
  }

  return group.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create text objects from the input positions
//  Used to label the grid points.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * VaporIntrusionGUIDocument::_createText( osg::Vec3Array* positions, StringVec labels )
{
  Guard guard( this );

  if( labels.size() != positions->size() )
  {
    return new osg::Group;
  }

  // create the font for the text
  osg::ref_ptr< osgText::Font > font ( osgText::readFontFile ( "fonts/arial.ttf" ) );

  // create the geode
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // create the stateset
  osg::ref_ptr< osg::StateSet > stateset ( geode->getOrCreateStateSet() );

  for( unsigned int i = 0; i < positions->size(); ++i )
  {
    // create the text object
    osg::ref_ptr< osgText::Text  > text ( new osgText::Text() );

    // set the font, color, size mode
    text->setFont( font.get() );
    text->setColor( osg::Vec4f( 0, 0, 0, 1 ) );
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );

    // don't use display lists
    text->setUseDisplayList( false );

    // get the vec2 of this position
    osg::Vec3 p ( positions->at( i ) );
    
    // set the character size, position, layout, resolution and height
    text->setCharacterSize( 20 );
    text->setPosition( p );
    text->setLayout( osgText::Text::LEFT_TO_RIGHT );
    text->setFontResolution ( 32, 32 );
    text->setMaximumHeight( 50 );

    // create the string for the text
    std::string tStr( Usul::Strings::format( labels.at( i ) ) );
    
    // set the text and set to autorotate to the screen
    text->setText( tStr );
    text->setAutoRotateToScreen( true );

    // add the text object to the geode
    geode->addDrawable( text.get() );

  }

  // make sure the stateset is set to fill and is protected
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
  mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
  stateset->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  // set the geode stateset
  geode->setStateSet( stateset.get() );
  
  // return the geode
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the mouse coordinates in world space
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::setMouseCoords( Usul::Math::Vec3f point )
{
  Guard guard ( this );

  // get the current view mode
  int viewMode ( this->getViewMode2D() );

  if( viewMode == IVPI::VIEW_MODE_2D_XY )
  {
    _mouseXCoord = point[0];
    _mouseYCoord = point[2];
  }

  if( viewMode == IVPI::VIEW_MODE_2D_XZ )
  {
    _mouseXCoord = point[0];
    _mouseYCoord = point[1];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the status text
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIDocument::_setStatusText( const std::string message, unsigned int &textXPos, unsigned int &textYPos,
                                                double xmult, double ymult, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  // useful typedefs
  typedef Usul::Interfaces::ITextMatrix::QueryPtr ITextPtr;
  typedef Usul::Interfaces::IViewport::QueryPtr   IViewPortPtr;

  ITextPtr textMatrix ( caller );
  if( false == textMatrix.valid() )
    throw std::runtime_error ( "Error 3793514250: Failed to find a valid interface to Usul::Interfaces::ITextMatrix " );

  IViewPortPtr viewPort( caller );
  if( false == viewPort.valid() )
    throw std::runtime_error ( "Error 2482359443: Failed to find a valid interface to Usul::Interfaces::IViewport " );

  textMatrix->removeText( static_cast< unsigned int > ( textXPos ),
                          static_cast< unsigned int > ( textYPos ) );
   
  const double xpos ( ::floor( viewPort->width()  * xmult ) );
  const double ypos ( ::floor( viewPort->height() * ymult ) );

  osg::Vec4f fcolor (  1.0, 1.0, 1.0, 1 );
  osg::Vec4f bcolor (  0.0, 0.0, 0.0, 1 );

  textXPos = static_cast< unsigned int > ( xpos );
  textYPos = static_cast< unsigned int > ( ypos );

  textMatrix->setText ( textXPos, textYPos, message, fcolor, bcolor );
}
