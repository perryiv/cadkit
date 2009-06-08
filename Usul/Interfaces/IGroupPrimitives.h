
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group all primitives that are connected together.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GROUP_PRIMITIVES_H_
#define _USUL_INTERFACE_GROUP_PRIMITIVES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IGroupPrimitives : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGroupPrimitives );

  /// Id for this interface.
  enum { IID = 1313723241u };

  // Group the primitives.
  virtual void groupPrimitives ( Usul::Interfaces::IUnknown *caller ) = 0;

  // Get the number of groups.
  virtual unsigned int groupsNumber () const = 0;

  // Get the primitive group
  virtual IUnknown*    getPrimitiveGroup ( unsigned int i ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GROUP_PRIMITIVES_H_
