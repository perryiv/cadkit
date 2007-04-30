
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scope for setting groups.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_SETTINGS_GROUP_SCOPE_H_
#define _CADKIT_HELIOS_QT_CORE_SETTINGS_GROUP_SCOPE_H_

#include "Helios/Qt/Core/Export.h"

#include <string>

class QSettings;


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT SettingsGroupScope
{
public:

  SettingsGroupScope ( const std::string &name, QSettings &settings );
  ~SettingsGroupScope();

private:

  QSettings &_settings;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_SETTINGS_GROUP_SCOPE_H_
