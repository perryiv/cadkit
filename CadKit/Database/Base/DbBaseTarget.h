
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

#include "DbBaseApi.h"

#include "Interfaces/IDataTarget.h"
#include "Interfaces/IControlled.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlRefPtr.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class DB_BASE_API DbBaseTarget : public SlRefBase, 
                             public IDataTarget,
                             public IControlled
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( const unsigned long &iid ) = 0;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataTarget interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the default output filename, based on the given filename.
  virtual std::string     getDefaultOutputName ( const std::string &filename );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IControlled interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the controller.
  virtual void            setController ( IUnknown *controller );

protected:

  SlRefPtr<IUnknown> _controller;

  DbBaseTarget();
  virtual ~DbBaseTarget();

  bool                    _notifyError    ( const std::string &message, const unsigned long &id );
  bool                    _notifyProgress ( const std::string &message );
  bool                    _notifyWarning  ( const std::string &message, const unsigned long &id );

  SL_DECLARE_REFCOUNT_TYPE ( DbBaseTarget );
  SL_DECLARE_CLASS ( DbBaseTarget, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_TARGET_DATABASE_CLASS_H_
