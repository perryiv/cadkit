
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IPASSWORD_PROMPT_H__
#define __USUL_INTERFACES_IPASSWORD_PROMPT_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct IPasswordPrompt : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPasswordPrompt );

  /// Id for this interface.
  enum { IID = 3801224426u };

  /// Prompt the user for a password.
	virtual std::string                          promptForPassword ( const std::string& text ) = 0;

}; // struct IPasswordPrompt


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IPASSWORD_PROMPT_H__
