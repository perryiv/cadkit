
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ITranslator: Interface for a translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_TRANSLATOR_H_
#define _CADKIT_INTERFACE_TRANSLATOR_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
#endif


namespace CadKit
{
class ITranslator : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032754104 };

  // Translate the database.
  virtual bool            translate ( const std::string &name, IUnknown *source, IUnknown *target ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_TRANSLATOR_H_
