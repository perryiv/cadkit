
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_MAX_DATE_SHOWN_H__
#define __MINERVA_CORE_COMMANDS_MAX_DATE_SHOWN_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT MaxDateShown : public Usul::Commands::Command
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_REF_POINTERS ( MaxDateShown );

  MaxDateShown ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~MaxDateShown();

  virtual void                _execute();
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_MAX_DATE_SHOWN_H__
