

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_MODE_STACK_H__
#define __USUL_INTERFACES_MODE_STACK_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    template < class ModeType, unsigned int iid >
    class IModeStack : public Usul::Interfaces::IUnknown
    {
    public:
      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IModeStack );

      enum { IID = iid };

      virtual void pushMode( ModeType ) = 0;
      virtual void popMode() = 0;

    }; //class IModeStack

    typedef IModeStack< unsigned int, 1100797533u > IModeStackUint;

  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_MODE_STACK_H__

