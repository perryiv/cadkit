
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseSource: Base class for other source database classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_

#include "DbBaseObject.h"

#include "Interfaces/IDataSource.h"

#include "Standard/SlRefPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif


namespace CadKit
{
class DB_BASE_API DbBaseSource : public DbBaseObject, 
                                 public IDataSource
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( unsigned long iid ) = 0;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataSource interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the data target.
  virtual void            setDataTarget ( IUnknown *target );

protected:

  SlRefPtr<IUnknown> _target;

  DbBaseSource();
  virtual ~DbBaseSource();

  SL_DECLARE_REFERENCE_POINTER ( DbBaseSource );
  SL_DECLARE_CLASS ( DbBaseSource, 1033073996 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_
