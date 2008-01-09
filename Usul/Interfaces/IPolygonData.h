
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IPOLYGON_DATA_H__
#define __USUL_INTERFACES_IPOLYGON_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace Usul {
namespace Interfaces {


struct IPolygonData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPolygonData );

  /// Id for this interface.
  enum { IID = 2101968926u };

  virtual osg::Node* buildPolygonData () = 0;

}; // struct IPolygonData


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IPOLYGON_DATA_H__ */
