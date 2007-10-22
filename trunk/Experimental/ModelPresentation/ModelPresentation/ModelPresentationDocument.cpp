
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/ModelPresentation/ModelPresentation/ModelPresentationDocument.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdMenuCommand.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdPrevTimestep.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdStartAnimation.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdStopAnimation.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdFirstTimestep.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdNextCommand.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdLocation.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdTools.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"


#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>
#include <Math.h>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModelPresentationDocument, ModelPresentationDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::ModelPresentationDocument() : 
  BaseClass ( "Model Presentation Document" ),
  _root ( new osg::Group ),
  _static ( new osg::Group ),
  _sceneTree ( 0x0 ),
  _sets ( 0x0 ),
  _update( UpdatePolicyPtr( new UpdatePolicy( 10 ) ) ),
  _useTimeLine( false ),
  _isAnimating( false ),
  _showTools ( false )
{
  USUL_TRACE_SCOPE;
  _timeSet.currentTime = 0;
  _timeSet.endTime = 200;
  _timeSet.interval = 1;
  _timeSet.timeline = new osg::Switch;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::~ModelPresentationDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModelPresentationDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMpdNavigator::IID:
    return static_cast < Usul::Interfaces::IMpdNavigator* > ( this ); 
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "mpd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "mpd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::write ( const std::string &name, Unknown *caller  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  if ( "mpd" == document->name() )
  {
    this->_readParameterFile( *document, caller );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Model Presentation (*.mpd)", "*.mpd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersExport() const
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

ModelPresentationDocument::Filters ModelPresentationDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Model Presentation (*.mpd)", "*.mpd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModelPresentationDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  _root->removeChild( 0, _root->getNumChildren() );
  for( unsigned int i = 0; i < _sceneTree.size(); ++i )
  {
    _root->addChild( _sceneTree.at( i ).get() );
  }
  _root->addChild( _static.get() );
  _root->addChild( _timeSet.timeline.get() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the time steps to see if anything needs to he hidden/shown.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_checkTimeSteps()
{
  for( unsigned int i = 0; i < _timeSet.groups.size(); ++i )
  { 
    // turn on groups that should be shown at this time step
    if( _timeSet.currentTime >=_timeSet.groups.at( i ).startTime  &&
        _timeSet.currentTime < _timeSet.groups.at( i ).endTime  )
    {      
      _timeSet.timeline->setValue( i, true );
    }
    // turn off groups that should be hidden at this time step
    if( _timeSet.currentTime < _timeSet.groups.at( i ).startTime   ||
        _timeSet.currentTime >= _timeSet.groups.at( i ).endTime )
    {
      _timeSet.timeline->setValue( i, false );
    }

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  if( true == _useTimeLine )
  {
    std::ostringstream text;
    text << "  Year " << _timeSet.currentTime;
    this->_setStatusBar( text.str(), caller );
  }

  if( true == this->isAnimating() )
  {
    if( true == (*_update)() )
    {
      if( _timeSet.currentTime == _timeSet.endTime )
        _timeSet.currentTime = 0;
      else
        _timeSet.currentTime += _timeSet.interval;

      this->_checkTimeSteps();

    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::nextGroup ( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_sceneTree.at( index )->setValue( _sets.at( index ).index, false );
  if( _sets.at( index ).index < this->_sceneTree.at( index )->getNumChildren() - 1 )
    _sets.at( index ).index ++;
  else
    _sets.at( index ).index = 0;

  this->_sceneTree.at( index )->setValue( _sets.at( index ).index, true );
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::nextStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( false == this->isAnimating() )
  {
    if( _timeSet.currentTime == _timeSet.endTime )
      _timeSet.currentTime = 0;
    else
      _timeSet.currentTime ++;
    this->_checkTimeSteps();

  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::prevStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( false == this->isAnimating() )
  {
    if( _timeSet.currentTime == 0 )
      _timeSet.currentTime = _timeSet.endTime;
    else
      _timeSet.currentTime --;
    this->_checkTimeSteps();

  }
  
}



///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::firstStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( false == this->isAnimating() )
  {
    
      _timeSet.currentTime = 0;
      this->_checkTimeSteps();

  }
  
  
}



///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::startAnimation ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->isAnimating( true );
  
}



///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::stopAnimation ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->isAnimating( false );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the current view matrix
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::displayViewMatrix()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  Usul::Interfaces::IViewMatrix::QueryPtr view ( Usul::Documents::Manager::instance().activeView() );

  if( true == view.valid() )
  {
    //Usul::Math::Matrix44f m;
    //OsgTools::Convert::matrix ( view->getViewMatrix(), m );
    
    osg::Matrixf matrix = view->getViewMatrix();

    std::ostringstream out;
    //out << Usul::System::Host::name() << "_home_position.txt";
    //std::ofstream file ( out.str().c_str() );
    std::cout << "Current view matrix: " << std::endl;
    Usul::Print::matrix ( "", matrix.ptr(), out, 20 );
    std::cout << out.str().c_str() << std::endl;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return the current animation state
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::isAnimating()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _isAnimating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  set the current animation set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::isAnimating( bool value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _isAnimating = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the parameter file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::_readParameterFile( XmlTree::Node &node, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "usetools" == iter->first )
    {
      std::string value = "false";
      Usul::Strings::fromString ( iter->second, value );
      if( value == "true" )
        _showTools = true;
    }
  }

  Children& children ( node.children() );
	
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "set" == node->name() )
    {
      this->_parseSet( *node, caller );
    }
    if ( "timeset" == node->name() )
    {
      this->_parseTimeSet( *node, caller );
    }
    if ( "static" == node->name() )
    {
      std::cout << "Parsing static entries..." << std::endl;
      this->_parseStatic( *node, caller );
    } 
    if ( "location" == node->name() )
    {
      std::cout << "Parsing location entries..." << std::endl;
      this->_parseLocation( *node, caller );
    } 
  }
  
  return true;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a static set of models (shown in every scene).
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseStatic( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
           
  }
  // TODO: create a set here --
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "model" == node->name() )
    {
      std::cout << "Found static model: " << std::flush;
      _static->addChild( this->_parseModel( *node, caller ) );
    }  
  }
#if 1
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( _static.get() );
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a location.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseLocation( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ok to update the status bar with time related information
  _useTimeLine = true;

  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  
  std::string name = "Location";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  { 
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, name );
    }  
  }
  // TODO: create destination matrix here --
  _locationNames.push_back( name );
  
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "matrix" == node->name() )
    {
      
      std::cout << "Found matrix" << std::endl;
     
      this->_parseMatrix( *node, caller, name);
    }
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a matrix
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseMatrix( XmlTree::Node &node, Unknown *caller, const std::string& name )
{
   USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ok to update the status bar with time related information
  //_useTimeLine = true;

  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  std::string type;
  std::string value;
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
    if ( "type" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, type );
    }
    if ( "value" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, value );
    }
 
  }
  // TODO: create destination matrix here --
  osg::Matrix matrix;
  if( value.size() > 0 && type.size() > 0 )
    this->_setMatrix( &matrix, value, type );
  else
    std::cout << "Incorrect Matrix format!" << std::endl;

  
  _locations[ name ] = matrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a matrix location
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_setMatrix( osg::Matrix * matrix, const std::string& values, const std::string& type )
{
  std::istringstream is( values );
  
  osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
  if( "xyzpry" == type )
  {
    std::cout << "Found XYZ, Pitch, Roll, Yaw designation location" << std::endl;

    float x ( 0 ), y( 0 ), z( 0 ), pitch( 0 ), roll( 0 ), yaw( 0 );
    is >> x >> y >> z >> pitch >> roll >> yaw;

    
    mt->setMatrix (  osg::Matrix::rotate ( osg::inDegrees ( pitch ), 1.0f, 0.0f, 0.0f ) *
                     osg::Matrix::rotate ( osg::inDegrees ( roll ), 0.0f, 1.0f, 0.0f ) *
                     osg::Matrix::rotate ( osg::inDegrees ( yaw ), 0.0f, 0.0f, 1.0f ) *
                     osg::Matrix::translate ( osg::Vec3f(x, y, z) ) );
    matrix->setRotate( mt->getMatrix().getRotate() );
    matrix->translate( mt->getMatrix().getTrans() );
    
  }
  if( "matrix" == type )
  {
    std::cout << "Found matrix designation location" << std::endl;
    char temp;
    osg::Matrixf::value_type m[16];
      is >> m[0] >> temp >> m[4] >> temp >> m[8] >> temp  >> m[12] >> temp;
      is >> m[1] >> temp >> m[5] >> temp >> m[9] >> temp  >> m[13] >> temp;
      is >> m[2] >> temp >> m[6] >> temp >> m[10] >> temp >> m[14] >> temp;
      is >> m[3] >> temp >> m[7] >> temp >> m[11] >> temp >> m[15];
    matrix->set( m );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseSet( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  MpdSet set;
  set.index = 0;
  set.name = "Set";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, set.name );
    }
    if ( "numgroups" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
    }
        
  }
  // TODO: create a set here --
  osg::ref_ptr< osg::Switch > switchNode ( new osg::Switch );

  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "group" == node->name() )
    {
      std::cout << "Found group..." << std::endl;
      switchNode->addChild( this->_parseGroup( *node, caller ), false );
    }
  }
  switchNode->setValue( 0, true );

#if 1
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( switchNode.get() );
#endif

  this->_sets.push_back( set );
  this->_sceneTree.push_back( switchNode.release() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a group.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseGroup( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
    }
    if ( "numitems" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
    }
        
  }
  
  // TODO: Create group here ---
  GroupPtr group ( new osg::Group );

  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "model" == node->name() )
    {
      std::cout << "Found group model: " << std::flush;
      group->addChild( this->_parseModel( *node, caller ) );
    }  
  }
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a time line set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseTimeSet( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ok to update the status bar with time related information
  _useTimeLine = true;

  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
    if ( "endTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _timeSet.endTime );
    }
    if ( "interval" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _timeSet.interval );
    }
    if ( "timelength" == iter->first )
    {
      unsigned int interval = 10;
      Usul::Strings::fromString ( iter->second, interval );
      std::cout << "Setting time update interval to " << interval << std::endl;
      _update = ( UpdatePolicyPtr( new UpdatePolicy( interval ) ) );
    }
        
  }
  // TODO: create a set here --
  osg::ref_ptr< osg::Switch > switchNode ( new osg::Switch );

  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "group" == node->name() )
    {
      std::cout << "Found group..." << std::endl;
      switchNode->addChild( this->_parseTimeGroup( *node, caller ), false );
    }
  }
  

#if 1
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( switchNode.get() );
#endif
  _timeSet.timeline = switchNode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a time line group.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseTimeGroup( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  MpdTimeGroup timeGroup;
  timeGroup.startTime = 0;
  timeGroup.endTime = _timeSet.endTime;

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "startTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeGroup.startTime );
    }
    if ( "endTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeGroup.endTime );
    }    
  }
  _timeSet.groups.push_back( timeGroup );

  GroupPtr group ( new osg::Group );

  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "model" == node->name() )
    {
      std::cout << "Found timeline group model: " << std::flush;
      group->addChild( this->_parseModel( *node, caller ) );
    }  
  }
  return group.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseModel( XmlTree::Node &node, Unknown *caller )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  Attributes& attributes ( node.attributes() );
  std::string path;
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "file" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, path );
      group->addChild( this->_loadFile( path, caller ) );
    }
    if ( "directory" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, path );
      group->addChild( this->_loadDirectory( path, caller ) );
    }
           
  }
  
  return group.release();  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a single model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_loadFile( const std::string& filename, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  std::cout << filename << " single file loading..." << std::endl;
  try
  {
     // This will create a new document.
    Info info ( DocManager::instance().find ( filename, caller ) );

    // Check to see if we created a document.
    if ( true == info.document.valid() && info.document->canOpen( filename ) )
    {
      
      // Ask the document to open the file.
      try
      {
        this->_openDocument ( filename, info.document.get(), caller );
      


        // Disable Memory pools
        {
          Usul::Interfaces::IMemoryPool::QueryPtr pool ( info.document );
          if ( true == pool.valid() )
          {
            pool->usePool ( false );
          }
        }
        
        Usul::Interfaces::IBuildScene::QueryPtr build ( info.document.get() );
        if ( true == build.valid() )
        {

          OsgTools::Triangles::TriangleSet::Options opt;
          opt[ "normals" ] = "per-vertex";
          opt[ "colors" ]  = "per-vertex";

          // tell Triangle set to build its scene and assign the resulting node
          // to our internal node.

          // Reference count should go from 1->0 but not delete.
          group->addChild( build->buildScene( opt, caller ) );
          
         }
      }
      catch( ... )
      {
        std::cout << "\tUnable to open file: " << filename << std::endl;
      }
    }
    else
      std::cout << "\tUnknown file format for: " << filename << std::endl;
  }
  catch( ... )
  {
    std::cout << "\tUnknown file format for: " << filename << std::endl;
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a directory of models.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_loadDirectory( const std::string& dir, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::cout << dir << " directory loading..." << std::endl;
  Files files;
  Usul::File::find( dir, "*", files );
    
  osg::ref_ptr< osg::Group> group ( new osg::Group );
  std::cout << "Found " << files.size() << " files in " << dir << std::endl;
  for( unsigned int i = 0; i < files.size(); ++i )
  {
    std::string filename = "";//files.at(i).c_str();
     #ifdef _MSC_VER
        filename = files.at(i).c_str();
      #else
        filename =  dir + files.at(i).c_str() ;
      #endif
    std::cout << "\tLoading file: " << filename << std::endl;
    /*
    if( Usul::File::extension( filename.c_str() ) == "ive" ||
        Usul::File::extension( filename.c_str() ) == "IVE" ||
        Usul::File::extension( filename.c_str() ) == "OSG" ||
        Usul::File::extension( filename.c_str() ) == "osg" )
    {
      #ifdef _MSC_VER
        osg::ref_ptr< osg::Node > loadedModel = osgDB::readNodeFile( files.at(i).c_str() );
      #else
        osg::ref_ptr< osg::Node > loadedModel = osgDB::readNodeFile( dir + files.at(i).c_str() );
      #endif
      group->addChild( loadedModel.release() );
    }
    else
    */
    try
    {
       // This will create a new document.
      Info info ( DocManager::instance().find ( filename, caller ) );

      // Check to see if we created a document.
      if ( true == info.document.valid() && true == info.document->canOpen( filename ) )
      {       
        // Ask the document to open the file.
        try
        {
          
          this->_openDocument ( filename, info.document.get(), caller );

          // Disable Memory pools
          {
            Usul::Interfaces::IMemoryPool::QueryPtr pool ( info.document );
            if ( true == pool.valid() )
            {
              pool->usePool ( false );
            }
          }
          
          Usul::Interfaces::IBuildScene::QueryPtr build ( info.document.get() );
          if ( true == build.valid() )
          {

            OsgTools::Triangles::TriangleSet::Options opt;
            opt[ "normals" ] = "per-vertex";
            opt[ "colors" ]  = "per-vertex";

            group->addChild( build->buildScene( opt, caller ) );
            
           }
        }
        catch( ... )
        {
          std::cout << "\tUnable to open file: " << filename << std::endl;
        }
      }
      else
        std::cout << "\tUnknown file format for: " << filename << std::endl;
    }
    catch ( ... )
    {
      std::cout << "\tUnknown file format for: " << filename << std::endl;
    }
  }

  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;

  std::cout << "Opening file: " << file << " ... " << std::endl;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  std::cout << "Done" << std::endl;
  
	
}



///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build an animation path 
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::MatrixVec ModelPresentationDocument::_getInterpolationMatrices ( const osg::Matrixd &m1, const osg::Matrixd &m2 ) const
{
  USUL_TRACE_SCOPE;
  // Guard guard ( this ); Nothing to guard.
  
  osg::Vec3d m1trans ( m1.getTrans() );
  osg::Vec3d m2trans ( m2.getTrans() );

  osg::Vec3d m3trans ( osg::Vec3d( m1trans[0], m1trans[1], m1trans[2] + 1.0 ) );

  // ( p3 - p2 ) ^ ( p1 - p2 )
  osg::Vec3d normal ( ( m3trans - m2trans ) ^ ( m1trans - m2trans ) );
  normal.normalize();
  double d = sqrt( pow( ( m2trans[0] - m1trans[0] ), 2 ) +
                   pow( ( m2trans[1] - m1trans[1] ), 2 ) +
                   pow( ( m2trans[2] - m1trans[2] ), 2 ) );

  /*osg::Vec3d interpolated1 ( ( m2trans[0] - m1trans[0] ) * 0.25 ,
                             ( m2trans[1] - m1trans[1] ) * 0.25, 
                             ( m2trans[2] - m1trans[2] ) * 0.25 ); 

  osg::Vec3d interpolated2 ( ( m2trans[0] - m1trans[0] ) * 0.75 ,
                             ( m2trans[1] - m1trans[1] ) * 0.75, 
                             ( m2trans[2] - m1trans[2] ) * 0.75 ); */
  osg::Vec3d interpolated1( m1trans + ( ( m2trans - m1trans ) * 0.33 ) );
  osg::Vec3d interpolated2( m1trans + ( ( m2trans - m1trans ) * 0.67 ) );

  osg::Vec3d intpos1 ( interpolated1 + ( normal * ( d / 10 ) ) );
  osg::Vec3d intpos2 ( interpolated2 + ( normal * ( d / 10 ) ) );

  osg::Matrixd mprime1;
  osg::Matrixd mprime2;

  mprime1.setRotate( m1.getRotate() );
  mprime1.setTrans( intpos1 );

  mprime2.setRotate( m2.getRotate() );
  mprime2.setTrans( intpos2 );

  MatrixVec mvec;

  mvec.push_back( mprime1 );
  mvec.push_back( mprime2 );

  return mvec;  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build an animation path 
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::setAnimationPath ( const std::string& name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef IAnimatePath::PackedMatrices PackedMatrices;
  typedef IAnimatePath::PackedMatrix PackedMatrix;

  // Check number of locations.
  if ( _locations.size() <= 0 )
    return;

  // Make sure we have this location.
  if ( _locations.end() == _locations.find ( name ) )
    return;

  // Get needed interfaces.
  IAnimatePath::QueryPtr path ( Usul::Components::Manager::instance().getInterface ( IAnimatePath::IID ) );
  Usul::Interfaces::IViewMatrix::QueryPtr view ( Usul::Documents::Manager::instance().activeView() );
  if ( false == path.valid() || false == view.valid() )
    return;

  // The matrices.
  PackedMatrices matrices;

  // Get current matrix and append.
  const osg::Matrixd m0 ( view->getViewMatrix() );
  matrices.push_back ( PackedMatrix ( m0.ptr(), m0.ptr() + 16 ) );

  // Get final matrix but don't append yet.
  const osg::Matrixd m1 ( this->_locations[name] );

#if 1
  // Append middle matrices.
  const MatrixVec mvec ( this->_getInterpolationMatrices ( m0, m1 ) );
  for ( MatrixVec::const_iterator i = mvec.begin(); i != mvec.end(); ++i )
  {
    const osg::Matrixd &temp ( *i );
    matrices.push_back ( PackedMatrix ( temp.ptr(), temp.ptr() + 16 ) );
  }
#endif

#if 0
  Matrix mhalf = ( m0 );
  osg::Vec3d trans = mhalf.getTrans();
  mhalf.setTrans ( osg::Vec3d( trans[0], trans[1] + 1, trans[2] ) );
  mhalf.setRotate(  m1.getRotate() );
  matrices.push_back ( mhalf );
#endif

  // Append final matrix.
  matrices.push_back ( PackedMatrix ( m1.ptr(), m1.ptr() + 16 ) );

  // Animate through these matrices.
  path->animatePath ( matrices );

#if 0
  std::ostringstream out;
  Usul::Print::matrix ( "", matrices.at( 1 ).ptr(), out, 20 );
  std::cout << out.str().c_str() << std::endl;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::menuAdd ( MenuKit::Menu& menu )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  Radio;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add the model menu
  {
    MenuKit::Menu::RefPtr ModelMenu ( new MenuKit::Menu ( "Models", MenuKit::Menu::VERTICAL ) );
    for( unsigned int i = 0; i < _sets.size(); ++i )
    {
      //cl.push_back( new MpdMenuCommand( me.get(), _sets.at(i).name, i ) );
      ModelMenu->append ( new Radio ( new MpdMenuCommand( me.get(), _sets.at(i).name, i ) ) );
    }
    
    menu.append ( ModelMenu );
  }
  // Add the Timeline menu if needed
  if( true == _useTimeLine )
  {
    MenuKit::Menu::RefPtr TimelineMenu ( new MenuKit::Menu ( "Timeline", MenuKit::Menu::VERTICAL ) );

    TimelineMenu->append ( new Button ( new MpdNextCommand( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdPrevTimestep( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdFirstTimestep( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdStartAnimation( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdStopAnimation( me.get() ) ) );

    menu.append ( TimelineMenu );
    
  }
  if( _locationNames.size() > 0 )
  {
    MenuKit::Menu::RefPtr locationMenu ( new MenuKit::Menu ( "Locations", MenuKit::Menu::VERTICAL ) );
    for( unsigned int i = 0; i < _locationNames.size(); ++i )
    {  
       locationMenu->append( new Button ( new MpdLocation( me.get(), _locationNames.at( i )  ) ) );
    }
    menu.append( locationMenu );
  }
  if( true == _showTools )
  {
    MenuKit::Menu::RefPtr toolsMenu ( new MenuKit::Menu ( "Tools", MenuKit::Menu::VERTICAL ) );
    toolsMenu->append ( new Button ( new MpdTools( me.get(), "ShowMatrix" ) ) );
    menu.append( toolsMenu );
  }
  

}
