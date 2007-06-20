
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_ANIMATION_SPEED_H__
#define __MINERVA_CORE_COMMANDS_ANIMATION_SPEED_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT AnimationSpeed : public Usul::Commands::Command,
                                      public Usul::Interfaces::ISerialize
{
public:
  // Typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_REF_POINTERS ( AnimationSpeed );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  AnimationSpeed ( double speed );

protected:
  virtual ~AnimationSpeed();

  virtual void                _execute();

private:

  double _speed;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( AnimationSpeed );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_ANIMATION_SPEED_H__
