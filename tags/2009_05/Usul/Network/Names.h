
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pre-defined names for name-value options.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_NETWORK_NAMES_H_
#define _USUL_NETWORK_NAMES_H_

#include <string>


namespace Usul {
namespace Network {


namespace Names
{
  const std::string REQUEST     ( "request" );
  const std::string FORMAT      ( "format" );
  const std::string SRS         ( "srs" );
  const std::string STYLES      ( "styles" );
  const std::string WIDTH       ( "width" );
  const std::string HEIGHT      ( "height" );
  const std::string LAYERS      ( "layers" );
  const std::string BBOX        ( "bbox" );
  const std::string VERSION     ( "version" );
}


} // namespace WMS
} // namespace Usul


#undef WMS_OUTPUT


#endif // _USUL_NETWORK_NAMES_H_
