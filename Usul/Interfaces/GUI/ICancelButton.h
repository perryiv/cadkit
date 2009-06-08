
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for showing and hiding a cancel button
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CANCEL_BUTTON_H_
#define _USUL_INTERFACE_CANCEL_BUTTON_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICancelButton : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICancelButton );

  /// Id for this interface.
  enum { IID = 1103221577u };

  /// Show/hide.
  virtual void showCancelButton() = 0;
  virtual void hideCancelButton() = 0;

  /// Helper struct to make sure we show/hide.
  struct ShowHide
  {
    template < class T > ShowHide ( T *t ) : _cancel ( t )
    {
      if ( _cancel.valid() )
        _cancel->showCancelButton();
    }
    ~ShowHide()
    {
      if ( _cancel.valid() )
        _cancel->hideCancelButton();
    }
  private:
    ICancelButton::QueryPtr _cancel;
  };
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CANCEL_BUTTON_H_

