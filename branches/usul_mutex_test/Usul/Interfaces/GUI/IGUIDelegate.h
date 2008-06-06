
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for a GUI delegate
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GUI_DELEGATE_H_
#define _USUL_INTERFACE_GUI_DELEGATE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Documents { class Document; } }


namespace Usul {
namespace Interfaces {


struct IGUIDelegate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGUIDelegate );

  /// Id for this interface.
  enum { IID = 1927013416u };

  virtual bool               doesHandle       ( const std::string& token ) const = 0;
  virtual void               createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GUI_DELEGATE_H_

