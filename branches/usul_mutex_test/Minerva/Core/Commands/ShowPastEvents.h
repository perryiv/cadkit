
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_SHOW_PAST_EVENTS_H__
#define __MINERVA_CORE_COMMANDS_SHOW_PAST_EVENTS_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT ShowPastEvents : public Usul::Commands::Command,
                                      public Usul::Interfaces::ISerialize
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( ShowPastEvents );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ShowPastEvents ();
  ShowPastEvents ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~ShowPastEvents();

  virtual void                _execute();

  // Is the item checked?
  virtual bool                updateCheck () const;

private:

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( ShowPastEvents );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_SHOW_PAST_EVENTS_H__
