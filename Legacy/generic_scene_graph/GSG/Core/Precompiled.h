
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

#ifndef _GENERIC_SCENE_GRAPH_CORE_INTERNAL_PRECOMPILED_HEADERS_H_
#define _GENERIC_SCENE_GRAPH_CORE_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef COMPILING_GSG_CORE
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>               // Base Windows header file.

# include "boost/static_assert.hpp"
# include "boost/intrusive_ptr.hpp"
# include "boost/mpl/assert_is_same.hpp"
# include "boost/call_traits.hpp"

# include <cassert>
# include <utility>
# include <string>
# include <vector>
# include <map>
# include <iterator>
# include <limits>
# include <algorithm>
# include <stack>
# include <memory>
# include <list>
# include <sstream>

#endif // _WIN32

#endif // _GENERIC_SCENE_GRAPH_CORE_INTERNAL_PRECOMPILED_HEADERS_H_
