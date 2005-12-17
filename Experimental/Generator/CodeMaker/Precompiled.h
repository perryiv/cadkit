
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

#ifndef _CODE_MAKER_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CODE_MAKER_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef _COMPILING_CODE_MAKER
# error "This header file is only used for compiling the application."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include "wx/wxprec.h"
# include "wx/wx.h"
# include "wx/docview.h"
# include "wx/docmdi.h"
# include "wx/config.h"
# include "wx/image.h"
# include "wx/bitmap.h"
# include "wx/icon.h"
# include "wx/menu.h"
# include "wx/splitter.h"
# include "wx/colordlg.h"
# include "wx/xrc/xmlres.h"
# include "wx/fl/controlbar.h"    // Core API
# include "wx/fl/barhintspl.h"    // Bevel for bars with "X"s and grooves
# include "wx/fl/rowdragpl.h"     // NC-look with draggable rows
# include "wx/fl/cbcustom.h"      // Customization plugin
# include "wx/fl/hintanimpl.h"
# include "wx/fl/gcupdatesmgr.h"  // Smooth d&d
# include "wx/fl/antiflickpl.h"   // Double-buffered repaint of decorations
# include "wx/fl/dyntbar.h"       // Auto-layout toolbar
# include "wx/fl/dyntbarhnd.h"    // Control-bar dimension handler for it

# include "boost/shared_ptr.hpp"

# include <windows.h>

# include <stdexcept>
# include <iostream>
# include <algorithm>
# include <map>
# include <set>
# include <string>

#endif // _WIN32

#endif // _CODE_MAKER_INTERNAL_PRECOMPILED_HEADERS_H_
