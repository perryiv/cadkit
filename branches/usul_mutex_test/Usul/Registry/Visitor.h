
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_VISITOR_CLASS_H_
#define _USUL_REGISTRY_VISITOR_CLASS_H_

#include "Usul/Base/Object.h"

namespace Usul { namespace Registry { class Node; class Database; } }


namespace Usul {
namespace Registry {


class USUL_EXPORT Visitor : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Visitor );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Visitor );

  // Visit the object.
  virtual void              visit ( Database * ) = 0;
  virtual void              visit ( Node * ) = 0;

protected:

  // Constructor.
  Visitor();

  // Use reference counting.
  virtual ~Visitor();

private:

  // No copying or assignment.
  Visitor &operator = ( const Visitor & );
  Visitor ( const Visitor & );
};


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_VISITOR_CLASS_H_
