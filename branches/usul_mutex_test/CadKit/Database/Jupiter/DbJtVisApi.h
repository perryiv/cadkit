
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
  DbJtVisApi ( const DbJtVisApi &visApi );
  ~DbJtVisApi();

  // Get the customer id.
  unsigned int            getCustomerId();

  // Initialize. Call this after you construct.
  bool                    init();

  // Set the customer id. If you don't assign a customer id then it will 
  // look for the environment variable "DMDTK_CUSTOMER_ID".
  void                    setCustomerId ( const unsigned int &customerId ) { _customerId = customerId; }

protected:

  unsigned int _customerId;
  bool _isInitialized;
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_JUPITER_INTERNAL_VIS_API_H_
