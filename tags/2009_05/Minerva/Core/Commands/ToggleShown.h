
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_TOGGLE_SHOWN_H__
#define __MINERVA_CORE_COMMANDS_TOGGLE_SHOWN_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/IBooleanState.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT ToggleShown : public Usul::Commands::Command,
                                   public Usul::Interfaces::ISerialize
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( ToggleShown );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ToggleShown ();
  ToggleShown ( Usul::Interfaces::IUnknown* layer, const std::string& text );

protected:
  virtual ~ToggleShown();

  virtual void                _execute();

  // Is the item checked?
  virtual bool                updateCheck () const;

private:
  Usul::Interfaces::IUnknown::QueryPtr _layer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( ToggleShown );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_TOGGLE_SHOWN_H__
