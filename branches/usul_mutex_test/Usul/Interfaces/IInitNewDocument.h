
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for initializing a new document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_INIT_NEW_DOCUMENT_H_
#define _USUL_INTERFACE_INIT_NEW_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IInitNewDocument : public Usul::Interfaces::IUnknown
{

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IInitNewDocument );

  /// Id for this interface.
  enum { IID = 1017125944u };

  /// Do we handle this document type?
  virtual bool                handlesDocumentType ( Usul::Interfaces::IUnknown *document ) = 0;

  /// Initialize a new document
  virtual void                initNewDocument ( Usul::Interfaces::IUnknown *document, Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_INIT_NEW_DOCUMENT_H_
