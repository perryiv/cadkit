
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_RESOURCES_REPORT_ERRORS_H___
#define __USUL_RESOURCES_REPORT_ERRORS_H___

#include "Usul/Export/Export.h"

namespace Usul { namespace Interfaces { struct IReportErrors; } }


namespace Usul {
namespace Resources {


// Get/set pointer to report errors
USUL_EXPORT Usul::Interfaces::IReportErrors*    reportErrors();
USUL_EXPORT void                                reportErrors ( Usul::Interfaces::IReportErrors * );


} // namespace Resources
} // namespace Usul



#endif // __USUL_RESOURCES_REPORT_ERRORS_H___
