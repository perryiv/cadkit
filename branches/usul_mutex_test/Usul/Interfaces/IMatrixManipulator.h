
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MATRIX_MANIPULATOR_H_
#define _USUL_INTERFACE_MATRIX_MANIPULATOR_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osgGA { class MatrixManipulator; }

namespace Usul {
namespace Interfaces {

struct IMatrixManipulator : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMatrixManipulator );

  /// Id for this interface.
  enum { IID = 1093526704u };

  virtual osgGA::MatrixManipulator *       getMatrixManipulator ( ) = 0;

}; // class IMatrixManipulator


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MATRIX_MANIPULATOR_H_
