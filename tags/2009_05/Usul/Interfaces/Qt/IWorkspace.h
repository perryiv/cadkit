
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_QT_IWORKSPACE_H__
#define __USUL_INTERFACES_QT_IWORKSPACE_H__

#include "Usul/Interfaces/IUnknown.h"

class QWorkspace;

namespace Usul {
namespace Interfaces {
namespace Qt {

struct IWorkspace : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWorkspace );

  /// Id for this interface.
  enum { IID = 1052899483u };

  virtual QWorkspace *               workspace() = 0;
  virtual const QWorkspace*          workspace() const = 0;

}; // struct IWorkspace


} // end namespace Qt
} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_QT_IWORKSPACE_H__ */

