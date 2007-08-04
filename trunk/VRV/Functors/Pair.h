
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor pair.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_FUNCTOR_PAIR_CALLBACK_H_
#define _VRV_FUNCTOR_PAIR_CALLBACK_H_

#include "VRV/Export.h"
#include "VRV/Functors/BaseFunctor.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT Pair : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Pair );
  typedef VRV::Functors::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;

  Pair ( 
    Unknown *unknown, 
    BaseFunctor *c1, 
    BaseFunctor *c2,
    unsigned int id );
  Pair ( const Pair &tp );

  // Have the transforms do their thing.
  virtual void          operator()();

protected:

  virtual ~Pair();

  BaseFunctor::RefPtr _c1;
  BaseFunctor::RefPtr _c2;
};


} // namespace Functors
} // namespace VRV


#endif // _VRV_FUNCTOR_PAIR_CALLBACK_H_
