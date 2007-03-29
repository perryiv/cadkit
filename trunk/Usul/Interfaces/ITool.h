
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Interface for responding to a fox event
//
/////////////////////////////////////////////////////////////////////////////
  
#ifndef __USUL_INTERFACES_FOX_EVENT_H__
#define __USUL_INTERFACES_FOX_EVENT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces   {
    
struct ITool : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITool );

  enum { IID = 1100798022u };

  virtual bool execute ( Unknown* caller, bool left, bool middle, bool right, bool motion, float x, float y, float z ) = 0; 

}; //class ITool

} //namespace Interfaces
} //namespace Usul

#endif // __USUL_INTERFACES_FOX_EVENT_H__

