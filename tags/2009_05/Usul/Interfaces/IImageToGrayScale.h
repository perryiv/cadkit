
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for converting to gray scale.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_IMAGE_TO_GRAY_SCALE_H_
#define _USUL_INTERFACE_IMAGE_TO_GRAY_SCALE_H_

#include "Usul/Types/Types.h"
#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace Usul {
namespace Interfaces {


template
<
  unsigned int iid_, 
  class ValueType_
>
struct IImageToGrayScale : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef ValueType_ ValueType;
  typedef std::vector<ValueType> Values;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IImageToGrayScale );

  /// Id for this interface.
  enum { IID = iid_ };

  /// Convert image to gray scale.
  virtual void toGrayScale ( unsigned int channels, Values &values ) const = 0;
};


// Common typedefs.
typedef IImageToGrayScale < 4100451872, Usul::Types::Uint8   > IImageToGrayScaleUint8;
typedef IImageToGrayScale < 4100451873, Usul::Types::Uint16  > IImageToGrayScaleUint16;
typedef IImageToGrayScale < 4100451874, Usul::Types::Uint32  > IImageToGrayScaleUint32;
typedef IImageToGrayScale < 4100451875, Usul::Types::Float32 > IImageToGrayScaleFloat32;
typedef IImageToGrayScale < 4100451876, Usul::Types::Float64 > IImageToGrayScaleFloat64;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_IMAGE_TO_GRAY_SCALE_H_
