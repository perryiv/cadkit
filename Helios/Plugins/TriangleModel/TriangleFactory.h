
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TriangleFactory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_FACTORY_CLASS_H_
#define _TRIANGLE_MODEL_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


class TriangleFactory : public Usul::Base::Referenced,
                        public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( TriangleFactory );

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( TriangleFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TriangleFactory();

protected:

  /// Use reference counting.
  virtual ~TriangleFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif // _TRIANGLE_MODEL_FACTORY_CLASS_H_
