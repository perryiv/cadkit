
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for moving widgets.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_TOOLS_MOVE_H_
#define _CADKIT_HELIOS_QT_TOOLS_MOVE_H_

#include "Helios/Qt/Tools/Export.h"

class QWidget;


namespace CadKit {
namespace Helios {
namespace Tools {


struct HELIOS_QT_TOOLS_EXPORT Move
{
  static void center ( QWidget *widget );
};


} // namespace Tools
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_TOOLS_MOVE_H_
