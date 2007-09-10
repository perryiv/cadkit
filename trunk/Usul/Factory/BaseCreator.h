
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_BASE_CREATOR_CLASS_
#define _USUL_FACTORY_BASE_CREATOR_CLASS_

#include "Usul/Export/Export.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace Usul { namespace Base { class Referenced; } }

namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT BaseCreator
{
public:

  USUL_DECLARE_REF_POINTERS ( BaseCreator );

  BaseCreator ( const std::string &name );

  virtual Usul::Base::Referenced *  operator()() = 0;

  const std::string &               name();

  void                              ref ();
  void                              unref ( bool allowDeletion = true );

protected:

  virtual ~BaseCreator();

  std::string  _name;
  unsigned int _refCount;
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_BASE_CREATOR_CLASS_
