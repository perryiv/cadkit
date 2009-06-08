
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICOMMAND_QUEUE_ADD_H__
#define __USUL_INTERFACES_ICOMMAND_QUEUE_ADD_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

  struct ICommand;

struct ICommandQueueAdd : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICommandQueueAdd );

  /// Id for this interface.
  enum { IID = 2017280712u };

  virtual void  addCommand ( Usul::Interfaces::ICommand* command ) = 0;

}; // struct ICommandQueueAdd


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICOMMAND_QUEUE_ADD_H__ */

