
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_FIND_MIN_MAX_DATES_H__
#define __MINERVA_VISITORS_FIND_MIN_MAX_DATES_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Core/Animate/Date.h"

namespace Minerva {
namespace Core {
namespace Visitors {

class MINERVA_EXPORT FindMinMaxDates : public Minerva::Core::Visitor
{
public:
  typedef Minerva::Core::Visitor BaseClass;
  typedef Minerva::Core::Animate::Date Date;

  USUL_DECLARE_REF_POINTERS ( FindMinMaxDates );

  FindMinMaxDates();

  virtual void    visit ( Minerva::Core::Data::Feature &object );

  /// Get the first date.
  const Date &    first() const { return _first; }

  /// Get the last date.
  const Date &    last() const { return _last; }

protected:

  /// Use reference counting.
  virtual ~FindMinMaxDates();
  
  /// Update min and max dates.
  void            _updateMin ( const Date& );
  void            _updateMax ( const Date& );

private:
  
  Date _first;
  Date _last;
};


}
}
}


#endif // __MINERVA_VISITORS_FIND_MIN_MAX_DATES_H__
