
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Scenario.h"

#include "Usul/Strings/Format.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/Strings/Trim.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Make.h"
#include "Usul/File/Remove.h"

#include <fstream>
#include <stdexcept>

USUL_IMPLEMENT_TYPE_ID ( Scenario );

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Scenario, Scenario::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Scenario::Scenario ( )
{
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Scenario::~Scenario()
{
}

/////////////////
//  PUBLIC
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Read the scenario input file
//
///////////////////////////////////////////////////////////////////////////////

void Scenario::read( const std::string &filename )
{
	 Guard guard ( this );

  // useful typedef
  typedef std::vector< std::string > StringVec;

  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
  {
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename, ". No cracks information" ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Reading cracks information from: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // parse the file
  while( EOF != ifs.peek() )
  {
    // create a buffer
    char buffer[bufSize+1];

    // get a line
    ifs.getline ( buffer, bufSize );

    // create a string from the buffer
    std::string tStr ( buffer );

    if( tStr.size() > 0 && tStr.at( 0 ) != '#' )
    {

      // separate the strings
      StringVec sv;
      Usul::Strings::split( tStr, ",", false, sv );
      
      // make sure all the columns are there
      if( sv.size() == 3 )
      {
				// convert the type to an int
				int type ( Usul::Convert::Type< std::string, int >::convert( sv[0] ) );

				// create the scenario entry
				VPIScenarioEntry s ( sv[1], sv[2], type );				

				// If the type is a drop down
				if( type == IVPI::SCENARIO_TYPE_DROP_DOWN )
				{
					s.options = this->_getOptions( sv[2] );
				}

				// add the scenario entry to the scenario
				_scenario.push_back( s );
      } 

    }// end if for valid entry found

  }// end while read for file parsing

  ifs.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the scenario input file
//
///////////////////////////////////////////////////////////////////////////////
 
Scenario::VPIScenario	Scenario::scenario()
{
	return _scenario;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the scenario input file
//
///////////////////////////////////////////////////////////////////////////////
 
void Scenario::scenario( IVPI::VPIScenario s )
{
	_scenario = s;
}


/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Read the scenario input file
//
///////////////////////////////////////////////////////////////////////////////
 
Scenario::StringVec	Scenario::_getOptions( const std::string& optStr )
{
	Guard guard ( this );

	// string vector for return
	StringVec options;

	if( optStr.size() > 0 )
  {
    // separate the strings
		StringVec sv;
    Usul::Strings::split( optStr, "|", false, sv );
    
		for( unsigned int i = 0; i < sv.size(); ++i )
		{
    	options.push_back( sv[i] );
    } 

  }// end if for valid entry found

	return options;
}

