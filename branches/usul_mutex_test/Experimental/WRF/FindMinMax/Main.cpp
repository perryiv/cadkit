
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/Parser.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Math/Finite.h"
#include "Usul/Math/NaN.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/SafeCall.h"

#include <iostream>
#include <algorithm>
#include <limits>
#include <functional>
#include <fstream>
#include <sstream>

#include <cstdlib>


struct FindMinMax
{
  FindMinMax () :
    close ( 10 ),
    min ( std::numeric_limits < float >::max() ),
    max ( std::numeric_limits < float >::min() )
  {
  }

  void update ( const float& value )
  {
    if ( !Usul::Math::finite ( value ) || Usul::Math::nan ( value ) )
      return;

    if ( close ( value, 1.0e+035f ) )
      return;

    if ( value < min )
      min = value;

    if ( value > max )
      max = value;
  }

  Usul::Predicates::CloseFloat< float > close;
  float min, max;
};

typedef void (FindMinMax::*Function) ( const float &s );
typedef Usul::Adaptors::MemberFunction < FindMinMax*, Function > MemFun;


///////////////////////////////////////////////////////////////////////////////
//
//  Run.
//
///////////////////////////////////////////////////////////////////////////////

void _run ()
{
  const std::string filename ( Usul::CommandLine::Arguments::instance().argv()[1] );

  Parser parser ( filename );

  unsigned int xSize ( 504 ), ySize ( 507 ), zSize ( 101 ), numTimesteps ( 25 );
  unsigned int numChannels ( 3 );
  unsigned int numFields2D ( 3 );
  bool headers ( true );

  /*unsigned int xSize ( 507 ), ySize ( 504 ), zSize ( 101 ), numTimesteps ( 1 );
  unsigned int numChannels ( 3 );
  unsigned int numFields2D ( 0 );
  bool headers ( false );*/

  /*unsigned int xSize ( 201 ), ySize ( 219 ), zSize ( 98 ), numTimesteps ( 49 );
  unsigned int numChannels ( 4 );
  unsigned int numFields2D ( 1 );
  bool headers ( false );*/

  parser.setSizes ( xSize, ySize, zSize );
  parser.timesteps ( numTimesteps );
  parser.channels ( numChannels );
  parser.numFields2D ( numFields2D );
  parser.headers ( headers );

  /*for ( unsigned int i = 2; i < numChannels; ++ i )
  {
    FindMinMax minMax;

    for ( unsigned int j = 24; j < numTimesteps; ++j )
    {
      Parser::Data data;

      parser.data ( data, j, i );
      
      std::for_each ( data.begin(), data.end(), MemFun ( &minMax, &FindMinMax::update ) );
    }

    std::cout << " Channel: " << i
                << " Min value: " << minMax.min
                << " Max value: " << minMax.max
                << std::endl;
  }*/
  {
    FindMinMax minMax;

    Parser::Data data;

    parser.field2D ( data, 1 );
    
    std::for_each ( data.begin(), data.end(), MemFun ( &minMax, &FindMinMax::update ) );

    std::cout << " Lat: "
                << " Min value: " << minMax.min
                << " Max value: " << minMax.max
                << std::endl;
  }

  {
    FindMinMax minMax;

    Parser::Data data;

    parser.field2D ( data, 2 );
    
    std::for_each ( data.begin(), data.end(), MemFun ( &minMax, &FindMinMax::update ) );

    std::cout << " Long: "
                << " Min value: " << minMax.min
                << " Max value: " << minMax.max
                << std::endl;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )
{
  Usul::CommandLine::Arguments::instance().set( argc, argv );

  if ( argc < 2 )
    return -1;

  Usul::Functions::safeCall ( _run, "1840397975" );

  std::cin.get();

  return 0;
}
