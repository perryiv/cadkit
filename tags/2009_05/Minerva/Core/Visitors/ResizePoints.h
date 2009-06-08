//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_RESIZE_POINTS_H__
#define __MINERVA_VISITORS_RESIZE_POINTS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"

#include <map>

namespace Minerva {
namespace Core {
namespace Visitors {
      
class MINERVA_EXPORT ResizePoints : public Minerva::Core::Visitor
{
public:
  typedef Minerva::Core::Visitor BaseClass;
        
  USUL_DECLARE_REF_POINTERS ( ResizePoints );
        
  ResizePoints ( double factor );
        
  virtual void visit ( Minerva::Core::Data::DataObject &object );

protected:
  /// Use reference counting.
  virtual ~ResizePoints();
  
private:
  double _factor;
};
      
      
}
}
}

#endif // __MINERVA_VISITORS_STACK_POINTS_H__
