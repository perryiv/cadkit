
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Utilities/Download.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Curl.h"
#include "Usul/Registry/Database.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/filesystem/operations.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Download file.
//
///////////////////////////////////////////////////////////////////////////////

bool Minerva::Core::Utilities::download ( const std::string& href, std::string& filename )
{
  bool success ( false );
  
  try
  {
    // Make the filename.
    filename = href;

    // Replace illegal characters for filename.
    boost::replace_first ( filename, "http://", "" );  
    boost::algorithm::replace_all ( filename, "/", "_" );
    boost::algorithm::replace_all ( filename, "?", "_" );
    boost::algorithm::replace_all ( filename, "=", "_" );
    filename = Usul::File::Temp::directory ( true ) + filename;
    
    // Download.
    {
      if ( boost::filesystem::exists ( filename ) && boost::filesystem::is_directory ( filename ) )
        boost::filesystem::remove_all ( filename );
      
      const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["kml_layer"]["timeout_milliseconds"].get<unsigned int> ( 600000, true ) );
      
      Usul::Network::Curl curl ( href, filename );
      curl.download ( timeout, static_cast<std::ostream*> ( 0x0 ), "" );
    }

    boost::algorithm::replace_all ( filename, "\\", "/" );
    
    // If we get here, it worked.
    success = true;
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1638679894" )
  
  return success;
}
