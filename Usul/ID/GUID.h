
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a globally unique identifier.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_GUID_CLASS_H_
#define _USUL_GUID_CLASS_H_

#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace ID {


struct GUID
{
  // Construct.
  GUID();
  GUID ( const std::string &guid );
  GUID ( Types::Uint32 data0, 
         Types::Uint32 data1, 
         Types::Uint32 data2, 
         Types::Uint32 data3 );

  // Compare.
  bool equal ( const GUID & ) const;

  // Get the parts.
  Types::Uint32 data0() const { return _data0; }
  Types::Uint32 data1() const { return _data1; }
  Types::Uint32 data2() const { return _data2; }
  Types::Uint32 data3() const { return _data3; }

private:

  Types::Uint32 _data0;
  Types::Uint32 _data1;
  Types::Uint32 _data2;
  Types::Uint32 _data3;
};


} // namespace ID
} // namespace Usul


#endif // _USUL_GUID_CLASS_H_
