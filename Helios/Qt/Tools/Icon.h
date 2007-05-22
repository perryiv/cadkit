
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
//  Helper class for creating icons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_TOOLS_ICONS_H_
#define _CADKIT_HELIOS_QT_TOOLS_ICONS_H_

#include "Helios/Qt/Tools/Export.h"

#include <string>

class QWidget;
class QAction;


namespace CadKit {
namespace Helios {
namespace Tools {


struct HELIOS_QT_TOOLS_EXPORT Icon
{
  static void set ( const std::string &file, QWidget *widget );
  static void set ( const std::string &file, QAction *action );
};


} // namespace Tools
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_TOOLS_ICONS_H_
