
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IACTIVE_DOCUMENT_LISTENER_H__
#define __USUL_INTERFACES_IACTIVE_DOCUMENT_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IActiveDocumentListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveDocumentListener );

  /// Id for this interface.
  enum { IID = 3846654953u };

  /// The active document has changed.
  virtual void                          activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc ) = 0;

}; // struct IActiveDocumentListener


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IACTIVE_DOCUMENT_LISTENER_H__
