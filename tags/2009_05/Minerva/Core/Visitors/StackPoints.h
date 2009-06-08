
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_STACK_POINTS_H__
#define __MINERVA_VISITORS_STACK_POINTS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/Animate/Date.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Predicates/LessVector.h"

#include <map>

namespace Minerva {
namespace Core {
namespace Visitors {
      
class MINERVA_EXPORT StackPoints : public Minerva::Core::Visitor
{
public:
  typedef Minerva::Core::Visitor BaseClass;
  typedef Minerva::Core::Animate::Date Date;

  USUL_DECLARE_REF_POINTERS ( StackPoints );

  StackPoints ( double mutliplier = 1.0 );

  virtual void visit ( Minerva::Core::Data::DataObject &object );
  virtual void visit ( Minerva::Core::Data::Container &layer );

protected:

  /// Use reference counting.
  virtual ~StackPoints ();

private:

  typedef std::equal_to<unsigned int> EqualPredicate;
  typedef Usul::Predicates::LessVector < EqualPredicate, 3 > LessVector;
  typedef std::map< Usul::Math::Vec3ui, unsigned int, LessVector > Counts;
  
  Counts _counts;
  double _multiplier;
};
      
      
}
}
}

#endif // __MINERVA_VISITORS_STACK_POINTS_H__
