
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for dealing with rgba color.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SAL_INTERFACE_RGBA_COLOR_H_
#define _SAL_INTERFACE_RGBA_COLOR_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector4.h"


namespace SAL {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IColorRGBA : public Usul::Interfaces::IUnknown
{
  typedef Usul::Math::Vector4<RealType> Color;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IColorRGBA );

  /// Id for this interface.
  enum { IID = iid };

  // Set/get the color.
  virtual void            setColorRGBA ( const Color &rgba ) = 0;
  virtual void            getColorRGBA ( Color &rgba ) const = 0;
};


typedef IColorRGBA<2261504698u,float>  IColorFloatRGBA;
typedef IColorRGBA<3772487937u,double> IColorDoubleRGBA;


}; // namespace Interfaces
}; // namespace SAL


#endif // _SAL_INTERFACE_RGBA_COLOR_H_
