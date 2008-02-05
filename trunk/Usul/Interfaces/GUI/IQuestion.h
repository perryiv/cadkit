
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for a prompting a question dialog
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_QUESTION_H_
#define _USUL_INTERFACE_QUESTION_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IQuestion : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IQuestion );

  /// Id for this interface.
  enum { IID = 3434193828u };

  virtual std::string      question ( const std::string &buttons, const std::string &title, const std::string &text ) = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_QUESTION_H_

