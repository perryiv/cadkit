
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ClipPlaneFactory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CLIP_PLANES_FACTORY_CLASS_H_
#define _CLIP_PLANES_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


namespace OsgFox {
namespace Plugins {
namespace ClipPlanes {


class ClipPlaneFactory : public Usul::Base::Referenced,
                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ClipPlaneFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ClipPlaneFactory();

protected:

  /// Use reference counting.
  virtual ~ClipPlaneFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};

}; // namespace ClipPlanes
}; // namespace Plugins
}; // namespace OsgFox


#endif // _VIEW_SEEK_FACTORY_CLASS_H_
