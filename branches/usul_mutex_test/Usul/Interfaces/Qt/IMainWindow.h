
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_QT_IMAIN_WINDOW_H__
#define __USUL_INTERFACES_QT_IMAIN_WINDOW_H__

#include "Usul/Interfaces/IUnknown.h"

class QMainWindow;

namespace Usul {
namespace Interfaces {
namespace Qt {

struct IMainWindow : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMainWindow );

  /// Id for this interface.
  enum { IID = 3793824554u };

  virtual QMainWindow *              mainWindow() = 0;
  virtual const QMainWindow*         mainWindow() const = 0;

}; // struct IMainWindow


} // end namespace Qt
} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_QT_IMAIN_WINDOW_H__ */

