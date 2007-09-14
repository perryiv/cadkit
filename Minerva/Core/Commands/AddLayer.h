
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
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Jobs/Job.h"

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

  USUL_DECLARE_QUERY_POINTERS ( AddLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  AddLayer ();
  AddLayer ( Usul::Interfaces::IUnknown * caller, Usul::Interfaces::ILayer* layer );
  AddLayer ( Usul::Interfaces::ILayer* layer );

protected:
  virtual ~AddLayer();

  virtual void                _execute();

  class AddLayerJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;

    AddLayerJob ( Usul::Interfaces::ILayer* layer, Usul::Interfaces::IUnknown *caller );

  protected:
    // Use reference counting.
    virtual ~AddLayerJob();

    bool               _addLayer ( Usul::Interfaces::IUnknown *caller );

    // Do the work here.
    virtual void       _started();
    virtual void       _finished();

  private:
    Usul::Interfaces::ILayer::QueryPtr _layer;
    Usul::Interfaces::IUnknown::QueryPtr _caller;
    Usul::Interfaces::IUnknown::QueryPtr _document;
  };

private:
  Usul::Interfaces::ILayer::QueryPtr _layer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS( AddLayer );
};

}
}
}

#endif // __MINERVA_CORE_COMMANDS_ADD_LAYER_H__
