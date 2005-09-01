
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the shading model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SHADING_MODEL_H_
#define _USUL_INTERFACE_SHADING_MODEL_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IShadeModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IShadeModel );

  /// Id for this interface.
  enum { IID = 3260613509u };

  enum Mode 
  {
    FLAT,
    SMOOTH
  };

  virtual bool               shadingFlat() const = 0;
  virtual bool               shadingSmooth() const = 0;
  virtual void               removeShadeModel() = 0;
  virtual void               toggleShadeModel ( Mode m ) = 0;
  virtual bool               hasShadeModel ( Mode m ) const = 0;
  virtual bool               hasShadeModel() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SHADING_MODEL_H_
