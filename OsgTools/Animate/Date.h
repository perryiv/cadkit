
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Date class wrapper around boost's date class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_DATE_H__
#define __OSGTOOLS_ANIMATE_DATE_H__

#include "OsgTools/Export.h"

#include <string>

#ifdef _MSC_VER
# pragma warning ( disable : 4996 )
# pragma warning ( disable : 4561 )
#endif

#include "boost/date_time/gregorian/gregorian.hpp"

namespace OsgTools {
namespace Animate {

class OSG_TOOLS_EXPORT Date
{
public:

  Date();

  Date ( const std::string& date );
  
  void         day( unsigned int );
  unsigned int day() const;

  void         month( unsigned int );
  unsigned int month() const;

  void         year( unsigned int );
  unsigned int year() const;

  void increment();

  void moveBackNumDays ( unsigned int );

  std::string toString();

  bool operator<( const Date& rhs ) const;
  bool operator>( const Date& rhs ) const;

  bool operator==( const Date& rhs ) const;
  bool operator!=(const Date& rhs ) const;

  bool operator<=( const Date& rhs ) const;
  bool operator>=( const Date& rhs ) const;

protected:
  long _toJulian() const;

private:
  boost::gregorian::date _date;
};

}
}

#endif // __OSGTOOLS_ANIMATE_DATE_H__
