
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PERFORMER_SAL_FACTORY_CLASS_H_
#define _PERFORMER_SAL_FACTORY_CLASS_H_

#include "SAL/Performer/CompileGuard.h"

#include "Usul/Base/Referenced.h"

#include "Usul/Interfaces/IClassFactory.h"

class pfNode;


namespace SAL {
namespace Performer {


class Factory : public Usul::Base::Referenced,
                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Factory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Factory();

  /// Create the proper component from the given pointer.
  static Unknown *        create ( ::pfNode * );

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


}; // namespace Performer
}; // namespace SAL


#endif // _PERFORMER_SAL_FACTORY_CLASS_H_
