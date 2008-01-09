
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_CHANGE_TIMESTEP_TYPE_H__
#define __MINERVA_CORE_COMMANDS_CHANGE_TIMESTEP_TYPE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT ChangeTimestepType : public Usul::Commands::Command,
                                          public Usul::Interfaces::ISerialize
{
public:
  // Typedef(s)
  typedef Usul::Commands::Command BaseClass;
  typedef Minerva::Interfaces::IAnimationControl::TimestepType TimestepType;

  USUL_DECLARE_COMMAND ( ChangeTimestepType );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ChangeTimestepType ( );
  ChangeTimestepType ( TimestepType type, Usul::Interfaces::IUnknown * caller = 0x0 );

protected:
  virtual ~ChangeTimestepType();

  virtual void                _execute();

  virtual bool                updateCheck() const;

private:

  TimestepType _type;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( ChangeTimestepType );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_CHANGE_TIMESTEP_TYPE_H__
