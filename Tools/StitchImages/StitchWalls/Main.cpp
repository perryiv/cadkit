
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to stitch walls together.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define NOMINMAX
#include <windows.h>
#endif

#include "Images/GIL/Actions/Stitcher.h"
#include "Images/GIL/IO/JPG.h"
#include "Images/GIL/IO/PNG.h"
#include "Images/GIL/IO/TIF.h"
#include "Images/GIL/Policies/RGB8.h"

#include "Usul/File/Path.h"

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Images::GIL::Policies::RGB8 Policy;
typedef Policy::ImageType ImageType;
typedef Images::GIL::IO::JPG<ImageType> JPG;
typedef Images::GIL::IO::PNG<ImageType> PNG;
typedef Images::GIL::IO::TIF<ImageType> TIF;
typedef Images::GIL::Actions::Stitcher<Policy> Stitcher;


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void _run ( const Stitcher::Strings &files )
{
  // Declare stitcher and add IO modules.
  Stitcher stitcher;
  stitcher.add ( "jpg",  new JPG() );
  stitcher.add ( "jpeg", new JPG() );
  stitcher.add ( "png",  new PNG() );
  stitcher.add ( "tif",  new TIF() );
  stitcher.add ( "tiff", new TIF() );

  // Allocate input files.
  Stitcher::FileNames input ( 1 );
  const unsigned int numFiles ( static_cast<unsigned int> ( files.size() ) );
  input.at(0).reserve ( numFiles );

  // Determine if outside walls are being stitched.
  const bool allWalls ( 7 == numFiles );

  // Add walls.
  if ( allWalls )
  {
    input.at(0).push_back ( files.at(6) );
  }
  input.at(0).push_back ( files.at(4) );
  input.at(0).push_back ( files.at(2) );
  input.at(0).push_back ( files.at(0) );
  input.at(0).push_back ( files.at(1) );
  input.at(0).push_back ( files.at(3) );
  if ( allWalls )
  {
    input.at(0).push_back ( files.at(5) );
  }

  // Add input.
  stitcher.input ( input );

  // Generated output file name.
  std::ostringstream out;
  out << Usul::File::directory ( input.at(0).at(0), true );
  out << "stitched_";
  out << ( ( allWalls ) ? Usul::File::base ( files.at(6) ) : Usul::File::base ( files.at(4) ) );
  out << "_to_";
  out << ( ( allWalls ) ? Usul::File::base ( files.at(5) ) : Usul::File::base ( files.at(3) ) );
  out << '.' << Usul::File::extension ( input.at(0).at(0) );
  const std::string output ( out.str() );
  stitcher.output ( output );

  // Stitch and write.
  stitcher.stitch ( &std::cout );
  stitcher.write ( &std::cout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  int result ( 1 );
  const unsigned int numWalls ( static_cast<unsigned int> ( argc ) - 1 );

  if ( 5 != numWalls && 7 != numWalls )
  {
    std::cout << "Usage:\n\t" << Usul::File::base ( argv[0] );
    std::cout << " <infile1> <infile2> ... <infile5> [infile6 infile7]" << std::endl;
    return result;
  }

  try
  {
    Stitcher::Strings input ( argv + 1, argv + argc );
    ::_run ( input );
    result = 0;
  }

  catch ( const std::exception &e )
  {
    const std::string what ( e.what() );
    std::cout << ( ( e.what() ) ? e.what() : "Standard exception caught" ) << std::endl;
  }

  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

  return result;
}
