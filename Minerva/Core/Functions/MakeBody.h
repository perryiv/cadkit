
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for creating common bodies.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FUNCTIONS_MAKE_BODY_H__
#define __MINERVA_CORE_FUNCTIONS_MAKE_BODY_H__

#include "Minerva/Core/Export.h"

// Forward declarations.
namespace Usul { namespace Jobs { class Manager; } }
namespace Minerva { namespace Core { namespace TileEngine { class Body; } } }

namespace Minerva {
namespace Core {
namespace Functions {
  

  MINERVA_EXPORT Minerva::Core::TileEngine::Body* makeEarth ( Usul::Jobs::Manager * );

  
}
}
}


#endif // __MINERVA_CORE_FUNCTIONS_CACHE_STRING_H__
