
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ABOUTFACTORY_H__
#define __ABOUTFACTORY_H__

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


class AboutFactory : public Usul::Base::Referenced,
                                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( AboutFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  AboutFactory();

protected:

  /// Use reference counting.
  virtual ~AboutFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif /* __ABOUTFACTORY_H__  */		
	
