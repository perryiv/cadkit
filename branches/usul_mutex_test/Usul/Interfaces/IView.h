
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VIEW_H_
#define _USUL_INTERFACE_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

  struct IDocument;

struct IView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IView );

  /// Id for this interface.
  enum { IID = 2923712104u };

  // Get the document
  virtual Usul::Interfaces::IDocument  * document() = 0;

}; // class IView


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_VIEW_H_
