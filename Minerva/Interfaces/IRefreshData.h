
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IREFRESH_DATA_H__
#define __MINERVA_INTERFACES_IREFRESH_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IRefreshData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRefreshData );

  /// Id for this interface.
  enum { IID = 2267764831u };

  /// Force refresh of the data.
  virtual void            refreshData() = 0;

}; // struct IRefreshData

  
} // end namespace Interfaces
} // end namespace Minerva

#endif // __MINERVA_INTERFACES_IREFRESH_DATA_H__
