
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_HANDLE_SEEK_H_
#define _USUL_INTERFACES_HANDLE_SEEK_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

namespace Usul {
namespace Interfaces {


struct IHandleSeek : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IHandleSeek );

  /// Id for this interface.
  enum { IID = 1024825806u };

  virtual void handleSeek ( const Usul::Math::Vec3d& intersectionPoint ) = 0;

}; // class IHandleSeek


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_HANDLE_SEEK_H_
