
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtVisApi: For initializing (and uninitializing) the DMDTk.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtVisApi.h"

#include <stdlib.h> // For "NULL", used in eaiStandard.h, 
                    // indirectly included by eaiEntityFactory.h

#include "DMDataTk/eaiEntityFactory.h"

#include "Standard/SlAssert.h"


using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtVisApi::DbJtVisApi() :
  _customerId ( 0 ),
  _isInitialized ( false )
{
#if ( DMDTK_MAJOR_VERSION >= 5 ) // Defined in eaiStandard.h
  eaiEntityFactory::init();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor. Probably not needed.
//
///////////////////////////////////////////////////////////////////////////////

DbJtVisApi::DbJtVisApi ( const DbJtVisApi &visApi ) :
  _customerId ( visApi._customerId ),
  _isInitialized ( visApi._isInitialized )
{
  // Note: Do not call eaiEntityFactory::init().
  SL_ASSERT ( 0 ); // If you need to be here then take this assertion out.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtVisApi::~DbJtVisApi()
{
#if ( DMDTK_MAJOR_VERSION >= 5 ) // Defined in eaiStandard.h
  eaiEntityFactory::fini();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtVisApi::init()
{
  // Only do this once.
  if ( _isInitialized )
    return true;

  // Get the customer number.
  const unsigned int &customer = this->getCustomerId();

  // Register the customer.
  _isInitialized = ( eai_OK == eaiEntityFactory::registerCustomer ( customer ) );

  // Did it work?
  return _isInitialized;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the customer number.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DbJtVisApi::getCustomerId()
{
  // Was it set by the client?
  if ( 0 != _customerId )
    return _customerId;

  // Try to get it from the environment.
  const char *temp = ::getenv ( "DMDTK_CUSTOMER_ID" );
  if ( temp ) 
    _customerId = ::atoi ( temp );

  // Return what we have.
  return _customerId;
}
