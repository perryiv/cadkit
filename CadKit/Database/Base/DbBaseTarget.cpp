
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseTarget: Base class for database classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbBasePrecompiled.h"
#include "DbBaseTarget.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlPathname.h"

#include "Interfaces/IErrorNotify.h"
#include "Interfaces/IWarningNotify.h"
#include "Interfaces/IProgressNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorted up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

using namespace CadKit;

SL_IMPLEMENT_CLASS ( DbBaseTarget, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseTarget::DbBaseTarget() : SlRefBase ( 0 ),
  _controller ( NULL )
{
  SL_PRINT2 ( "In DbBaseTarget::DbBaseTarget(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbBaseTarget::~DbBaseTarget()
{
  SL_PRINT2 ( "In DbBaseTarget::~DbBaseTarget(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbBaseTarget::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbBaseTarget::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IDataTarget::IID:
    return static_cast<IDataTarget *>(this);
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
//  Get the default output filename, based on the given filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbBaseTarget::getDefaultOutputName ( const std::string &filename )
{
  SL_PRINT3 ( "In DbBaseTarget::getDefaultOutputName(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Parse the path.
  SlPathname<std::string> path ( filename );

  // Make a copy of the input filename.
  std::string out ( filename );

  // Drop the extension.
  out.resize ( out.size() - path.getExtension().size() );

  // Add the given extension.
  out += this->getFileExtension();

  // Return the output filename.
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the controller.
//
///////////////////////////////////////////////////////////////////////////////

void DbBaseTarget::setController ( IUnknown *controller )
{
  SL_PRINT3 ( "In DbBaseTarget::setController(), this = %X, controller = %X\n", this, controller );

  // Set the controller, it may be null.
  _controller = controller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Error notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseTarget::_notifyError ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbBaseTarget::_notifyError(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IErrorNotify> controller ( IErrorNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->errorNotify ( message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Progress notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseTarget::_notifyProgress ( const std::string &message )
{
  SL_PRINT3 ( "In DbBaseTarget::_notifyProgress(), this = %X, message = \n", this, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IProgressNotify> controller ( IProgressNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->progressNotify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Warning notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbBaseTarget::_notifyWarning ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbBaseTarget::_notifyWarning(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the controller supports the proper interface.
  SlQueryPtr<IWarningNotify> controller ( IWarningNotify::IID, _controller );

  // If the interface is not implemented then return true to proceed.
  if ( controller.isNull() )
    return true;

  // Let the controller know.
  return controller->warningNotify ( message, id );
}
