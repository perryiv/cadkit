
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseObject: Base class for other target database classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_

#include "DbBaseApi.h"

#include "Interfaces/IControlled.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlRefPtr.h"

#include "Interfaces/IMessageNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class DB_BASE_API DbBaseObject : public SlRefBase, 
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
  //  IControlled interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the controller.
  virtual void            setController ( IUnknown *controller );

protected:

  SlRefPtr<IUnknown> _controller;

  DbBaseObject();
  virtual ~DbBaseObject();

  bool                    _notifyError    ( const std::string &message, const unsigned long &id );
  bool                    _notifyMessage  ( const std::string &message, const unsigned long &id, const IMessageNotify::Type &type );
  bool                    _notifyProgress ( const std::string &message );
  bool                    _notifyWarning  ( const std::string &message, const unsigned long &id );

  SL_DECLARE_REFERENCE_POINTER ( DbBaseObject );
  SL_DECLARE_CLASS ( DbBaseObject, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_
