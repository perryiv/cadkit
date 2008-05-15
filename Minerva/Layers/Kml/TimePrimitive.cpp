
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/TimePrimitive.h"
#include "Minerva/Core/Animate/Date.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"

#include <vector>

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimePrimitive::TimePrimitive() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimePrimitive::TimePrimitive ( const XmlTree::Node &node ) : BaseClass ( node )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TimePrimitive::~TimePrimitive()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse.
//
///////////////////////////////////////////////////////////////////////////////

TimePrimitive::Date TimePrimitive::parse ( const std::string& value )
{
  typedef std::vector<std::string> Strings;
  Strings strings;
  Usul::Strings::split ( value, "T", false, strings );
  
  std::string date ( strings.size() > 0 ? strings[0] : "" );
  
  // Currently not handling time.
  //std::string time ( strings.size() > 1 ? strings[1] : "" );

  Strings parts;
  Usul::Strings::split ( date, "-", false, parts );

  if ( false == parts.empty() )
  {
    int year ( Usul::Convert::Type<std::string,int>::convert ( parts[0] ) );
    int month ( parts.size() > 1 ? Usul::Convert::Type<std::string,int>::convert ( parts[1] ) : 1 );
    int day ( parts.size() > 2 ? Usul::Convert::Type<std::string,int>::convert ( parts[2] ) : 1 );

    if ( year < 1400 )
      year += 1400;
  
    return Date ( boost::gregorian::from_simple_string ( Usul::Strings::format ( year, "-", month, "-", day ) ) );
  }

  return Date();
}
