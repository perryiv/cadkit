
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating documents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_NEW_DOCUMENT_CREATE_H_
#define _USUL_INTERFACE_NEW_DOCUMENT_CREATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct INewDocumentCreate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INewDocumentCreate );

  /// Id for this interface.
  enum { IID = 2870744805u };

  /// Name of the document.
  virtual std::string       documentTypeName() = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_NEW_DOCUMENT_CREATE_H_
