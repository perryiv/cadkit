
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMD_FACTORY_CLASS_H_
#define _IMD_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


namespace OsgFox {
namespace Plugins {
namespace IMD {


class Factory : public Usul::Base::Referenced,
                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Factory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Factory();

protected:

  /// Use reference counting.
  virtual ~Factory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};

}; // namespace IMD
}; // namespace Plugins
}; // namespace OsgFox


#endif // _IMD_FACTORY_CLASS_H_
