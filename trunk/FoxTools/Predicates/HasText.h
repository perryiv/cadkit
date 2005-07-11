
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for checking if the text is present.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_PREDICATE_HAS_TEXT_H_
#define _FOX_TOOLS_PREDICATE_HAS_TEXT_H_

#include "FoxTools/Export/Export.h"

#include <functional>
#include <string>

namespace FX { class FXObject; }


namespace FoxTools {
namespace Predicates {


struct FOX_TOOLS_EXPORT HasText : public std::unary_function < const FX::FXObject *, bool >
{
  typedef std::unary_function < const FX::FXObject *, bool > BaseClass;

  HasText ( const std::string &text, bool substring = false );
  HasText ( const HasText & );
  ~HasText();

  HasText &       operator = ( const HasText & );
  bool            operator () ( const FX::FXObject *object ) const;

private:

  std::string _text;
  bool _sub;
};


} // namespace Predicates
} // namespace FoxTools


#endif // _FOX_TOOLS_PREDICATE_HAS_TEXT_H_
