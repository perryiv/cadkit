
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class to test if the member string ends with the given string.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_FILE_NAME_ENDS_WITH_H_
#define _USUL_PREDICATES_FILE_NAME_ENDS_WITH_H_

#include <string>
#include <functional>


namespace Usul {
namespace Predicates {


struct EndsWith : public std::unary_function < std::string, bool >
{
  EndsWith ( const EndsWith &ew ) : _s ( ew._s ){}
  EndsWith ( const std::string &s ) : _s ( s ){}
  bool operator () ( const std::string &s ) const
  {
    return 
      s.size() > _s.size() &&
      std::equal ( s.begin() + ( s.size() - _s.size() ), s.end(), _s.begin() );
  }
protected:
  std::string _s;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_FILE_NAME_ENDS_WITH_H_
