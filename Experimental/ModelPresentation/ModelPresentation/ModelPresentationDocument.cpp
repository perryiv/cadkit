
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModelPresentationDocument.h"
#include "MpdMenuCommand.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"

#include <iterator>


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
  _sets ( 0x0 )
{
  USUL_TRACE_SCOPE;
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
  case Usul::Interfaces::ICommandList::IID:
    return static_cast < Usul::Interfaces::ICommandList* > ( this ); 
  case Usul::Interfaces::IMpdNavigator::IID:
    return static_cast < Usul::Interfaces::IMpdNavigator* > ( this ); 
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

void ModelPresentationDocument::read ( const std::string &name, Unknown *caller )
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
  filters.push_back ( Filter ( "Model Presentation(*.mpd)", "*.mpd" ) );
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
  filters.push_back ( Filter ( "Model Presentation(*.mpd)", "*.mpd" ) );
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
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
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
    if ( "numsets" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _dir );
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
    if ( "static" == node->name() )
    {
      std::cout << "Parsing static entries..." << std::endl;
      this->_parseStatic( *node, caller );
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
    if ( "path" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, path );
      group->addChild( this->_loadPath( path, caller ) );
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

osg::Node* ModelPresentationDocument::_loadPath( const std::string& path, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::string slash ( Usul::File::slash() );
  std::cout << path << " single file loading..." << std::endl;
  osg::ref_ptr< osg::Node > loadedModel = osgDB::readNodeFile( path );
  return loadedModel.release();
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
  Files osgfiles;
  Usul::File::find( dir, "osg", osgfiles );

  Files ivefiles;
  Usul::File::find( dir, "ive", ivefiles );
  
  osg::ref_ptr< osg::Group> group ( new osg::Group );
  for( unsigned int i = 0; i < osgfiles.size(); ++i )
  {
    std::cout << "\tLoading file: " << osgfiles.at(i).c_str() << std::endl;
    osg::ref_ptr< osg::Node > loadedModel = osgDB::readNodeFile( osgfiles.at(i).c_str() );
    group->addChild( loadedModel.release() );
  }
  for( unsigned int i = 0; i < ivefiles.size(); ++i )
  {
    std::cout << "\tLoading file: " << osgfiles.at(i).c_str() << std::endl;
    osg::ref_ptr< osg::Node > loadedModel = osgDB::readNodeFile( ivefiles.at(i).c_str() );
    group->addChild( loadedModel.release() );
  }

  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the command list.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::CommandList ModelPresentationDocument::getCommandList()
{
  USUL_TRACE_SCOPE;
  CommandList cl;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  std::cout << me.get () << std::endl;

  for( unsigned int i = 0; i < _sets.size(); ++i )
  {
    cl.push_back( new MpdMenuCommand( me.get(), _sets.at(i).name, i ) );
  }
  //cl.push_back( new NextTimestep( me.get() ) );
  
  return cl;

}
