
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtVisApiHeaders: Include this to get the VisAPI headers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_
#define _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_

#ifdef _WIN32

// This hack is because DMDataTk/eaiStandard.h (which is indirectly included 
// below) includes <iostream.h>. This causes VC++ error C2874 with STLport.
#define EAI_STANDARD_HXX // From DMDataTk/eaiStandard.h
#define EAI_TOOLKIT_API __declspec(dllimport)
enum { eai_ERROR = 0, eai_OK = 1 };
enum Units { UNKNOWN=0, MICROMETERS, MILLIMETERS, CENTIMETERS, DECIMETERS, 
             METERS, KILOMETERS, INCHES, FEET, YARDS, MILES, MILS };

// DMDTk 5.2 introduced <iostream.h> in DMDataTk/eaiEntityList.h.
#define EAI_ENTITY_LIST_HXX
class eaiEntityList;

#pragma warning ( disable: 4251 )

#endif // _WIN32

#include <stdlib.h> // For "NULL", used in eaiStandard.h, 
                    // indirectly included by eaiEntityFactory.h

#include "DMDataTk/eaiEntityFactory.h" // Doesn't compile in DbJtPrecompiled.h
#include "DMDataTk/eaiCADImporter.h"
#include "DMDataTk/eaiTraverser.h"
#include "DMDataTk/eaiAttrib.h"
#include "DMDataTk/eaiLineStripSet.h"
#include "DMDataTk/eaiPointSet.h"
#include "DMDataTk/eaiPolygonSet.h"
#include "DMDataTk/eaiTriStripSet.h"
#include "DMDataTk/eaiClientData.h"

#endif // _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_
