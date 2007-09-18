
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_NAVIGATOR_H__
#define __VRV_COMMANDS_NAVIGATOR_H__

#include "VRV/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Functors/Interaction/Common/BaseFunctor.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace VRV {
namespace Commands {

class VRV_EXPORT Navigator : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseFunctor;

  USUL_DECLARE_COMMAND ( Navigator );

  Navigator ( BaseFunctor* functor, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~Navigator ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  BaseFunctor::RefPtr _functor;
};

}
}

#endif // __VRV_COMMANDS_NAVIGATOR_H__
