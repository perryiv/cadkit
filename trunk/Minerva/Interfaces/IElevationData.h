
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to store elevation data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IELEVATION_DATA_H__
#define __MINERVA_INTERFACES_IELEVATION_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IElevationData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IElevationData );

  /// Id for this interface.
  enum { IID = 4235048219u };

  typedef float ValueType;
  typedef unsigned int SizeType;

  /// Set/get the value.
  virtual void       value ( SizeType row, SizeType column, ValueType value ) = 0;
  virtual ValueType  value ( SizeType row, SizeType column ) const = 0;

  /// Get the value that corresponds to no data.
  virtual ValueType  noDataValue() const = 0;

  /// Get height and width.
  virtual SizeType   height() const = 0;
  virtual SizeType   width() const = 0;

}; // struct IElevationData


} // End namespace Interfaces
} // End namespace Minerva

#endif // __MINERVA_INTERFACES_IELEVATION_DATA_H__
