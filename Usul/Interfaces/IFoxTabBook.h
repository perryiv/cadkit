
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _USUL_INTERFACE_FOX_TAB_BOOK_H_
#define _USUL_INTERFACE_FOX_TAB_BOOK_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXTabBook; }

namespace Usul {
namespace Interfaces {

struct IFoxTabBook : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxTabBook );

  /// Id for this interface.
  enum { IID = 1267154677u };

  virtual FX::FXTabBook *       tabBook  (  ) = 0;
  virtual const FX::FXTabBook * tabBook  (  ) const = 0;


}; // class IFoxTabBook


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_TAB_BOOK_H_
