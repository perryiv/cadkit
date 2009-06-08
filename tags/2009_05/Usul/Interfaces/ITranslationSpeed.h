
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
//  Interface for returning the translation speed.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_TRANSLATION_SPEED_H_
#define _USUL_INTERFACES_TRANSLATION_SPEED_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITranslationSpeed : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITranslationSpeed );

  // Id for this interface.
  enum { IID = 4002239526u };

  // Add the new model.
  virtual float translationSpeed () const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_TRANSLATION_SPEED_H_
