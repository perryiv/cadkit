
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_INAVIGATION_FUNCTOR_H__
#define __USUL_INTERFACES_INAVIGATION_FUNCTOR_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Functors { namespace Interaction { namespace Common { class BaseFunctor; } } } }

namespace Usul {
namespace Interfaces {


struct INavigationFunctor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INavigationFunctor );

  /// Id for this interface.
  enum { IID = 2228532706u };

  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseFunctor;

  /// Get/Set the navigation functor.
  virtual void                         navigator ( BaseFunctor * ) = 0;
  virtual BaseFunctor *                navigator () = 0;
  virtual const BaseFunctor *          navigator () const = 0;

}; // struct INavigationFunctor


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_INAVIGATION_FUNCTOR_H__
