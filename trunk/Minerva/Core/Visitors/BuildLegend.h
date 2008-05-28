
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_BUILD_LEGEND_H__
#define __MINERVA_VISITORS_BUILD_LEGEND_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Legend/Legend.h"

#include <map>

namespace Minerva {
namespace Core {
namespace Visitors {
      
class MINERVA_EXPORT BuildLegend : public Minerva::Core::Visitor
{
public:
  typedef Minerva::Core::Visitor BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( BuildLegend );
  
  BuildLegend ( OsgTools::Legend::Legend *legend );
  
  virtual void visit ( Minerva::Core::Layers::Container &layer );
  
protected:
  
  /// Use reference counting.
  virtual ~BuildLegend();
  
private:
  OsgTools::Legend::Legend::RefPtr _legend;
};


}
}
}

#endif // __MINERVA_VISITORS_BUILD_LEGEND_H__
