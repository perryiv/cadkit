
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
#include "Usul/App/Application.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Curl.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/filesystem/operations.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Download file.
//
///////////////////////////////////////////////////////////////////////////////

bool Minerva::Core::Utilities::download ( const std::string& href, std::string& filename )
{
  return Minerva::Core::Utilities::download ( href, filename, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Download file.  TODO: Re-write function to handle work offline state.
//
///////////////////////////////////////////////////////////////////////////////

bool Minerva::Core::Utilities::download ( const std::string& href, std::string& filename, bool useCache )
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
    boost::algorithm::replace_all ( filename, "&", "_" );
    boost::algorithm::replace_all ( filename, "@", "_" );

    const std::string program ( Usul::App::Application::instance().program() );
    filename = Usul::Strings::format ( Usul::File::Temp::directory ( true ), program, "/", filename );
    
    if ( useCache && Usul::Predicates::FileExists::test ( filename ) )
    {
      return true;
    }
    
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


///////////////////////////////////////////////////////////////////////////////
//
//  Download to given filename.
//
///////////////////////////////////////////////////////////////////////////////

bool Minerva::Core::Utilities::downloadToFile ( const std::string& href, const std::string& filename )
{
  const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["timeout_milliseconds"].get<unsigned int> ( 600000, true ) );
  return downloadToFile ( href, filename, timeout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Download to given filename.
//
///////////////////////////////////////////////////////////////////////////////

bool Minerva::Core::Utilities::downloadToFile ( const std::string& href, const std::string& filename, unsigned int timeout )
{
  // See if we can use the network.
  const bool workOffline ( Usul::Registry::Database::instance()["work_offline"].get<bool> ( false, true ) );
  
  // Return now if we are suppose to work offline.
  if ( true == workOffline )
    return false;

  bool success ( false );

  try
  {
    Usul::Network::Curl curl ( href, filename );
    curl.download ( timeout, static_cast<std::ostream*> ( 0x0 ), "" );

    success = true;
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1638679894" )

  return success;
}
