
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IADD_ROW_LEGEND_H__
#define __USUL_INTERFACES_IADD_ROW_LEGEND_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Widgets { class LegendObject; } }

namespace Usul {
namespace Interfaces {


struct IAddRowLegend : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddRowLegend );

  /// Id for this interface.
  enum { IID = 1598417075u };

  /// Get flag to show row in legend.
  virtual bool                         showInLegend () const = 0;

  virtual void                         addLegendRow ( OsgTools::Widgets::LegendObject* row ) = 0;

}; // struct IAddRowLegend


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IADD_ROW_LEGEND_H__ */

