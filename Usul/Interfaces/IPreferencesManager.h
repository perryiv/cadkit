
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_PREFERENCES_MANAGER_H__
#define __USUL_INTERFACES_PREFERENCES_MANAGER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    class IPreferencesManager : public Usul::Interfaces::IUnknown
    {
    public:
      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IPreferencesManager );

      enum { IID = 1102358547u };

      virtual Usul::Interfaces::IUnknown* getPreferences ( ) = 0;

    }; //class IPreferenceManager
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_PREFERENCES_MANAGER_H__

