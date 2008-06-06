
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDOCUMENTSELECT_H__
#define __USUL_INTERFACES_IDOCUMENTSELECT_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Documents/Document.h"

#include <list>

namespace Usul {
namespace Interfaces {


struct IDocumentSelect : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocumentSelect );

  /// Id for this interface.
  enum { IID = 52292362u };

  typedef Usul::Documents::Document             Document;
  typedef Document::RefPtr                      DocumentPtr;
  typedef std::list< DocumentPtr >              Documents;

  virtual Usul::Documents::Document*            selectDocument ( const Documents& ) = 0;
}; // struct IDocumentSelect


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDOCUMENTSELECT_H__ */
