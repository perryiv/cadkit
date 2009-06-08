

///////////////////////////////////////////////////////////////////////////////

//

//  Copyright (c) 2002, Perry L. Miller IV

//  All rights reserved.

//  BSD License: http://www.opensource.org/licenses/bsd-license.html

//

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

//

//  DbJtVisApiVersion: A work-around for the unfriendly DMDTk headers.

//

///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_USE_JTOPEN

#include "JtTk/JtkStandard.h"

#else // _CADKIT_USE_JTOPEN

#include "DMDataTk/eaiStandard.h"

#endif // _CADKIT_USE_JTOPEN

// I want this to break if the number is wrong. DMDTk versions >= 5.0

// define this in eaiStandard.h.

#define DMDTK_MAJOR_VERSION 5

