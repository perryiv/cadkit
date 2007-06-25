
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CV_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# pragma warning ( disable : 4312 ) // Cast to type of greater size
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>
# include <direct.h>

# include "osg/Geode"
# include "osg/Matrix"
# include "osg/MatrixTransform"
# include "osg/ShapeDrawable"
# include "osg/PolygonMode"
# include "osg/LineSegment"
# include "osg/Material"

# include "osgDB/ReadFile"

# include "osgUtil/IntersectVisitor"

# include "vrj/Kernel/Kernel.h"

# include "gmtl/Generate.h"

# include "gadget/Type/DigitalInterface.h"
# include "gadget/Type/AnalogInterface.h"
# include "gadget/Type/PositionInterface.h"

# include "VRV/Devices/ButtonGroup.h"
# include "VRV/Devices/TrackerDevice.h"
# include "VRV/Devices/JoystickDevice.h"

# include "cppdom/cppdom.h"
# include "cppdom/shared_ptr.h"

# include "boost/mpl/assert.hpp"
# include "boost/static_assert.hpp"

# include <algorithm>
# include <functional>
# include <iostream>
# include <list>
# include <sstream>
# include <stdexcept>
# include <string>
# include <utility>
# include <vector>
# include <stack>
# include <memory>
# include <limits>

#endif // _MSC_VER

#endif // _CV_INTERNAL_PRECOMPILED_HEADERS_H_
