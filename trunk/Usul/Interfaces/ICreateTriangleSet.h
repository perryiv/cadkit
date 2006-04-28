
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Create a triangle set
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICREATETRIANGLESET_H__
#define __USUL_INTERFACES_ICREATETRIANGLESET_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>

namespace Usul {
namespace Interfaces {

  struct ITriangleSet;

struct ICreateTriangleSet : public Usul::Interfaces::IUnknown
{
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICreateTriangleSet );

  /// Id for this interface.
  enum { IID = 1044687500u };

  virtual bool                canOpen   ( const std::string &ext ) const = 0;

  virtual Filters             filtersOpen()   const = 0;

  virtual ITriangleSet*       read ( const std::string &filename, IUnknown *caller = 0x0 ) = 0;

}; // struct ICreateTriangleSet


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICREATETRIANGLESET_H__ */
