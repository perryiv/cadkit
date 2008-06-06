
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICFDCommands_H__
#define __USUL_INTERFACES_ICFDCommands_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct ICFDCommands : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICFDCommands );

  /// Id for this interface.
  enum { IID = 2392408715u };

  // go to the next group in the set
  virtual void              animation ( bool state ) = 0;
  virtual bool              animation ()  = 0;

  
}; // struct ICFDCOmmands



}
}

#endif // __USUL_INTERFACES_ICFDCommands_H__
