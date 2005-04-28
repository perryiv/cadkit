
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Delete a triangle
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DELETE_TRIANGLE_H_
#define _USUL_INTERFACE_DELETE_TRIANGLE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace OsgTools { namespace Triangles { class Loop; }  }

namespace Usul {
namespace Interfaces {


struct IDeleteTriangle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDeleteTriangle );

  /// Id for this interface.
  enum { IID = 1744889731u };

  virtual void deleteTriangle ( unsigned int num ) = 0;

}; // struct IDeleteTriangle


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_DELETE_TRIANGLE_H_
