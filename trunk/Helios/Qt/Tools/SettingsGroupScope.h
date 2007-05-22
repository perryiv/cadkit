
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
//  Scope for setting groups.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_TOOLS_SETTINGS_GROUP_SCOPE_H_
#define _CADKIT_HELIOS_QT_TOOLS_SETTINGS_GROUP_SCOPE_H_

#include "Helios/Qt/Tools/Export.h"

#include <string>

class QSettings;


namespace CadKit {
namespace Helios {
namespace Tools {


class HELIOS_QT_TOOLS_EXPORT SettingsGroupScope
{
public:

  SettingsGroupScope ( const std::string &name, QSettings &settings );
  ~SettingsGroupScope();

private:

  // No copying or assignment.
  SettingsGroupScope ( const SettingsGroupScope & );
  SettingsGroupScope &operator = ( const SettingsGroupScope & );

  QSettings &_settings;
};


} // namespace Tools
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_TOOLS_SETTINGS_GROUP_SCOPE_H_
