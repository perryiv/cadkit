
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtVisApiHeaders: Include this to get the VisAPI headers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_
#define _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_

// This hack is because DMDataTk/eaiStandard.h (which is indirectly included 
// below) includes <iostream.h>. This causes VC++ error C2874 with STLport.
#ifdef _WIN32
#define EAI_STANDARD_HXX // From DMDataTk/eaiStandard.h
#define EAI_TOOLKIT_API __declspec(dllimport)
enum { eai_ERROR = 0, eai_OK = 1 };
enum Units { UNKNOWN=0, MICROMETERS, MILLIMETERS, CENTIMETERS, DECIMETERS, 
             METERS, KILOMETERS, INCHES, FEET, YARDS, MILES, MILS };
#endif // _WIN32

#include "DMDataTk/eaiEntityFactory.h" // Doesn't compile in DbJtPrecompiled.h
#include "DMDataTk/eaiCADImporter.h"
#include "DMDataTk/eaiTraverser.h"
#include "DMDataTk/eaiAttrib.h"
#include "DMDataTk/eaiLineStripSet.h"
#include "DMDataTk/eaiPointSet.h"
#include "DMDataTk/eaiPolygonSet.h"
#include "DMDataTk/eaiTriFanSet.h"
#include "DMDataTk/eaiTriStripSet.h"

#endif // _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_
