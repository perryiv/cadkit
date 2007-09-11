
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor sequence.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTORS_INTERACTION_SEQUENCE_H_
#define _USUL_FUNCTORS_INTERACTION_SEQUENCE_H_

#include "Usul/Functors/Interaction/Common/BaseFunctor.h"

#include <vector>


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Common {


class USUL_EXPORT Sequence : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef std::vector<BaseClass::RefPtr> Functors;
  typedef BaseClass::Unknown Unknown;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Sequence );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Sequence );

  Sequence ( Unknown *caller = 0x0, const std::string &name = "" );
  Sequence ( const Sequence &tp );

  // Append a functor.
  void                  append ( BaseClass * );

  // Have the functors do their thing.
  virtual void          operator()();

  // Set the caller.
  virtual void          caller ( Unknown* caller );

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~Sequence();

  Functors _functors;
};


} // namespace Common
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTORS_INTERACTION_SEQUENCE_H_
