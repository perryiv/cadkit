
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_ADD_LAYER_H__
#define __MINERVA_CORE_COMMANDS_ADD_LAYER_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT AddLayer : public Usul::Commands::Command,
                                public Usul::Interfaces::ISerialize
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( AddLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  AddLayer ();
  AddLayer ( Usul::Interfaces::IUnknown * parent, Usul::Interfaces::IUnknown* layer );

protected:
  
  virtual ~AddLayer();

  virtual void                _execute();

private:
  Usul::Interfaces::IUnknown::QueryPtr _parent;
  Usul::Interfaces::IUnknown::QueryPtr _layer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( AddLayer );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_ADD_LAYER_H__
