
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseObject: Base class for database classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbBasePrecompiled.h"
#include "DbBaseObject.h"
#include "DbBaseInline.h"

#include "Standard/SlPrint.h"
#include "Standard/SlQueryPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

using namespace CadKit;

SL_IMPLEMENT_CLASS ( DbBaseObject, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseObject::DbBaseObject() : SlRefBase ( 0 ),
  _genericClientDataMap  ( new GenericClientDataMap ),
  _assemblyClientDataMap ( new AssemblyClientDataMap ),
  _partClientDataMap     ( new PartClientDataMap ),
  _instanceClientDataMap ( new InstanceClientDataMap ),
  _groupClientDataMap    ( new GroupClientDataMap ),
  _lodClientDataMap      ( new LodClientDataMap ),
  _shapeClientDataMap    ( new ShapeClientDataMap ),
  _controller ( NULL ),
  _messageNotify ( NULL ),
  _truncate ( 0.0f, 0.0f, 0.0f )
{
  SL_PRINT2 ( "In DbBaseObject::DbBaseObject(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseObject::~DbBaseObject()
{
  SL_PRINT2 ( "In DbBaseObject::~DbBaseObject(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbBaseObject::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbBaseObject::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IControlled::IID:
    return static_cast<IControlled *>(this);
  case IGenericClientData::IID:
    return static_cast<IGenericClientData *>(this);
  case IAssemblyClientData::IID:
    return static_cast<IAssemblyClientData *>(this);
  case IPartClientData::IID:
    return static_cast<IPartClientData *>(this);
  case IInstanceClientData::IID:
    return static_cast<IInstanceClientData *>(this);
  case IGroupClientData::IID:
    return static_cast<IGroupClientData *>(this);
  case ILodClientData::IID:
    return static_cast<ILodClientData *>(this);
  case IShapeClientData::IID:
    return static_cast<IShapeClientData *>(this);
  case IZeroRangeFloat::IID:
    return static_cast<IZeroRangeFloat *>(this);
  case IFormatAttribute::IID:
    return static_cast<IFormatAttribute *>(this);
  case CadKit::IUnknown::IID:
    return static_cast<CadKit::IUnknown *>(static_cast<IControlled *>(this));
  default:
    return NULL;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the controller.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseObject::setController ( IUnknown *controller )
{
  SL_PRINT3 ( "In DbBaseObject::setController(), this = %X, controller = %X\n", this, controller );

  // Set the controller, it may be null.
  _controller = controller;

  // Assign the specific interface pointers. This will query for the 
  // respective interfaces. If the resulting interface pointer is invalid, it
  // simply means the interface was not available.
  _messageNotify = controller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Message notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseObject::_notifyMessage ( const std::string &message, const unsigned long &id, const MessageType &type )
{
  SL_PRINT5 ( "In DbBaseObject::_notifyMessage(), this = %X, id = %d, type = %d, message = \n", this, id, type, message.c_str() );

  // If the interface is not implemented then return true to proceed.
  if ( _messageNotify.isNull() )
    return true;

  // Let the controller know.
  return _messageNotify->messageNotify ( message, id, type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The various notification functions.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseObject::_notifyError    ( const std::string &message, const unsigned long &id )       { return this->_notifyMessage ( message, id,       CadKit::MESSAGE_ERROR ); }
bool DbBaseObject::_notifyWarning  ( const std::string &message, const unsigned long &id )       { return this->_notifyMessage ( message, id,       CadKit::MESSAGE_WARNING ); }
bool DbBaseObject::_notifyProgress ( const std::string &message, const unsigned long &priority ) { return this->_notifyMessage ( message, priority, CadKit::MESSAGE_PROGRESS ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the client data.
//
///////////////////////////////////////////////////////////////////////////////

const void *DbBaseObject::getClientData ( void * const key )         const { return Utility::find ( key, *(_genericClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const AssemblyHandle key ) const { return Utility::find ( key, *(_assemblyClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const PartHandle key )     const { return Utility::find ( key, *(_partClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const InstanceHandle key ) const { return Utility::find ( key, *(_instanceClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const GroupHandle key )    const { return Utility::find ( key, *(_groupClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const LodHandle key )      const { return Utility::find ( key, *(_lodClientDataMap.get()) ); }
const void *DbBaseObject::getClientData ( const ShapeHandle key )    const { return Utility::find ( key, *(_shapeClientDataMap.get()) ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Set the client data.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseObject::setClientData ( void * const key,         const void *data ) { (*_genericClientDataMap)[key]  = data; }
void DbBaseObject::setClientData ( const AssemblyHandle key, const void *data ) { (*_assemblyClientDataMap)[key] = data; }
void DbBaseObject::setClientData ( const PartHandle key,     const void *data ) { (*_partClientDataMap)[key]     = data; }
void DbBaseObject::setClientData ( const InstanceHandle key, const void *data ) { (*_instanceClientDataMap)[key] = data; }
void DbBaseObject::setClientData ( const GroupHandle key,    const void *data ) { (*_groupClientDataMap)[key]    = data; }
void DbBaseObject::setClientData ( const LodHandle key,      const void *data ) { (*_lodClientDataMap)[key]      = data; }
void DbBaseObject::setClientData ( const ShapeHandle key,    const void *data ) { (*_shapeClientDataMap)[key]    = data; }


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the client data maps.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseObject::_clearClientDataMaps()
{
  SL_PRINT2 ( "In DbBaseObject::_clearClientDataMaps(), this = %X\n", this );

  _genericClientDataMap->clear();
  _assemblyClientDataMap->clear();
  _partClientDataMap->clear();
  _instanceClientDataMap->clear();
  _groupClientDataMap->clear();
  _lodClientDataMap->clear();
  _shapeClientDataMap->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the range that gets truncated to zero.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseObject::getZeroRange ( float &negative, float &positive ) const
{
  SL_PRINT2 ( "In DbBaseObject::getZeroRange(), this = %X\n", this );

  // Get the range.
  negative = _truncate.getLow();
  positive = _truncate.getHigh();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the range that gets truncated to zero.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseObject::setZeroRange ( const float &negative, const float &positive )
{
  SL_PRINT4 ( "In DbBaseObject::setZeroRange(), this = %X, negative = %0.15f, positive = 0.15f\n", this, negative, positive );
  SL_ASSERT ( negative <= 0.0f && 0.0f <= positive );

  // Get the range.
  _truncate.setLow   ( negative );
  _truncate.setValue ( 0.0f );
  _truncate.setHigh  ( positive );
}
