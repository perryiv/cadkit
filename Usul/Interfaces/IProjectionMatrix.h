
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_PROJECTION_MATRIX_H_
#define _USUL_INTERFACE_PROJECTION_MATRIX_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IProjectionMatrix : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProjectionMatrix );

  /// Id for this interface.
  enum { IID = 2313773568u };

  /// Set the projection matrix.
  virtual void projectionMatrix ( double left, double right, double bottom, double top, double zNear, double zFar ) = 0;

  /// Get the projection settings.
  virtual bool projectionMatrix ( double &fovy, double &aspect, double &zNear, double &zFar ) = 0;

  /// Add others as needed...
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_PROJECTION_MATRIX_H_
