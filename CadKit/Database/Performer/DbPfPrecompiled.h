
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfPrecompiled.h: The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
# define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX              // Do not define min and max as macros.
# include <windows.h>          // Base Windows header file.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# include <math.h>
# include <string>
# include <vector>
# include <list>
# include <map>
# include <stack>
# include <algorithm>
# include "Performer/pf/pfDCS.h"
# include "Performer/pf/pfLOD.h"
# include "Performer/pf/pfGeode.h"
# include "Performer/pr/pfGeoSet.h"
# include "Performer/pr/pfGeoState.h"
# include "Performer/pr/pfLinMath.h"
# include "Performer/pr/pfGeoMath.h"
# include "Performer/pr/pfMaterial.h"
# include "Performer/pfdu.h"
#endif

#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
