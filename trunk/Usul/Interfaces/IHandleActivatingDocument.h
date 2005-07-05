
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for handling things that need to be done when a document is 
//  activating/deactivating.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_HANDLE_ACTIVATING_DOCUMENT_H_
#define _USUL_INTERFACE_HANDLE_ACTIVATING_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {

  namespace Documents { class Document; }

namespace Interfaces {


struct IHandleActivatingDocument : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IHandleActivatingDocument );

  /// Id for this interface.
  enum { IID = 2833672391u };

  // Notify the document that it is no longer active.
  virtual void                noLongerActive ( Usul::Documents::Document* document, const std::string& activeType ) = 0;

  // Notify the document that it is active.
  virtual void                nowActive      ( Usul::Documents::Document* document, const std::string& oldType ) = 0;

}; // struct IHandleActivatingDocument


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_HANDLE_ACTIVATING_DOCUMENT_H_
