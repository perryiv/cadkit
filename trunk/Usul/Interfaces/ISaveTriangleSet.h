
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface to save a triangle set.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISAVETRIANGLESET_H__
#define __USUL_INTERFACES_ISAVETRIANGLESET_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>

namespace Usul {
namespace Interfaces {

  struct ITriangleSet;

struct ISaveTriangleSet : public Usul::Interfaces::IUnknown
{
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISaveTriangleSet );

  /// Id for this interface.
  enum { IID = 1930937500u };

  virtual bool                canSave   ( const std::string &ext ) const = 0;

  virtual Filters             filtersSave()   const = 0;

  void                        save ( ITriangleSet*, const std::string& filename, Unknown *caller = 0x0 );

}; // struct ISaveTriangleSet


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISAVETRIANGLESET_H__ */
