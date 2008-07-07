
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_COLOR_EDITOR_H__
#define __USUL_INTERFACES_COLOR_EDITOR_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector4.h"

namespace Usul {
namespace Interfaces {


struct IColorEditor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IColorEditor );

  /// Id for this interface.
  enum { IID = 1322309556u };

  /// Get the color.
  virtual bool                       editColor ( Usul::Math::Vec4f& color ) = 0;

}; // struct IColorEditor


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_COLOR_EDITOR_H__
