
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

namespace FX { class FXObject; class FXMetaClass; }


namespace FoxTools {
namespace Predicates {


struct FOX_TOOLS_EXPORT IsMemberOf
{
  IsMemberOf ( const FX::FXMetaClass *meta );
  IsMemberOf ( const IsMemberOf & );
  ~IsMemberOf();

  IsMemberOf &    operator = ( const IsMemberOf & );
  bool            operator () ( const FX::FXObject *object ) const;

private:

  const FX::FXMetaClass *_meta;
};


} // namespace Predicates
} // namespace FoxTools


#endif // _FOX_TOOLS_PREDICATE_IS_MEMBER_OF_H_
