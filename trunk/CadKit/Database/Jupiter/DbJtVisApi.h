
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

#ifndef _CADKIT_DATABASE_JUPITER_INTERNAL_VIS_API_H_
#define _CADKIT_DATABASE_JUPITER_INTERNAL_VIS_API_H_


namespace CadKit
{
class DbJtVisApi
{
public:

  DbJtVisApi();
  ~DbJtVisApi();

  // Get the customer id.
  static unsigned int     getCustomerId();

  // Initialize. Call this after you construct.
  bool                    init();

  // Set the customer id. If you don't assign a customer id then it will 
  // look for the environment variable "DMDTK_CUSTOMER_ID".
  static void             setCustomerId ( const unsigned int &customerId ) { _customerId = customerId; }

protected:

  static unsigned int _customerId;
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_INTERNAL_VIS_API_H_
