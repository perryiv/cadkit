
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_SEQUENCE_H__
#define __USUL_INTERFACES_SEQUENCE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    class ISequence : public Usul::Interfaces::IUnknown
    {
    public:
      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( ISequence );

      enum { IID = 1100798064 };

      virtual void appendUnknown ( Usul::Interfaces::IUnknown * ) = 0;
      virtual void removeUnknown ( Usul::Interfaces::IUnknown * ) = 0;
      virtual bool containsUnknown ( Usul::Interfaces::IUnknown * ) = 0;
      virtual void clearAllUnknowns ( ) = 0;
      virtual IUnknown * getUnknownAt ( unsigned int index ) = 0;

    }; //class ISequence
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_SEQUENCE_H__

