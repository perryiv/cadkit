
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_TIMESTEP_AMOUNT_H__
#define __MINERVA_CORE_COMMANDS_TIMESTEP_AMOUNT_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT TimestepAmount : public Usul::Commands::Command,
                                      public Usul::Interfaces::ISerialize
{
public:
  // Typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_REF_POINTERS ( TimestepAmount );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  TimestepAmount ( );
  TimestepAmount ( unsigned int amount );

protected:
  virtual ~TimestepAmount();

  virtual void                _execute();

private:

  unsigned int _amount;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( TimestepAmount );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_TIMESTEP_AMOUNT_H__
