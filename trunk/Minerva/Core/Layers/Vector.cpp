
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/Vector.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Group.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "osg/Group"

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Vector, Vector::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Vector::Vector() : 
  BaseClass (),
  _layers(),
  _updateListeners(),
  _builders(),
  _name(),
  _guid( Usul::Functions::GUID::generate() ),
  _shown ( true ),
  _flags ( Vector::ALL ),
  _lowerLeft ( -180.0, -90.0 ),
  _upperRight ( 180.0, 90.0 ),
  _root ( new osg::Group ),
SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Vector::Vector( const Vector& rhs ) : 
  BaseClass ( rhs ),
  _layers( rhs._layers ),
  _updateListeners ( rhs._updateListeners ),
  _builders ( rhs._builders ),
  _name( rhs._name ),
  _guid( Usul::Functions::GUID::generate() ),
  _shown ( rhs._shown ),
  _flags ( rhs._flags | Vector::SCENE_DIRTY ), // Make sure scene gets rebuilt.
  _lowerLeft ( rhs._lowerLeft ),
  _upperRight ( rhs._upperRight ),
  _root ( new osg::Group ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Vector::~Vector()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::_registerMembers()
{
  USUL_TRACE_SCOPE;
  SERIALIZE_XML_ADD_MEMBER ( _name );
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _shown );
  this->_addMember ( "layers", _layers );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Vector::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene* > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  case Minerva::Interfaces::IElevationChangedListnerer::IID:
    return static_cast < Minerva::Interfaces::IElevationChangedListnerer* > ( this );
  default:
    return 0x0;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse all DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::traverse ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Unknowns::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    if ( Minerva::Core::Layers::Vector *vector = dynamic_cast< Minerva::Core::Layers::Vector*> ( (*iter).get() ) )
    {
      vector->accept ( visitor );
    }

    if ( Minerva::Core::DataObjects::DataObject *object = dynamic_cast< Minerva::Core::DataObjects::DataObject*> ( (*iter).get() ) )
    {
      object->accept ( visitor );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::name( const std::string& name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Vector::name() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string Vector::guid() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::showLayer( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _shown = b;
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::showLayer() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _shown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a data object.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::add ( Usul::Interfaces::IUnknown* unknown )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != unknown )
  {
    Guard guard ( this );
    _layers.push_back ( unknown );
  }

  // Add the update listener.
  _updateListeners.add ( unknown );

  // Add the builder.
  _builders.add ( unknown );

  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::remove ( Usul::Interfaces::IUnknown* unknown )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  Unknowns::iterator doomed ( std::find( _layers.begin(), _layers.end(), Unknowns::value_type ( unknown ) ) );
  if( doomed != _layers.end() )
    _layers.erase( doomed );

  // Remove the update listener.
  _updateListeners.remove ( unknown );

  // Remove the builder.
  _builders.remove ( unknown );
  
  this->dirtyScene( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::clear ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _layers.clear();
  _builders.clear();
  _updateListeners.clear();

  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of data objects in this layer.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Vector::number() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene (IBuildScene).
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Vector::buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::_buildScene( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard( this->mutex() );
  
  if ( true == _root.valid() || this->dirtyScene() )
  {
    // For debugging...
    _root->setName ( this->name() );
    
    // Remove all children.
    OsgTools::Group::removeAllChildren ( _root.get() );

    // Add to the scene if we are shown.
    if ( this->showLayer() )
    {
      Builders::Guard guard ( _builders.mutex() );
      for ( Builders::iterator iter = _builders.begin(); iter != _builders.end(); ++iter )
      {
        // Query for needed interfaces.
        ILayer::QueryPtr layer ( *iter );
        Usul::Interfaces::IBuildScene::QueryPtr build ( *iter );

        // Should the layer be shown?
        const bool show ( layer.valid() ? layer->showLayer() : true );

        // Build the scene.
        if ( show && build.valid() )
          _root->addChild ( build->buildScene ( Usul::Interfaces::IBuildScene::Options(), caller ) );
      }
    }
    
    // Our scene is no longer dirty.
    this->dirtyScene ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Vector::flags() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::flags( unsigned int f )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::dirtyData() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::DATA_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyData( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::DATA_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::dirtyExtents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::EXTENTS_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyExtents( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::EXTENTS_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::dirtyScene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::SCENE_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyScene( bool b, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::SCENE_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::extents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight )
{
  USUL_TRACE_SCOPE;
  if ( this->dirtyExtents() )
  {
    Usul::Math::Vec2d ll, ur;
    
    // Calculate new extents.
    this->_calculateExtents ( ll, ur );
    
    // Assign our data members.
    Guard guard ( this->mutex() );
    _lowerLeft = ll;
    _upperRight = ur;
    
    // Extents are no longer dirty.
    this->dirtyExtents( false );
  }
  
  Guard guard ( this->mutex() );
  lowerLeft = _lowerLeft;
  upperRight = _upperRight;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::_calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const
{
  USUL_TRACE_SCOPE;
  lowerLeft.set ( -180.0, -90.0 );
  upperRight.set ( 180.0, 90.0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  // No need to guard _builders or _updateListeners because they have thier own mutex.

  osg::ref_ptr<osg::Group> root ( Usul::Threads::Safe::get ( this->mutex(), _root ) );

  // Check to see if the number of children in the root is the same as the data objects.  This is a hack before dirtyScene isn't alway accurate.
  const bool needsBuild ( root.valid() ? root->getNumChildren() != _builders.size() : false );

  // Build if we need to...
  if ( this->dirtyScene() || needsBuild )
    this->_buildScene( caller );

  // Ask each one to update.
  _updateListeners.for_each ( std::bind2nd ( std::mem_fun ( &IUpdateListener::updateNotify ), caller ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Vector::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * Vector::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  return Usul::Interfaces::ITreeNode::QueryPtr ( _layers.at( which ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void Vector::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  this->name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string Vector::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

void Vector::setBooleanState ( bool b )
{
  USUL_TRACE_SCOPE;
  this->showLayer ( b );

  // Set the state of our children.
  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IBooleanState::QueryPtr state ( *iter );
    if ( state.valid() )
      state->setBooleanState ( b );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  return this->showLayer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _dataMemberMap.deserialize ( node );

  // Add layers.
  for ( Unknowns::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    // Add the update listener.
    _updateListeners.add ( *iter );

    // Add the builder.
    _builders.add ( *iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this as an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Vector::asUnknown()
{
  USUL_TRACE_SCOPE;
  return this->queryInterface( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Elevation has changed within given extents.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::elevationChangedNotify ( const Extents& extents, ImagePtr elevationData, IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  bool handled ( false );

  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Minerva::Interfaces::IElevationChangedListnerer::QueryPtr ecl ( *iter );
      if ( ecl.valid() )
        handled = ecl->elevationChangedNotify ( extents, elevationData, caller );
    }
  }

  if ( handled )
    this->dirtyScene ( true );

  return handled;
}
