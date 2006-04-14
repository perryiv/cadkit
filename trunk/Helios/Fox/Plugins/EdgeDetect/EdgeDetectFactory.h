
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EDGE_DETECT_FACTORY_CLASS_H_
#define _EDGE_DETECT_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"



class EdgeDetectFactory : public Usul::Base::Referenced,
                          public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( EdgeDetectFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  EdgeDetectFactory();

protected:

  /// Use reference counting.
  virtual ~EdgeDetectFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};




#endif // _EDGE_DETECT_FACTORY_CLASS_H_
