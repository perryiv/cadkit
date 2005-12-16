
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_FACTORY_CLASS_H_
#define _WX_GUI_FACTORY_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IClassFactory.h"


class WxFactory : public Usul::Base::Referenced,
                  public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( WxFactory );

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( WxFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  WxFactory();

protected:

  /// Use reference counting.
  virtual ~WxFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif // _WX_GUI_FACTORY_CLASS_H_
