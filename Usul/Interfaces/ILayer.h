
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILAYER_H__
#define __USUL_INTERFACES_ILAYER_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct ILayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayer );

  /// Id for this interface.
  enum { IID = 1743049682u };

  /// Get the guid.
  virtual std::string                 guid() const = 0;

  /// Get/Set the name.
  virtual std::string                 name() const = 0;
  virtual void                        name( const std::string& ) = 0;

  /// Get/Set show layer
  virtual void                        showLayer ( bool b ) = 0;
  virtual bool                        showLayer() const = 0;

  /// See if the given level falls within this layer's range of levels.
  virtual bool                        isInLevelRange ( unsigned int level ) const = 0;
}; // struct ILayer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ILAYER_H__ */

