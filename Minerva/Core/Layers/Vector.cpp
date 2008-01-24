
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

#include "Usul/Functions/GUID.h"

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
SERIALIZE_XML_INITIALIZER_LIST
{
  SERIALIZE_XML_ADD_MEMBER ( _name );
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _shown );
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
    (*iter)->accept ( visitor );
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
  
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  for( DataObjects::iterator iter = _dataObjects.begin(); iter != _dataObjects.end(); ++iter )
  {
    if ( (*iter).valid() )
      group->addChild( (*iter)->buildScene( caller ) );
  }
  return group.release();
}
