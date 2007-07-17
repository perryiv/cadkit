
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_NODE_CLASS_H_
#define _USUL_REGISTRY_NODE_CLASS_H_

#include "Usul/Base/Object.h"

namespace Usul { namespace Registry { class Visitor; } }


namespace Usul {
namespace Registry {


class USUL_EXPORT Node : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Node );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  // Constructor.
  Node();

  // Accept the visitor.
  void                      accept ( Visitor & );

private:

  // Use reference counting.
  virtual ~Node();

private:

  // No copying or assignment.
  Node &operator = ( const Node & );
  Node ( const Node & );
};


}; // namespace Registry
}; // namespace Usul


#endif // _USUL_REGISTRY_VISITOR_CLASS_H_
