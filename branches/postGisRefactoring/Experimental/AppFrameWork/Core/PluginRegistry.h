
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin registry class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_PLUGIN_REGISTRY_CLASS_H_
#define _APP_FRAME_WORK_PLUGIN_REGISTRY_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include <string>
#include <list>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT PluginRegistry
{
public:

  // Typedefs.
  typedef std::list < std::string > PluginFiles;

  // Constructor and destructor.
  PluginRegistry();
  ~PluginRegistry();

  // Read registry file.
  bool                read ( PluginFiles & );
  bool                read ( const std::string &file, PluginFiles & );

protected:

  void                _addModule ( const std::string & );

  void                _moduleFile ( const std::string & );

  bool                _read ( const std::string &file, PluginFiles & );

private:

  // No copying.
  PluginRegistry ( const PluginRegistry & );
  PluginRegistry &operator = ( const PluginRegistry & );

  std::string _file;
  PluginFiles _plugins;
};


} // namespace Core
} // namespace AFW


#endif // _APP_FRAME_WORK_PLUGIN_REGISTRY_CLASS_H_
