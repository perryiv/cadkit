
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IDIRTY_DATA_H__
#define __MINERVA_INTERFACES_IDIRTY_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IDirtyData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDirtyData );
  
  /// Id for this interface.
  enum { IID = 2267764831u };
  
  /// Get/Set dirty scene flag.
  virtual bool            dirtyData() const = 0;
  virtual void            dirtyData( bool b ) = 0;
  
}; // struct IDirtyData

  
} // end namespace Interfaces
} // end namespace Minerva

#endif // __MINERVA_INTERFACES_IDIRTY_DATA_H__
