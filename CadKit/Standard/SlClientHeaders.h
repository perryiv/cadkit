
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//	SlClientHeaders.h: Put this in your precompiled header file.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CLIENT_HEADERS_H_
#define _CADKIT_STANDARD_LIBRARY_CLIENT_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include <math.h>
#include <memory> // For std::pair
#include <vector>
#include <locale>
#include <stack>
#include <list>
#include <algorithm> // For std::transform

// Note: SlStringIO.h is left out because it includes <iostream>
#include "SlVec2.h"
#include "SlVec3.h"
#include "SlVec4.h"
#include "SlRefBase.h"
#include "SlPrint.h"
#include "SlString.h"
#include "SlStringFunctions.h"
#include "SlThread.h"
#include "SlColor.h"
#include "SlConstants.h"
#include "SlMatrix44.h"
#include "SlBoundingBox.h"
#include "SlTrace.h"
#include "SlLine2.h"
#include "SlLine3.h"
#include "SlMessage.h"
#include "SlRefPtr.h"
#include "SlViewport.h"
#include "SlCallbackList.h"
#include "SlBitmask.h"
#include "SlTrackball.h"
#include "SlPreComputedArrays.h"
#include "SlList.h"
#include "SlStack.h"
#include "SlPathname.h"
#include "SlMaterial.h"
#include "SlInline.h"
#include "SlSystem.h"

#endif // _CADKIT_STANDARD_LIBRARY_CLIENT_HEADERS_H_
