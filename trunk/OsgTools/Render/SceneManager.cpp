
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
#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/LightSource"
#include "osg/PolygonMode"
#include "osg/Version"

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::SceneManager() :
  BaseClass(),
  _scene           ( new Group ),
  _clipNode        ( new osg::ClipNode ),
  _cameraNode      ( new osg::Camera ),
  _lightNode       ( new osg::Group ),
  _lights          (),
  _groupMap        (),
  _projectionMap   (),
  _textMap         ()
{
  _scene->addChild ( _cameraNode.get() );
  _scene->addChild ( _clipNode.get() );
  _scene->addChild ( _lightNode.get() );

  // Make sure it draws last and without depth testing.
  _cameraNode->setRenderOrder ( osg::Camera::POST_RENDER );
  _cameraNode->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _cameraNode->setClearMask( GL_DEPTH_BUFFER_BIT );
  _cameraNode->setViewMatrix( osg::Matrix::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager::~SceneManager() 
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

void SceneManager::model ( osg::Node* node )
{
  // Remove any thing that may be under the clip node
  _clipNode->removeChild ( 0, _clipNode->getNumChildren() );

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
  return ( ( _clipNode->getNumChildren() > 0 ) ? _clipNode->getChild ( 0 ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node* SceneManager::model() const
{
  return ( ( _clipNode->getNumChildren() > 0 ) ? _clipNode->getChild ( 0 ) : 0x0 );
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
    group->setName ( key );

    // Add the group to the scene
    _scene->addChild ( group.get() );
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
//  Get group under the projection node with given key. 
//  Creates one if doesn't exist
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
    _cameraNode->addChild( group.get() );
    _cameraNode->dirtyBound();
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
  _cameraNode->removeChild ( group.get() );
  _cameraNode->dirtyBound();
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
//  Get the camera node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Camera* SceneManager::camera()
{
  return _cameraNode.get();
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

osgText::Text* SceneManager::getText ( unsigned int x, unsigned int y )
{
  // Find the xy pair if it exists
  TextMap::iterator i = _textMap.find ( TextMap::key_type ( x, y ) );
  if ( _textMap.end() == i )
  {
    osg::ref_ptr < osgText::Text > text ( new osgText::Text );
    text->setPosition( osg::Vec3 ( x, y, 0 ) );

    osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
    text->setFont ( font.get() );

    text->setColor ( osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) );

#if OPENSCENEGRAPH_MAJOR_VERSION >= 2 && OPENSCENEGRAPH_MINOR_VERSION >= 3
    text->setCharacterSize ( 24.0f );
#else
    text->setCharacterSize ( 12.0f );
#endif

    _textMap.insert ( TextMap::value_type( TextMap::key_type ( x, y ), text ) );

    osg::ref_ptr<osg::Group> group ( this->projectionGroupGet ( OsgTools::Render::Constants::TEXT_MATRIX ) );
    
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    geode->addDrawable ( text.get() );

    mt->addChild ( geode.get() );
    osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL ) );
    mt->getOrCreateStateSet()->setAttributeAndModes ( mode.get(), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    
    // Turn off lighting.
    OsgTools::State::StateSet::setLighting ( mt.get(), false );

    group->addChild ( mt.get() );

    return text.get();
  }

  return i->second.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set text value.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::setText( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4& color, const osg::Vec4f& backDropColor )
{
  osg::ref_ptr < osgText::Text > t ( this->getText ( x, y ) );
  t->setColor ( color );
  if ( 0 != backDropColor[3] )
  {
    t->setBackdropColor ( backDropColor );
    t->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
  }
  t->setText ( text );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Add a global light.  This will be placed in an absolute reference frame.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int SceneManager::globalLight ( osg::Light* light )
{
  // Get the number for this light.
  unsigned int num ( _lights.size() );

  // Set the number.
  light->setLightNum ( num );

  // Add to the end of the list.
  _lights.push_back ( light );

  // Make the light source.
  osg::ref_ptr < osg::LightSource > source ( new osg::LightSource );
  source->setReferenceFrame ( osg::LightSource::ABSOLUTE_RF );
  source->setLight( light );
  source->setLocalStateSetModes ( osg::StateAttribute::ON );

  // Add to the scene.
  _lightNode->addChild ( source.get() );

  // Enable the lights in the root's state set.
  osg::ref_ptr < osg::StateSet > ss ( this->scene()->getOrCreateStateSet() );
  source->setStateSetModes( *ss, osg::StateAttribute::ON );

  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get global light at index i.
//
///////////////////////////////////////////////////////////////////////////////

osg::Light* SceneManager::globalLight( unsigned int i )
{
  return _lights.at( i ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get global light at index i.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Light* SceneManager::globalLight( unsigned int i ) const
{
  return _lights.at( i ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove global light at index i.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::globalLightRemove ( unsigned int i )
{
  if( i < _lights.size() )
  {
    _lightNode->removeChild ( i );
    _lights.erase ( _lights.begin() + i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the global lights.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::globalLightClear ( )
{
  _lightNode->removeChild ( 0, _lightNode->getNumChildren () );
  _lights.clear ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void SceneManager::resize ( double x, double width, double y, double height )
{
  _cameraNode->setViewport ( static_cast<int> ( x ), static_cast<int> ( y ), static_cast<int> ( width ), static_cast<int> ( height ) );
  _cameraNode->setProjectionMatrixAsOrtho ( x, width, y, height, -1000.0, 1000.0 );
}
