
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_INTERNAL_PRECOMPILED_HEADERS_H_
#define _WX_GUI_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef _COMPILING_WX_GUI
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# pragma warning ( disable : 4251 ) // DLL-interface warning.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include "wx/wxprec.h"
# include "wx/wx.h"
# include "wx/docview.h"
# include "wx/docmdi.h"
# include "wx/image.h"
# include "wx/bitmap.h"
# include "wx/icon.h"
# include "wx/menu.h"
# include "wx/splitter.h"
# include "wx/colordlg.h"
# include "wx/splash.h"
# include "wx/artprov.h"
# include "wx/generic/colrdlgg.h"
# include "wx/html/htmlwin.h"
# include "wx/ifm/ifm.h"

# include "boost/shared_ptr.hpp"

# include <windows.h>

# include <stdexcept>
# include <iostream>
# include <algorithm>
# include <map>
# include <set>
# include <string>

#endif // _WIN32

#endif // _WX_GUI_INTERNAL_PRECOMPILED_HEADERS_H_
