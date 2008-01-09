
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert QString to native.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_QT_STRING_TO_NATIVE_
#define _USUL_STRINGS_QT_STRING_TO_NATIVE_

#include "QtCore/QString"

#include <string>


namespace Usul {
namespace Strings {


inline void convert ( const QString &source, std::string &target )
{
  target.clear();
  if ( false == source.isEmpty() )
  {
    const QByteArray bytes ( source.toAscii() );
    target.assign ( bytes.begin(), bytes.end() );
  }
}


inline std::string convert ( const QString &source )
{
  std::string target;
  Usul::Strings::convert ( source, target );
  return target;
}


template < class QtStringContainer, class StandardStringContainer > 
inline void convertTo ( const QtStringContainer &q, StandardStringContainer &s )
{
  s.clear();
  if ( false == q.empty() )
  {
    s.reserve ( q.size() );
    for ( typename QtStringContainer::const_iterator i = q.begin(); i != q.end(); ++i )
    {
      const QString &temp ( *i );
      const QByteArray bytes ( temp.toAscii() );
      s.push_back ( std::string ( bytes.begin(), bytes.end() ) );
    }
  }
}

template < class QtStringContainer, class StandardStringContainer > 
inline void convertFrom ( const StandardStringContainer &s, QtStringContainer &q )
{
  q.clear();
  if ( false == s.empty() )
  {
    for ( typename StandardStringContainer::const_iterator i = s.begin(); i != s.end(); ++i )
    {
      q.push_back ( i->c_str() );
    }
  }
}


} // String
} // Usul


#endif // _USUL_STRINGS_QT_STRING_TO_NATIVE_
