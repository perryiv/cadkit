
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IBACKGROUND_COLOR_H__
#define __USUL_INTERFACES_IBACKGROUND_COLOR_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector4.h"

namespace Usul {
namespace Interfaces {


struct IBackgroundColor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBackgroundColor );

  /// Id for this interface.
  enum { IID = 3224676530u };

  /// Get/Set the background color.
  virtual void                       backgroundColor ( const Usul::Math::Vec4f& color ) = 0;
  virtual Usul::Math::Vec4f          backgroundColor () const = 0;

}; // struct IBackgroundColor


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IBACKGROUND_COLOR_H__
