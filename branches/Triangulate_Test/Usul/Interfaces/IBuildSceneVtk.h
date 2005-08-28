
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_BUILD_SCENE_VTK_H_
#define _USUL_INTERFACE_BUILD_SCENE_VTK_H_

#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

class vtkProp;

namespace Usul {
namespace Interfaces {

struct IBuildSceneVtk : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBuildSceneVtk );

  /// Id for this interface.
  enum { IID = 4194675303u };

  typedef std::map<std::string,std::string>     Options;

  virtual vtkProp *         buildSceneVtk ( const Options &options, Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;


}; // class IBuildSceneVtk


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_BUILD_SCENE_VTK_H_
