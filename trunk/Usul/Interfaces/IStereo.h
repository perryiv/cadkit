
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting stereo and getting modes
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_STEREO_H_
#define _USUL_INTERFACE_STEREO_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IStereo : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IStereo );

  /// Id for this interface.
  enum { IID = 2362813894u };



}; // class IStereo


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_STEREO_H_
