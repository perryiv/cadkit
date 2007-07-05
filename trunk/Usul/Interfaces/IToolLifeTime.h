
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Interface for defining life time for a tool. 
//
/////////////////////////////////////////////////////////////////////////////
  
#ifndef __USUL_INTERFACES_TOOL_LIFE_TIME_H__
#define __USUL_INTERFACES_TOOL_LIFE_TIME_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces   {
    
struct IToolLifeTime : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IToolLifeTime );

  enum { IID = 3504968652u };

  virtual bool temporary()   = 0;

}; //class IToolLifeTime

} //namespace Interfaces
} //namespace Usul

#endif // __USUL_INTERFACES_TOOL_LIFE_TIME_H__

