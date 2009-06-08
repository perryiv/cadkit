
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILAYER_ADD_GUI_QT_H__
#define __USUL_INTERFACES_ILAYER_ADD_GUI_QT_H__

#include "Usul/Interfaces/IUnknown.h"

class QWidget;

#include <string>

namespace Usul {
namespace Interfaces {


struct ILayerAddGUIQt : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayerAddGUIQt );

  /// Id for this interface.
  enum { IID = 2946322505u };

  virtual QWidget*            layerAddGUI ( Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;

  virtual std::string         name () const = 0;

  virtual void                apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller ) = 0;

}; // struct ILayerAddGUIQt


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_ILAYER_ADD_GUI_QT_H__
