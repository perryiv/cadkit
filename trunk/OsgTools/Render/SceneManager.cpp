
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#include "OsgTools/Font.h"
#include "OsgTools/Render/Constants.h"

#include "osg/Geode"

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager (  ) :
  BaseClass(),
  _scene           ( new Group ),
  _clipNode        ( new osg::ClipNode ),
  _projectionNode  ( new osg::Projection ),
  _groupMap        (),
  _projectionMap   (),
  _textMap         ()
{
  _scene->addChild( _clipNode.get() );
 
  _scene->addChild ( _projectionNode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::~SceneManager (  ) 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * SceneManager::scene()
{
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node * SceneManager::scene() const
{
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::model( osg::Node* node )
{
  // Remove any thing that may be under the clip node
  _clipNode->removeChild ( 0 , _clipNode->getNumChildren() );

  // If we are given a valid node
  if ( 0x0 != node )
  {
    // Add the node
    _clipNode->addChild ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* SceneManager::model()
{
  if( _clipNode->getNumChildren() > 0 )
    return _clipNode->getChild( 0 );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node* SceneManager::model() const
{
  if( _clipNode->getNumChildren() > 0 )
    return _clipNode->getChild( 0 );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* SceneManager::groupGet ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[ key ];

  // Has the group been created
  if ( !group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName( key );

    // Add the group to the scene
    _scene->addChild( group.get() );
  }

  return group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::groupRemove ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[key];
  _scene->removeChild ( group.get() );
  group = 0x0;

  // Remove key from group map.
  _groupMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::groupHas ( const std::string& key ) const
{
  GroupMap::const_iterator i = _groupMap.find ( key );
  return i != _groupMap.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group under the projection node with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* SceneManager::projectionGroupGet ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _projectionMap[ key ];

  // Has the group been created
  if ( !group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName( key );

    // Add the group to the scene
    _projectionNode->addChild( group.get() );
    _projectionNode->dirtyBound();
  }

  return group.get();
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::projectionGroupRemove ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _projectionMap[key];
  _projectionNode->removeChild ( group.get() );
  _projectionNode->dirtyBound();
  group = 0x0;

  // Remove key from group map.
  _projectionMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool SceneManager::projectionGroupHas ( const std::string& key ) const
{
  GroupMap::const_iterator i = _projectionMap.find ( key );
  return i != _projectionMap.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Projection* SceneManager::projection()
{
  return _projectionNode.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clip node.
//
///////////////////////////////////////////////////////////////////////////////

osg::ClipNode* SceneManager::clipNode()
{
  return _clipNode.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clip node.
//
///////////////////////////////////////////////////////////////////////////////

const osg::ClipNode* SceneManager::clipNode() const
{
  return _clipNode.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get text at the (x,y) on the screen.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Text* SceneManager::getText( unsigned int x, unsigned int y )
{
  // Find the xy pair if it exists
  TextMap::iterator i = _textMap.find( TextMap::key_type ( x, y ) );
  if( i == _textMap.end() )
  {
    osg::ref_ptr < osgText::Text > text ( new osgText::Text );
    text->setPosition( osg::Vec3 ( x, y, 0 ) );
    text->setFont ( OsgTools::Font::defaultFont() );
    text->setColor( osg::Vec4 ( 0.0f,0.0f,0.0f,1.0f ) );
    text->setCharacterSize( 24.0f );

    _textMap.insert( TextMap::value_type( TextMap::key_type ( x, y ), text ) );

    osg::ref_ptr<osg::Group> group ( this->projectionGroupGet ( OsgTools::Render::Constants::TEXT_MATRIX ) );
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    geode->addDrawable( text.get() );

    group->addChild( geode.get() );

    return text.get();
  }

  return i->second.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set text value.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::setText( unsigned int x, unsigned int y, const std::string& text )
{
  this->getText(x,y)->setText( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove text.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::removeText( unsigned int x, unsigned int y )
{
  // Find the xy pair if it exists
  TextMap::iterator i = _textMap.find( TextMap::key_type ( x, y ) );
  if( i != _textMap.end() )
  {
    // Remove the text.
    osg::ref_ptr< osgText::Text > text ( i->second );

    // Get the parent.
    osg::ref_ptr < osg::Node > parent ( text->getParent( 0 ) );

    // Get the group
    osg::ref_ptr<osg::Group> group ( this->projectionGroupGet ( OsgTools::Render::Constants::TEXT_MATRIX ) );

    if ( osg::Geode *geode = dynamic_cast < osg::Geode* > ( parent.get() ) )
    {
      geode->removeDrawable( text.get() );
    }

    // Remove the parent of the text from the group.
    group->removeChild( parent.get() );
    
    // Remove the text from the map.
    _textMap.erase( i );
  }
}
