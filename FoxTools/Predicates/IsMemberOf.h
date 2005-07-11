
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for checking class-type membership.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_PREDICATE_IS_MEMBER_OF_H_
#define _FOX_TOOLS_PREDICATE_IS_MEMBER_OF_H_

#include "FoxTools/Export/Export.h"

#include <functional>

namespace FX { class FXObject; class FXMetaClass; }


namespace FoxTools {
namespace Predicates {


struct FOX_TOOLS_EXPORT IsMemberOf : public std::unary_function < const FX::FXObject *, bool >
{
  typedef std::unary_function < const FX::FXObject *, bool > BaseClass;

  IsMemberOf ( const FX::FXMetaClass *meta, bool doCount, unsigned int which = 0 );
  IsMemberOf ( const IsMemberOf & );
  ~IsMemberOf();

  IsMemberOf &    operator = ( const IsMemberOf & );
  bool            operator () ( const FX::FXObject *object ) const;

private:

  const FX::FXMetaClass *_meta;
  bool _doCount;
  mutable unsigned int _count;
  unsigned int _which;
};


} // namespace Predicates
} // namespace FoxTools


#endif // _FOX_TOOLS_PREDICATE_IS_MEMBER_OF_H_
