
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISERIALIZE_H__
#define __USUL_INTERFACES_ISERIALIZE_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace XmlTree { class Node; }

namespace Usul {
namespace Interfaces {


struct ISerialize : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISerialize );

  /// Id for this interface.
  enum { IID = 2942725826u };

  virtual void           serialize ( XmlTree::Node& node ) const = 0;
  virtual void           deserialize ( const XmlTree::Node& node ) = 0;
  virtual const char*    className () const = 0;

}; // struct ISerialize


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDATABASE_CONNECTION_H__ */

