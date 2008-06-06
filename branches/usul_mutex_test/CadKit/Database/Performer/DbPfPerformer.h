
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfPerformer.h: 
//  The Performer headers are unsafe and produce tons of warnings (at least 
//  with VC6).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_PERFORMER_HEADERS_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_PERFORMER_HEADERS_H_

# ifdef _MSC_VER
#  pragma warning ( push )
#  pragma warning ( disable: 4244 4244 )
//#  define __WIN32STUBS_H__
# endif

#include "Performer/pf/pfDCS.h"
#include "Performer/pf/pfLOD.h"
#include "Performer/pf/pfGeode.h"
#include "Performer/pr/pfGeoSet.h"
#include "Performer/pr/pfGeoState.h"
#include "Performer/pr/pfLinMath.h"
#include "Performer/pr/pfGeoMath.h"
#include "Performer/pr/pfMaterial.h"
#include "Performer/pfdu.h"

# ifdef _MSC_VER
#  pragma warning ( pop )
# endif

#undef sinf
#undef cosf
#undef sqrtf
#undef expf
#undef tanf
#undef floorf
#undef atan2f
#undef asinf
#undef acosf


#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_PERFORMER_HEADERS_H_
