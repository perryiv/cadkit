
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_XML_XMLFILE_H__
#define __USUL_XML_XMLFILE_H__


#include <sstream>
#include <vector>
#include <string>


class XMLFile
{
public:
  typedef std::vector < std::string > Strings;

  XMLFile( std::ostream& );

  void openTag ( const std::string& );
  void closeTag ( const std::string& );

  void tagWithValue( const std::string& tag, const std::string& value );

  void tagWithAttributes( const std::string& tag, const Strings& keys, const Strings& values, const std::string& value );
  void openTagWithAttributes ( const std::string& tag, const Strings& keys, const Strings& values );

  std::ostream&       stream();
  const std::ostream& stream() const;
  
private:
  XMLFile();
  XMLFile& operator= ( const XMLFile& );

  std::ostream &_os;
};


#endif // __USUL_XML_XMLFILE_H__
