
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_VIEWS_FORMAT_H__
#define __CADKIT_HELIOS_QT_VIEWS_FORMAT_H__

#include "Helios/Qt/Views/OSG/Export.h"

class QGLFormat;

namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {

/// Get the default Qt OpenGL format.
HELIOS_QT_VIEWS_OSG_EXPORT  QGLFormat  defaultFormat ();

}
}
}
}

#endif // __CADKIT_HELIOS_QT_VIEWS_FORMAT_H__
