
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _USUL_INTERFACE_FOX_TAB_ITEM_H_
#define _USUL_INTERFACE_FOX_TAB_ITEM_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXTabItem; }

namespace Usul {
namespace Interfaces {

struct IFoxTabItem : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxTabItem );

  /// Id for this interface.
  enum { IID = 1102357568u };

  virtual void                  addTab    ( const std::string &name ) = 0;
  virtual void                  deleteTab ( const std::string &name ) = 0;
  virtual void                  deleteTabs() = 0;
  virtual FX::FXTabItem *       tabItem   ( const std::string &name ) = 0;
  virtual const FX::FXTabItem * tabItem   ( const std::string &name ) const = 0;


}; // class IFoxTabItem


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_TAB_ITEM_H_
