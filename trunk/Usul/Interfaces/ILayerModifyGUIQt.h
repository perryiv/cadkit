
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILAYER_MODIFY_GUI_QT_H__
#define __USUL_INTERFACES_ILAYER_MODIFY_GUI_QT_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {

struct ILayerModifyGUIQt : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayerModifyGUIQt );

  /// Id for this interface.
  enum { IID = 1262894538u };

  virtual bool                handle ( Usul::Interfaces::IUnknown* layer ) const = 0;
  virtual void                showModifyGUI ( Usul::Interfaces::IUnknown* layer, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;

}; // struct ILayerModifyGUIQt


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_ILAYER_MODIFY_GUI_QT_H__
