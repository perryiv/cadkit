
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IGUI_DELEGATE_NOTIFY_H__
#define __USUL_INTERFACES_IGUI_DELEGATE_NOTIFY_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Documents { class Document; } }

namespace Usul {
namespace Interfaces {

struct IGUIDelegateNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGUIDelegateNotify );

  /// Id for this interface.
  enum { IID = 4190648721u };

  virtual void               notifyDocumentFinishedLoading ( Usul::Documents::Document* document ) = 0;

}; // struct IGUIDelegateNotify


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IGUI_DELEGATE_NOTIFY_H__ */

