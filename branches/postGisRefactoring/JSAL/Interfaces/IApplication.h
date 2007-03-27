
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_APPLICATION_H_
#define _JSAL_INTERFACE_APPLICATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace JSAL {
namespace Interfaces {


struct IApplication : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IApplication );

  /// Id for this interface.
  enum { IID = 1311792055 };
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_APPLICATION_H_
