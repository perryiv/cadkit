
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functions/Guid.h"

#ifdef _MSC_VER
# include <vector>
# include "windows.h"
#endif

std::string Usul::Functions::generateGUID()
{
  std::string guid;
#ifdef _MSC_VER
  std::vector < unsigned char > b ( 256, 0x0 );
  UUID uuid;
  UuidCreateSequential( &uuid );
  unsigned char *p = &b.front();
  ::UuidToString ( &uuid, &p );

  unsigned int len ( ::strlen ( (char* ) p ) );
  guid.insert ( guid.end(), p, p + len );
#else
  // TODO: I think there is a better way to get a guid on a unix box.  Maybe look for uuidgen and execute it externaly?
  // Create a guid.
  char buf [ 256 ];
  unsigned int t1 ( ::time ( 0x0 ) );

  ::srand ( ::time ( 0x0 ) );

  unsigned int r1 ( ::rand () );
  unsigned int r2 ( ::rand () );
  unsigned int r3 ( ::rand () );
  
  unsigned long t2 ( ::time ( 0x0 ) );
  ::sprintf ( buf, "%08x-%04x-%04x-%04x-%012x", t1, r1, r2, r3, t2 );

  guid = buf;
#endif

  return guid;
}
