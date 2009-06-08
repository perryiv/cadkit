
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting image data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_IMAGE_DATA_H_
#define _USUL_INTERFACE_GET_IMAGE_DATA_H_

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
struct IGetImageData : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef ValueType_ ValueType;
  typedef std::vector<ValueType> Values;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetImageData );

  /// Id for this interface.
  enum { IID = iid_ };

  /// Get the image dimensions.
  virtual void getImageDimensions ( unsigned int &rows, unsigned int &columns, unsigned int &layers, unsigned int &channels ) const = 0;

  /// Get the image values.
  virtual void getImageValues ( Values & ) const = 0;
};


// Common typedefs.
typedef IGetImageData < 4227521170u, Usul::Types::Uint8   > IGetImageDataUint8;
typedef IGetImageData < 3914360671u, Usul::Types::Uint16  > IGetImageDataUint16;
typedef IGetImageData < 1967682583u, Usul::Types::Uint32  > IGetImageDataUint32;
typedef IGetImageData < 3227115360u, Usul::Types::Float32 > IGetImageDataFloat32;
typedef IGetImageData < 3894806559u, Usul::Types::Float64 > IGetImageDataFloat64;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GET_IMAGE_DATA_H_
