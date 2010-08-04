
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
//  Write the scenario input file
//
///////////////////////////////////////////////////////////////////////////////

void Scenario::write( const std::string& filename )
{
	 Guard guard ( this );

  // useful typedef
  typedef std::vector< std::string > StringVec;

  // create a file handle
  std::ofstream ofs;

  // open the file
  ofs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
  {
    std::cout << Usul::Strings::format ( "Failed to open file: ", filename ) << std::endl;
    return;
  }

  // feedback.
  std::cout << "Writing Scenario file: " << filename << std::endl;

  // buffer size
  const unsigned long int bufSize ( 4095 );

  // line number
  unsigned int lineNumber ( 0 );

  for( unsigned int i = 0; i < _scenario.size(); ++i )
  {
		// get the scenario entry
		IVPI::VPIScenarioEntry s ( _scenario.at( i ) );

		std::string value ( s.value );

		// format the value if it is a drop down type
		if( s.type == IVPI::SCENARIO_TYPE_DROP_DOWN )
		{
			value = "";
			for( unsigned int j = 0; j < s.options.size() - 1; ++j )
			{
				value = Usul::Strings::format( value, s.options.at( j ), "|" );
			}
			value = Usul::Strings::format( value, s.options.at( s.options.size() - 1 ) );
		}

    // output string
    std::string str1 ( Usul::Strings::format( s.type, ",", s.lineNumber, ",", s.selectedOption, ",", s.question, ",", value ) );

    // output to the file
    ofs << str1 << std::endl;    
  }

  // close file
  ofs.close();
}	


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
      if( sv.size() == 5 )
      {
				// convert the type to an int
				int type ( Usul::Convert::Type< std::string, int >::convert( sv[0] ) );

				// convert the line number to an int
				int ln ( Usul::Convert::Type< std::string, int >::convert( sv[1] ) );

				// convert the selected option to an int
				int so ( Usul::Convert::Type< std::string, int >::convert( sv[2] ) );

				// create the scenario entry
				VPIScenarioEntry s ( sv[3], sv[4], type );		

				// set the line number and selected option
				s.lineNumber = ln;
				s.selectedOption = so;

				// If the type is a drop down
				if( type == IVPI::SCENARIO_TYPE_DROP_DOWN )
				{
					s.options = this->_getOptions( sv[4] );
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

