
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_SEQUENCE_MANAGER_H__
#define __USUL_INTERFACES_SEQUENCE_MANAGER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    class ISequenceManager : public Usul::Interfaces::IUnknown
    {
    public:
      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( ISequenceManager );

      enum { IID = 1100798040 };

      virtual Usul::Interfaces::IUnknown* getListOf ( unsigned int iid ) = 0;

    }; //class ISequenceManager
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_SEQUENCE_MANAGER_H__

