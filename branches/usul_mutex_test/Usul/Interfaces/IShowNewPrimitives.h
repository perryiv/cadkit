
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

#ifndef __USUL_INTERFACES_ISHOWNEWPRIMITIVES_H__
#define __USUL_INTERFACES_ISHOWNEWPRIMITIVES_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace Usul {
namespace Interfaces {


struct IShowNewPrimitives : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IShowNewPrimitives );

  /// Id for this interface.
  enum { IID = 1136583726u };

  /// Get a node showing new primitives.
  virtual osg::Node*   getNewPrimitives ( ) = 0;
}; // struct IShowNewPrimitives


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISHOWNEWPRIMITIVES_H__ */
