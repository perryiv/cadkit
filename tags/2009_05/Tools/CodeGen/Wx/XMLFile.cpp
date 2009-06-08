
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "XMLFile.h"


XMLFile::XMLFile( std::ostream& os ) : _os ( os )
{
  _os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
}


void XMLFile::openTag ( const std::string& tag )
{
  _os << "<" << tag << ">" << std::endl;
}

void XMLFile::closeTag ( const std::string& tag)
{
  _os << "</" << tag << ">" << std::endl;
}

void XMLFile::tagWithValue( const std::string& tag, const std::string& value )
{
  _os << "<" << tag;
  
  if ( value.empty() )
    _os << " />" << std::endl;
  else
    _os << ">" << value << "</" << tag << ">" << std::endl;
}

void XMLFile::tagWithAttributes( const std::string& tag, const Strings& keys, const Strings& values, const std::string& value )
{
  if ( keys.size() != values.size() )
    return;

  _os << "<" << tag;
	for ( unsigned int i = 0; i < keys.size(); ++i ) 
  {
		_os << " " << keys[i] << "=\"" << values[i] << "\"";
	}
  if ( value.empty() ) 
  {
		_os << " />" << std::endl;
	} 
  else 
  {
		_os << ">" << value << "</" << tag << ">" << std::endl;
	}
}

void XMLFile::openTagWithAttributes ( const std::string& tag, const Strings& keys, const Strings& values )
{
  if (keys.size() != values.size() )
	  return;

	_os << "<" << tag;
	for ( unsigned int i = 0; i < keys.size(); ++i ) 
  {
		_os << " " << keys[i] << "=\"" << values[i] << "\"";
	}
	_os << ">" << std::endl;
}


std::ostream& XMLFile::stream()
{
  return _os;
}

const std::ostream& XMLFile::stream() const
{
  return _os;
}
