////////////////////////////////////////////////////////////////////
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IASSEMBLY_MANAGER_H__
#define __USUL_INTERFACES_IASSEMBLY_MANAGER_H__

#include "Usul/Interfaces/IUnknown.h"

#include <vector>
#include <string>

namespace Usul {
namespace Interfaces {

  struct IAssembly;

struct IAssemblyManager : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAssemblyManager );

  /// Id for this interface.
  enum { IID = 178326388u };

  virtual void              getAssemblyNames ( std::vector< std::string >& names ) = 0;

  virtual IAssembly*        getAssemblyByName ( const std::string& name ) = 0;

  virtual void              createAssembly ( const std::string& name ) = 0;

}; // struct IAssemblyManager


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IASSEMBLY_MANAGER_H__ */
