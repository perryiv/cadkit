
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICOMMAND_LIST_H__
#define __USUL_INTERFACES_ICOMMAND_LIST_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Commands/Command.h"

#include <vector>

namespace Usul {
namespace Interfaces {

struct ICommandList : public Usul::Interfaces::IUnknown
{
  /// Typedefs
  typedef Usul::Commands::Command          Command;
  typedef std::vector < Command::RefPtr >  CommandList;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICommandList );

  /// Id for this interface.
  enum { IID = 2399688248u };

  virtual CommandList  getCommandList () = 0;

}; // struct ICommandList


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICOMMAND_LIST_H__ */

