
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_BUILD_SCENE_H_
#define _USUL_INTERFACE_BUILD_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace osg { class Node; }

namespace Usul {
namespace Interfaces {

struct IBuildScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBuildScene );

  /// Id for this interface.
  enum { IID = 3373393853u };

  typedef std::map<std::string,std::string>     Options;

  virtual osg::Node *         buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;


}; // class IBuildScene


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_BUILD_SCENE_H_
