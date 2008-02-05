
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
#endif

std::string Usul::Functions::GUID::generate()
{
  std::string guid;
  
  std::vector < char > b ( 256, 0x0 );
#ifdef _MSC_VER
  UUID uuid;
  UuidCreateSequential( &uuid );
  char *p = &b.front();
  ::UuidToString ( &uuid, &p );
#else
  
#if 0
  // TODO: I think there is a better way to get a guid on a unix box.  Maybe look for uuidgen and execute it externaly?
  // Create a guid.
  char buf [ 256 ];
  unsigned int t1 ( ::time ( 0x0 ) );

  ::srand ( ::time ( 0x0 ) );

  unsigned int r1 ( ::rand () );
  unsigned int r2 ( ::rand () );
  unsigned int r3 ( ::rand () );
  
  unsigned int t2 ( ::time ( 0x0 ) );
  ::sprintf ( buf, "%08x-%04x-%04x-%04x-%012x", t1, r1, r2, r3, t2 );

  guid = buf;
#endif
  uuid_t uuid;
  ::uuid_generate ( uuid );
  ::uuid_unparse ( uuid, &b[0] );
#endif
  
  const unsigned int len ( ::strlen ( (char* ) &b[0] ) );
  const char *ptr ( &b[0] );
  guid.insert ( guid.end(), ptr, ptr + len );

  return guid;
}
