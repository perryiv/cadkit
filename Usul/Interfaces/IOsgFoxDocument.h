
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_OSG_FOX_DOCUMENT_H_
#define _USUL_INTERFACE_OSG_FOX_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace OsgFox { namespace Documents { class Document; }; };


namespace Usul {
namespace Interfaces {


struct IOsgFoxDocument : public Usul::Interfaces::IUnknown
{
  /// Useful typedef.
  typedef OsgFox::Documents::Document Document;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOsgFoxDocument );

  /// Id for this interface.
  enum { IID = 2405311697u };

  /// Return pointer to the document.
  virtual const Document *        getOsgFoxDocument() const = 0;
  virtual Document *              getOsgFoxDocument() = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OSG_FOX_DOCUMENT_H_
