
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Core/Data/TimeSpan.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/File/Temp.h"
#include "Usul/Registry/Database.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

using namespace Minerva::Core::Data;

/////////////////////////////////////////////////////////////////////////////
//
//  Declare serialization wrappers.
//
/////////////////////////////////////////////////////////////////////////////

USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( Feature::Extents );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( Feature::Extents );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( Feature::Extents );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Feature, Feature::BaseClass );

/////////////////////////////////////////////////////////////////////////////
//
//  Registry sections.
//
/////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string RASTER_LAYER_CACHE_DIR ( "raster_layer_cache_dir" );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::Feature() : 
  BaseClass(),
  _description(),
  _name(),
	_styleUrl(),
  _visibility ( true ),
  _lookAt ( 0x0 ),
  _timePrimitive ( 0x0 ),
  _extents(),
  _dataChangedListeners()
{
  this->_addMember ( "name", _name );
  this->_addMember ( "visibility", _visibility );
  this->_addMember ( "extents", _extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::Feature ( const Feature& rhs ) : BaseClass ( rhs ),
  _description( rhs._description ),
  _name( rhs._name ),
  _styleUrl( rhs._styleUrl ),
  _visibility ( rhs._visibility ),
  _lookAt ( rhs._lookAt ),
  _timePrimitive ( rhs._timePrimitive ),
  _extents ( rhs._extents ),
  _dataChangedListeners ( rhs._dataChangedListeners )
{
  this->_addMember ( "name", _name );
  this->_addMember ( "visibility", _visibility );
  this->_addMember ( "extents", _extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Feature::~Feature()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Feature::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::IDataChangedNotify::IID:
    return static_cast < Usul::Interfaces::IDataChangedNotify* > ( this );
  case Usul::Interfaces::ILayerExtents::IID:
    return static_cast<Usul::Interfaces::ILayerExtents*> ( this );
  case Minerva::Interfaces::IFeature::IID:
    return static_cast<Minerva::Interfaces::IFeature*> ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Feature::name() const
{
  Guard guard ( this->mutex() );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::name ( const std::string& name )
{
  USUL_TRACE_SCOPE;

  // Set the name.
  this->_nameSet ( name );

  // Notify any listeners that the data has changed.
  this->_notifyDataChnagedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::_nameSet ( const std::string& name )
{
  USUL_TRACE_SCOPE;

  // Set the name.
  Usul::Threads::Safe::set ( this->mutex(), name, _name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the visiblity.
//
///////////////////////////////////////////////////////////////////////////////

bool Feature::visibility() const
{
  Guard guard ( this->mutex() );
  return _visibility;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visiblity.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::visibility ( bool b )
{
  Guard guard ( this->mutex() );
  _visibility = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the style url.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::styleUrl ( const std::string& url )
{
  Guard guard ( this->mutex() );
	_styleUrl = url;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style url.
//
///////////////////////////////////////////////////////////////////////////////

std::string Feature::styleUrl() const
{
  Guard guard ( this->mutex() );
	return _styleUrl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time primitive.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::timePrimitive ( TimePrimitive* timePrimitive )
{
  Guard guard ( this->mutex() );
  _timePrimitive = timePrimitive;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time primitive.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Data::TimePrimitive* Feature::timePrimitive() const
{
  Guard guard ( this->mutex() );
  return _timePrimitive.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the description.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::description ( const std::string& s )
{
  Guard guard ( this->mutex() );
  _description = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the description.
//
///////////////////////////////////////////////////////////////////////////////

std::string Feature::description() const
{
  Guard guard ( this->mutex() );
  return _description;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the look at.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::lookAt ( LookAt* lookAt )
{
  Guard guard ( this->mutex() );
  _lookAt = lookAt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the look at.
//
///////////////////////////////////////////////////////////////////////////////

LookAt* Feature::lookAt() const
{
  Guard guard ( this->mutex() );
  return _lookAt.get();
}


/////////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::addDataChangedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _dataChangedListeners.add ( caller );
}


/////////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::removeDataChangedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _dataChangedListeners.remove ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Notify data changed listeners.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::_notifyDataChnagedListeners()
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  _dataChangedListeners.for_each ( std::bind2nd ( std::mem_fun ( &IDataChangedListener::dataChangedNotify ), me.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::extents ( const Extents& e )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _extents = e;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Feature::Extents Feature::extents() const
{
  USUL_TRACE_SCOPE;
  return Usul::Threads::Safe::get ( this->mutex(), _extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Feature::minLon() const
{
  Guard guard ( this );
  return _extents.minLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Feature::minLat() const
{
  Guard guard ( this );
  return _extents.minLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Feature::maxLon() const
{
  Guard guard ( this );
  return _extents.maxLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Feature::maxLat() const
{
  Guard guard ( this );
  return _extents.maxLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::_updateExtents ( Usul::Interfaces::IUnknown* unknown )
{
  USUL_TRACE_SCOPE;
  
  Usul::Interfaces::ILayerExtents::QueryPtr le ( unknown );
  
  const double minLon ( le.valid() ? le->minLon() : -180.0 );
  const double minLat ( le.valid() ? le->minLat() :  -90.0 );
  const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
  const double maxLat ( le.valid() ? le->maxLat() :   90.0 );

  Guard guard ( this->mutex() );
  _extents.expand ( Extents ( minLon, minLat, maxLon, maxLat ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the feature.
//
///////////////////////////////////////////////////////////////////////////////

Feature* Feature::feature()
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

void Feature::defaultCacheDirectory ( const std::string& dir )
{
  Usul::Registry::Database::instance()[Detail::RASTER_LAYER_CACHE_DIR] = dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Feature::defaultCacheDirectory()
{
  return Usul::Registry::Database::instance()[Detail::RASTER_LAYER_CACHE_DIR].get ( Usul::File::Temp::directory ( false ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Feature::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * Feature::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void Feature::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  this->name( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string Feature::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this as an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Feature::asUnknown()
{
  USUL_TRACE_SCOPE;
  return this->queryInterface( Usul::Interfaces::IUnknown::IID );
}
