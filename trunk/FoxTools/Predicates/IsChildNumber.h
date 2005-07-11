
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for checking the child's location in the parent's list.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_PREDICATE_IS_CHILD_NUMBER_H_
#define _FOX_TOOLS_PREDICATE_IS_CHILD_NUMBER_H_

#include "FoxTools/Export/Export.h"

#include <functional>

namespace FX { class FXObject; }


namespace FoxTools {
namespace Predicates {


struct FOX_TOOLS_EXPORT IsChildNumber : public std::unary_function < const FX::FXObject *, bool >
{
  typedef std::unary_function < const FX::FXObject *, bool > BaseClass;

  IsChildNumber ( unsigned int num );
  IsChildNumber ( const IsChildNumber & );
  ~IsChildNumber();

  IsChildNumber & operator = ( const IsChildNumber & );
  bool            operator () ( const FX::FXObject *object ) const;

private:

  unsigned int _num;
  mutable unsigned int _count;
};


} // namespace Predicates
} // namespace FoxTools


#endif // _FOX_TOOLS_PREDICATE_IS_CHILD_NUMBER_H_
