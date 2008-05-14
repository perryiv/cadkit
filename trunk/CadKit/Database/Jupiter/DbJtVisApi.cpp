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
#include "DbJtVisApiHeaders.h"
#include "DbJtVisApiVersion.h"

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
  unsigned int customer ( this->getCustomerId() );

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
#ifdef _CADKIT_USE_JTOPEN
#if defined _WIN32 && _MSC_VER >= 1400
  char *temp;
  size_t sz;
  if( _dupenv_s( &temp, &sz, "JTTK_CUSTOMER_ID" ) )
    return 0;
#else
  const char *temp = ::getenv ( "JTTK_CUSTOMER_ID" );
#endif
#else // _CADKIT_USE_JTOPEN
  const char *temp = ::getenv ( "DMDTK_CUSTOMER_ID" );
#endif // _CADKIT_USE_JTOPEN

  if ( temp ) 
    _customerId = ::atoi ( temp );

#if defined _WIN32 && _MSC_VER >= 1400
  free( temp );
#endif

  // Return what we have.
  return _customerId;
}

