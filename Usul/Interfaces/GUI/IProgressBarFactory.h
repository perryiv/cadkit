
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IPROGRESS_BAR_FACTORY_H__
#define __USUL_INTERFACES_IPROGRESS_BAR_FACTORY_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IProgressBarFactory : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProgressBarFactory );

  /// Id for this interface.
  enum { IID = 1512701809u };

  // Create a progress bar.
  virtual Usul::Interfaces::IUnknown::RefPtr  createProgressBar ( bool waitIfNotGuiThread = false ) = 0;

}; // struct IProgressBarFactory


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IPROGRESS_BAR_FACTORY_H__ */

