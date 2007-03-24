
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Info.h"
#include "Connection.h"

#include "Minerva/Core/DB/Info.h"

using namespace DT::Minerva::Glue;

array< System::String^ >^  Info::geometryTables ( DT::Minerva::Interfaces::IDatabaseConnection^ connection )
{
  typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  try
  {
    if( nullptr != connection )
    {
      DT::Minerva::Glue::Connection^ c = safe_cast < DT::Minerva::Glue::Connection^ > ( connection );
      if( nullptr != c )
      {
        DbInfo::RefPtr info ( new DbInfo ( c->nativeConnection() ) );

        Strings strings ( info->geometryTables() );

        array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

        for( unsigned int i = 0; i < strings.size(); ++i )
          tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

        return tables;
      }
    }
  }
  catch ( System::Exception ^ e )
  {
    System::Console::WriteLine ( "Error 3942088156: {0}", e->Message );
  }

  return gcnew array<System::String^>(0);
}
