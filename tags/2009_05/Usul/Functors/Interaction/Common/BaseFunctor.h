
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_INTERACTION_BASE_CLASS_H_
#define _USUL_FUNCTOR_INTERACTION_BASE_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Common {


class USUL_EXPORT BaseFunctor : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BaseFunctor );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseFunctor );

  // Get/Set the caller.
  const Unknown *       caller() const;
  Unknown *             caller();
  virtual void          caller ( Unknown* caller );

  // Exact functionality determined by inheriting class.
  virtual void          operator()() = 0;

  // Clone this functor.
  virtual BaseFunctor*  clone () = 0;

protected:

  BaseFunctor ( Unknown *caller, const std::string &name );
  BaseFunctor ( const BaseFunctor &cb );
  virtual ~BaseFunctor();

private:

  Unknown::QueryPtr _caller;
};


} // namespace Common
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_INTERACTION_BASE_CLASS_H_
