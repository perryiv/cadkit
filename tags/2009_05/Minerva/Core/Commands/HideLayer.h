
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_COMMANDS_HIDE_LAYER_H__
#define __MINERVA_CORE_COMMANDS_HIDE_LAYER_H__

#include "Minerva/Core/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/ILayer.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Commands {

class MINERVA_EXPORT HideLayer : public Usul::Commands::Command,
                                 public Usul::Interfaces::ISerialize
{
public:
  // typedef(s)
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( HideLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  HideLayer ();
  HideLayer ( Usul::Interfaces::ILayer* layer );

protected:
  virtual ~HideLayer();

  virtual void                _execute();

private:
  Usul::Interfaces::ILayer::QueryPtr _layer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( HideLayer );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_HIDE_LAYER_H__
