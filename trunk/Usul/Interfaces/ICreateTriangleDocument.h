
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICREATETRIANGLEDOCUMENT_H__
#define __USUL_INTERFACES_ICREATETRIANGLEDOCUMENT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace Usul {
namespace Interfaces {


struct ICreateTriangleDocument : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICreateTriangleDocument );

  /// Id for this interface.
  enum { IID = 102523u };

  virtual void    createTriangleDocument ( OsgTools::Triangles::TriangleSet*, IUnknown *caller = 0x0 ) = 0;

}; // struct ICreateTriangleDocument


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICREATETRIANGLEDOCUMENT_H__ */
