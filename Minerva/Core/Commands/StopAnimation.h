
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_STOP_ANIMATION_H__
#define __MINERVA_CORE_COMMANDS_STOP_ANIMATION_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT StopAnimation : public Usul::Commands::Command,
                                     public Usul::Interfaces::ISerialize
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( StopAnimation );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  StopAnimation ( );
  StopAnimation ( Usul::Interfaces::IUnknown *caller );

protected:
  virtual ~StopAnimation();

  virtual void                _execute();

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( StopAnimation );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_STOP_ANIMATION_H__
