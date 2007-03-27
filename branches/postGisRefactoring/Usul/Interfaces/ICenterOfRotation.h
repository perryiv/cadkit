
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICENTEROFROTATION_H__
#define __USUL_INTERFACES_ICENTEROFROTATION_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICenterOfRotation : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICenterOfRotation );

  /// Id for this interface.
  enum { IID = 10212668u };

  virtual void showCenterOfRotation ( bool b ) = 0;
  virtual bool showCenterOfRotation ( ) const = 0;

}; // struct ICenterOfRotation


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICENTEROFROTATION_H__ */
