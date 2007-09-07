
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating documents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_CREATE_H_
#define _USUL_INTERFACE_DOCUMENT_CREATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {

  namespace Documents { class Document; }

namespace Interfaces {


struct IDocumentCreate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentCreate );

  /// Id for this interface.
  enum { IID = 1432533693u };

  /// Create the document.
  virtual Usul::Documents::Document *    createDocument ( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_CREATE_H_
