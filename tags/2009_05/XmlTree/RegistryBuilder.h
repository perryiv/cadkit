
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that traverses an XML tree and builds Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_REGISTRY_BUILDER_H_
#define _XML_TREE_REGISTRY_BUILDER_H_

#include "XmlTree/Export.h"

#include "Usul/Base/Object.h"

namespace Usul { namespace Registry { class Database; class Node; } }
namespace XmlTree { class Node; }


namespace XmlTree {


class XML_TREE_EXPORT RegistryBuilder : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( RegistryBuilder );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RegistryBuilder );

  // Construction and destruction.
  RegistryBuilder();

  // Build the registry.
  void                      build ( const XmlTree::Node *, Usul::Registry::Database & );
  void                      build ( const XmlTree::Node *, Usul::Registry::Node & );

protected:

  virtual ~RegistryBuilder();

private:

  // No copying or assignment.
  RegistryBuilder &operator = ( const RegistryBuilder & );
  RegistryBuilder ( const RegistryBuilder & );
};


} // namespace XmlTree


#endif // _XML_TREE_REGISTRY_BUILDER_H_
