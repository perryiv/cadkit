
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
  _controller ( NULL )
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

IUnknown *DbBaseObject::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbBaseObject::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IControlled::IID:
    return static_cast<IControlled *>(this);
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Message notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseObject::_notifyMessage ( const std::string &message, const unsigned long &id, const IMessageNotify::Type &type )
{
  SL_PRINT5 ( "In DbBaseObject::_notifyMessage(), this = %X, id = %d, type = %d, message = \n", this, id, type, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IMessageNotify> controller ( IMessageNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->messageNotify ( message, id, type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The various notification functions.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseObject::_notifyError    ( const std::string &message, const unsigned long &id ) { return this->_notifyMessage ( message, id, IMessageNotify::MESSAGE_ERROR ); }
bool DbBaseObject::_notifyWarning  ( const std::string &message, const unsigned long &id ) { return this->_notifyMessage ( message, id, IMessageNotify::MESSAGE_WARNING ); }
bool DbBaseObject::_notifyProgress ( const std::string &message )                          { return this->_notifyMessage ( message, 0,  IMessageNotify::MESSAGE_PROGRESS ); }
