
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_PLANET_NODE_H__
#define __USUL_INTERFACES_PLANET_NODE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Group; }

#include <string>

namespace Usul {
namespace Interfaces {


struct IPlanetNode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPlanetNode );

  /// Id for this interface.
  enum { IID = 3692497394u };

  virtual osg::Group* planetNode ( const std::string& key ) = 0;

}; // struct IPlanetNode


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_PLANET_NODE_H__ */

