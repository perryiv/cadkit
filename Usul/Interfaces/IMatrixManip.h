
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting/setting the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MATRIX_MANIP_H_
#define _USUL_INTERFACE_MATRIX_MANIP_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osgGA { class MatrixManipulator; }

namespace Usul {
namespace Interfaces {

struct IMatrixManip : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMatrixManip );

  /// Id for this interface.
  enum { IID = 2351855661u };

  enum ManipTypes
  {
    TRACKBALL,
    FLIGHT,
    DRIVE
  };

  virtual void                        setMatrixManip ( ManipTypes type ) = 0;
  virtual osgGA::MatrixManipulator *  getMatrixManip ( ManipTypes type ) = 0;


}; // class IViewMatrix


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MATRIX_MANIP_H_
