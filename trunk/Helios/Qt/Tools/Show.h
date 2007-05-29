
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
//  Class for showing widgets.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_TOOLS_SHOW_H_
#define _CADKIT_HELIOS_QT_TOOLS_SHOW_H_

#include "Helios/Qt/Tools/Export.h"

class QWidget;


namespace CadKit {
namespace Helios {
namespace Tools {


struct HELIOS_QT_TOOLS_EXPORT Show
{
  static void center ( QWidget *widget );
};


} // namespace Tools
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_TOOLS_SHOW_H_
