
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _USUL_INTERFACE_FOX_TREE_LIST_H_
#define _USUL_INTERFACE_FOX_TREE_LIST_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXTreeList; }

namespace Usul {
namespace Interfaces {

struct IFoxTreeList : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxTreeList );

  /// Id for this interface.
  enum { IID = 3239707569u };

  virtual void                   addTree ( const std::string &name ) = 0;
  virtual FX::FXTreeList *       tree    ( const std::string &name ) = 0;
  virtual const FX::FXTreeList * tree    ( const std::string &name ) const = 0;


}; // class IFoxTreeList


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_TREE_LIST_H_

