
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functions/GUID.h"

# include <vector>

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
#else
#include "uuid/uuid.h"
#include <cstring>
#endif


std::string Usul::Functions::GUID::generate()
{
  std::vector < char > b ( 256, 0x0 );
  const char *ptr ( 0x0 );

#ifdef _MSC_VER

  UUID uuid;
  UuidCreateSequential( &uuid );
  unsigned char *p = reinterpret_cast<unsigned char *> ( &b[0] );
  ::UuidToString ( &uuid, &p );
  ptr = reinterpret_cast<const char *> ( p );

#else

  uuid_t uuid;
  ::uuid_generate ( uuid );
  ::uuid_unparse ( uuid, &b[0] );
  ptr = &b[0];

#endif
  
  // Make the std::string.
  const unsigned int len ( ::strlen ( ptr ) );
  std::string guid ( ptr, len );

  return guid;
}
