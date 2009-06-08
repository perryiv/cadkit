
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VIEW_MATRIX_H_
#define _USUL_INTERFACE_VIEW_MATRIX_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Matrixf; class Matrixd; }

namespace Usul {
namespace Interfaces {

struct IViewMatrix : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewMatrix );

  /// Id for this interface.
  enum { IID = 1479567401u };

  virtual void                 setViewMatrix ( const osg::Matrixd & ) = 0;
  virtual osg::Matrixd         getViewMatrix (  ) const = 0;


}; // class IViewMatrix


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_VIEW_MATRIX_H_
