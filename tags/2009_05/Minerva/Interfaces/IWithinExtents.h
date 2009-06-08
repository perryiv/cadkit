
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For getting the child items within the given extents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_INTERFACES_ITEMS_WITHIN_EXTENTS_H_
#define _MINERVA_INTERFACES_ITEMS_WITHIN_EXTENTS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IWithinExtents : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWithinExtents );

  /// Id for this interface.
  enum { IID = 2019998408u };

  /// Get the items within the extents. The returned pointer should be a container of some sort.
  virtual Usul::Interfaces::IUnknown::RefPtr    getItemsWithinExtents ( double minLon, double minLat, double maxLon, double maxLat, Usul::Interfaces::IUnknown::RefPtr caller = Usul::Interfaces::IUnknown::RefPtr ( 0x0 ) ) const = 0;
};


} // end namespace Interfaces
} // end namespace Minerva


#endif // _MINERVA_INTERFACES_ITEMS_WITHIN_EXTENTS_H_ 
