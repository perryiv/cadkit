
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

#include "Usul/Bits/Bits.h"
#include "Usul/Functions/GUID.h"

#include "OsgTools/Group.h"

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
  _dataObjects(),
  _name(),
  _guid( Usul::Functions::GUID::generate() ),
  _shown ( true ),
  _flags ( Vector::ALL ),
  _lowerLeft ( -180.0, -90.0 ),
  _upperRight ( 180.0, 90.0 ),
  _root ( new osg::Group ),
SERIALIZE_XML_INITIALIZER_LIST
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Vector::Vector( const Vector& rhs ) : 
  BaseClass ( rhs ),
  _dataObjects( rhs._dataObjects ),
  _name( rhs._name ),
  _guid( Usul::Functions::GUID::generate() ),
  _shown ( rhs._shown ),
  _flags ( rhs._flags | Vector::SCENE_DIRTY ), // Make sure scene gets rebuilt.
  _lowerLeft ( rhs._lowerLeft ),
  _upperRight ( rhs._upperRight ),
  _root ( new osg::Group ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Vector::~Vector()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _name );
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _shown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Vector::queryInterface ( unsigned long iid )
{
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
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse all DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::traverse ( Minerva::Core::Visitor& visitor )
{
  Guard guard ( this );
  for ( DataObjects::iterator iter = _dataObjects.begin(); iter != _dataObjects.end(); ++iter )
  {
    DataObjectPtr dataObject ( *iter );

    if ( dataObject.valid() )
      dataObject->accept ( visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::name( const std::string& name )
{
  Guard guard( this->mutex() );
  _name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Vector::name() const
{
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
  Guard guard ( this->mutex() );
  _shown = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::showLayer() const
{
  Guard guard ( this->mutex() );
  return _shown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a data object.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::addDataObject ( DataObject *dataObject )
{
  Guard guard ( this->mutex() );

  if ( 0x0 != dataObject )
    _dataObjects.push_back ( dataObject );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::clearDataObjects ()
{
  Guard guard ( this->mutex() );
  _dataObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of data objects in this layer.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Vector::number() const
{
  Guard guard ( this->mutex() );
  return _dataObjects.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene (IBuildScene).
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Vector::buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller )
{
  Guard guard( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::_buildScene( Usul::Interfaces::IUnknown *caller )
{
  Guard guard( this->mutex() );
  
  if ( true == _root.valid() || this->dirtyScene() )
  {
    OsgTools::Group::removeAllChildren ( _root.get() );

    for( DataObjects::iterator iter = _dataObjects.begin(); iter != _dataObjects.end(); ++iter )
    {
      if ( (*iter).valid() )
        _root->addChild( (*iter)->buildScene( caller ) );
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
  Guard guard( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::DATA_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyData( bool b )
{
  Guard guard( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::DATA_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::dirtyExtents() const
{
  Guard guard( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::EXTENTS_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyExtents( bool b )
{
  Guard guard( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::EXTENTS_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Vector::dirtyScene() const
{
  Guard guard( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Vector::SCENE_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::dirtyScene( bool b, Usul::Interfaces::IUnknown* caller )
{
  Guard guard( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Vector::SCENE_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Vector::extents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight )
{
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
  if ( this->dirtyScene() )
    this->_buildScene( caller );
}
