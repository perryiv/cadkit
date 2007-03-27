
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DATABASE_PAGER_H_
#define _USUL_INTERFACE_DATABASE_PAGER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osgDB { class DatabasePager; }

namespace Usul {
namespace Interfaces {

struct IDatabasePager : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDatabasePager );

  /// Id for this interface.
  enum { IID = 207745454u };

  virtual osgDB::DatabasePager *       getDatabasePager ( ) = 0;

}; // class IDatabasePager


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DATABASE_PAGER_H_
