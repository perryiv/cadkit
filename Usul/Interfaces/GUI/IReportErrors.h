
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for reporting errors
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_REPORT_ERRORS_H_
#define _USUL_INTERFACE_REPORT_ERRORS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct IReportErrors : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReportErrors);

  /// Id for this interface.
  enum { IID = 1103226210u };

  enum
  {
    STANDARD_OUT   = 0x00000001,
    STANDARD_ERROR = 0x00000002,
    LOG_FILE       = 0x00000004,
    ERROR_DIALOG   = 0x00000008,
    MESSAGE_WINDOW = 0x00000010,
    STATUS_BAR     = 0x00000020,
  };

  virtual void reportErrors ( unsigned int options, bool clear ) = 0;

}; //  IReportsErrors


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_REPORT_ERRORS_H_
