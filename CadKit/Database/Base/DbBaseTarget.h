
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseTarget: Base class for other target database classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_TARGET_DATABASE_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_TARGET_DATABASE_CLASS_H_

#include "DbBaseObject.h"

#include "Interfaces/IDataTarget.h"

#include "Standard/SlRefPtr.h"

#include "Interfaces/IMessageNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class DB_BASE_API DbBaseTarget : public DbBaseObject, 
                                 public IDataTarget
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( unsigned long iid ) = 0;

protected:

  DbBaseTarget();
  virtual ~DbBaseTarget();

  SL_DECLARE_REFERENCE_POINTER ( DbBaseTarget );
  SL_DECLARE_CLASS ( DbBaseTarget, 1033073318 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_TARGET_DATABASE_CLASS_H_
