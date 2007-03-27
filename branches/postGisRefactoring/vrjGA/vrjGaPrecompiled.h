
///////////////////////////////////////////////////////////////////////////////
//
//  Insert copyright here.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_INTERNAL_PRECOMPILED_HEADERS_H_
#define _VRJGA_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef _COMPILING_VRJ_GA
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++
# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# pragma warning ( disable : 4244 ) // Integer truncation.
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.
# include <windows.h>               // Base Windows header file.
# include <gadget/Type/AnalogInterface.h>  // for stick
# include <gadget/Type/Digital.h>  // for buttons
# include <gadget/Type/DigitalInterface.h>  // for buttons
# include <gadget/Type/PositionInterface.h>  // for tracker
# include <gmtl/Generate.h>
# include <gmtl/Matrix.h>
# include <string>
# include <map>
#endif // _MSC_VER

#endif // _VRJGA_INTERNAL_PRECOMPILED_HEADERS_H_
