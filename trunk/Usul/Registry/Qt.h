
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String conversion functions for Qt objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_STRING_CONVERSIONS_QT_H_
#define _USUL_REGISTRY_STRING_CONVERSIONS_QT_H_

#include "QtCore/QByteArray"
#include "QtCore/QRect"
#include "QtCore/QString"
#include "QtCore/QStringList"

#include <sstream>


namespace Usul { namespace Registry {


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a QRect.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < QRect >
{
  static std::string to ( const QRect &r )
  {
    std::ostringstream os;
    os << r.left() << ' ' << r.right() << ' ' << r.top() << ' ' << r.bottom();
    return os.str();
  }
  static void from ( const std::string &s, QRect &r )
  {
    std::istringstream in ( s );
    int left ( 0 ), right ( 0 ), top ( 0 ), bottom ( 0 );
    in >> left >> right >> top >> bottom;
    r.setLeft   ( left );
    r.setRight  ( right );
    r.setTop    ( top );
    r.setBottom ( bottom );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a QStringList.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < QStringList >
{
  static std::string to ( const QStringList &sl )
  {
    if ( true == sl.isEmpty() )
      return "";

    std::ostringstream out;
    out << sl.at(0).toLocal8Bit().constData();
    for ( unsigned int i = 0; i < sl.size(); ++i )
      out << ';' << sl.at(i).toLocal8Bit().constData();

    return out.str();
  }
  static void from ( const std::string &s, QStringList &sl )
  {
    sl.clear();

    if ( true == s.empty() )
      return;

    QString temp ( s.c_str() );
    sl = temp.split ( ';', QString::KeepEmptyParts, Qt::CaseSensitive );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a QByteArray.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < QByteArray >
{
  typedef int IntegerType;

  static std::string to ( const QByteArray &ba )
  {
    std::ostringstream out;
    const IntegerType numElements ( static_cast < IntegerType > ( ba.size() ) );
    if ( numElements > 0 )
    {
      out << numElements << " ";
      for ( IntegerType i = 0; i < numElements; ++i )
      {
        const IntegerType c ( static_cast < IntegerType > ( ba.at(i) ) );
        out << c << ( ( ( i + 1 ) < numElements ) ? " " : "" );
      }
    }
    return out.str();
  }
  static void from ( const std::string &s, QByteArray &ba )
  {
    ba.clear();

    if ( true == s.empty() )
      return;

    std::istringstream in ( s );

    IntegerType numElements ( 0 );
    in >> numElements;

    if ( 0 == numElements )
      return;

    ba.reserve ( static_cast < int > ( numElements ) );

    for ( IntegerType i = 0; i < numElements; ++i )
    {
      if ( EOF == in.peek() )
        return;

      IntegerType c ( 0 );
      in >> c;

      ba.push_back ( static_cast < char > ( c ) );
    }
  }
};


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_STRING_CONVERSIONS_QT_H_
