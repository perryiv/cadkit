
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
#include "SlMatrix4.h"
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
