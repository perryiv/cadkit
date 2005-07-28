
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGLE_SV_H__
#define __USUL_INTERFACES_TRIANGLE_SV_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class SharedVertex; } }

namespace Usul {
namespace Interfaces {


struct ITriangleSV : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangleSV );

  /// Id for this interface.
  enum { IID = 4260819650u };

  typedef OsgTools::Triangles::SharedVertex SharedVertex;

  // Get the shared vertices of the i'th triangle.
  virtual const SharedVertex*         sharedVertex0 ( unsigned int i ) const = 0;
  virtual const SharedVertex*         sharedVertex1 ( unsigned int i ) const = 0;
  virtual const SharedVertex*         sharedVertex2 ( unsigned int i ) const = 0;


}; //struct ITriangleSV


}
}

#endif // __USUL_INTERFACES_TRIANGLE_SV_H__
