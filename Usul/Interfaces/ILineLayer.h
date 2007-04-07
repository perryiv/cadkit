
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILINE_LAYER_H__
#define __USUL_INTERFACES_ILINE_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ILineLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILineLayer );

  /// Id for this interface.
  enum { IID = 1772311221u };

  virtual float lineWidth() const = 0;
  virtual void  lineWidth ( float value ) = 0;

}; // struct ILineLayer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ILINE_LAYER_H__ */

