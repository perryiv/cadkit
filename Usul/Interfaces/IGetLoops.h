
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get loops for capping
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_LOOPS_H_
#define _USUL_INTERFACE_GET_LOOPS_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace OsgTools { namespace Triangles { class Loop; }  }

namespace Usul {
namespace Interfaces {


struct IGetLoops : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetLoops );

  /// Id for this interface.
  enum { IID = 3752436619u };

  typedef std::vector< OsgTools::Triangles::Loop > Loops;

  virtual const Loops& getUncappedLoops () const = 0;
  virtual Loops&       getUncappedLoops () = 0;

  virtual const Loops& getCappedLoops () const = 0;
  virtual Loops&       getCappedLoops () = 0;

}; // struct IGetLoops


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GET_LOOPS_H_
