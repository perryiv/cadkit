
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for requesting a read operation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_REQUEST_MODEL_READ_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_REQUEST_MODEL_READ_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

#include <string>


namespace VRV {
namespace Interfaces {


struct IRequestRead : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRequestRead );

  // Id for this interface.
  enum { IID = 4077516636u };

  // Request the component to read the model from the named source and 
  // position it using the matrix.
  virtual void requestRead ( const std::string &source, const Matrix &m ) = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_REQUEST_MODEL_READ_H_
