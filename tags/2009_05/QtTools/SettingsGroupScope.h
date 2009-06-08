
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

#ifndef _QT_TOOLS_SETTINGS_GROUP_SCOPE_H_
#define _QT_TOOLS_SETTINGS_GROUP_SCOPE_H_

#include "QtTools/Export.h"

#include <string>

class QSettings;


namespace QtTools {


class QT_TOOLS_EXPORT SettingsGroupScope
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


} // namespace QtTools


#endif //_QT_TOOLS_SETTINGS_GROUP_SCOPE_H_
