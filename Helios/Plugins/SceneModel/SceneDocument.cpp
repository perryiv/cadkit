
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneDocument.h"

#include "OsgTools/IO/STLWriter.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/ReadModel.h"
#include "OsgTools/ForEach.h"
#include "OsgTools/FlipNormals.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "osgDB/WriteFile"

#include <fstream>
#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneDocument, SceneDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::SceneDocument() : BaseClass ( "Scene Document" ), 
  _scene ( new osg::Group )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::~SceneDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IFlipNormals::IID:
    return static_cast < Usul::Interfaces::IFlipNormals * > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool SceneDocument::canExport ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" || ext == "osg" || ext == "stl" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool SceneDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool SceneDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" || ext == "osg" || ext == "yarn" || ext == "flt" || ext == "crss" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool SceneDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" || ext == "osg" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void SceneDocument::write ( const std::string &name, Unknown *caller  ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  //Write the file as an stl
  if( "stl" == ext )
  {    
    //make a copy of the options
    BaseClass::Options options ( this->options() );

    OsgTools::IO::STLWriter writer ( name );

    writer.header ( "Created by Helios. " + _scene->getName() );

    //Should we write in ascii?
    if( "ascii" == options["format"] )
    {
      writer.writeASCII( *_scene );
    }
    else if ( "binary" == options["format"] )
    { 
      writer.writeBinary ( *_scene );
    }
    else
    {
      std::ostringstream message;
      message << "Error 1251388893: " << options["format"] << " is an invalid file format, expected 'binary' or 'ascii'.";
      throw std::invalid_argument ( message.str() );    
    }

  }
  else
  {
    //Write the file using osg
    osgDB::writeNodeFile( *_scene, name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void SceneDocument::read ( const std::string &name, Unknown *caller )
{
  osg::ref_ptr< osg::Node > node ( OsgTools::readModel ( name ) );

  // If the root hasn't been created...
  if ( !_scene )
  {
    if ( node->asGroup() )
    {
      _scene = dynamic_cast< osg::Group* > ( node.get() );
    }
    else
    {
      _scene = new osg::Group;
      _scene->addChild ( node.get() );
    }
  }

  // Add to the scene.
  else
  {
    _scene->addChild( node.get() );
  }

  osg::Material *mat ( new osg::Material );
  _scene->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void SceneDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  _scene = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::Filters SceneDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::Filters SceneDocument::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  filters.push_back ( Filter ( "STL ASCII (*.stl)",             "*.stl" ) );
  filters.push_back ( Filter ( "STL Binary (*.stl)",            "*.stl" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::Filters SceneDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph (*.osg *.ive)",  "*.osg;*.ive" ) );
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg"       ) );
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive"       ) );
  filters.push_back ( Filter ( "YARN (*.yarn)",                 "*.yarn"      ) );
  filters.push_back ( Filter ( "CRSS (*.crss)",                 "*.crss"      ) );
  filters.push_back ( Filter ( "OpenFlight (*.flt)",            "*.flt"       ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

SceneDocument::Filters SceneDocument::filtersInsert() const
{
  return this->filtersOpen();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *SceneDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  // Return the scene
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the direction of the normal vectors if there is a valid scene.
//
///////////////////////////////////////////////////////////////////////////////

void SceneDocument::flipNormalVectors()
{
  // Check the scene.
  if ( !_scene.valid() )
    return;

  // Declare the visitor.
  typedef OsgTools::ForEach < OsgTools::FlipNormals, osg::Drawable > Operation;
  typedef OsgTools::Visitor < osg::Geode, Operation > Visitor;

  // Make the visitor.
  OsgTools::FlipNormals flip;
  Operation op ( flip );
  Visitor::Ptr visitor ( new Visitor ( op ) );

  // Traverse the models-scene and unselect all the nodes.
  _scene->accept ( *visitor );

  // We modified the document.
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void SceneDocument::notify ( unsigned short message )
{
  BaseClass::notify ( message );
}
