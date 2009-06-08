
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
//  Program to stitch images together.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Handle broken jpeg header.
#define XMD_H
#ifdef FAR
#undef FAR
#endif

#include "Images/GIL/Actions/Stitcher.h"
#include "Images/GIL/IO/JPG.h"
#include "Images/GIL/IO/PNG.h"
#include "Images/GIL/IO/TIF.h"
#include "Images/GIL/Policies/RGB8.h"

#include "Usul/File/Path.h"


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

void _run ( int argc, char **argv )
{
  Stitcher stitcher;
  stitcher.add ( "jpg",  new JPG() );
  stitcher.add ( "jpeg", new JPG() );
  stitcher.add ( "png",  new PNG() );
  stitcher.add ( "tif",  new TIF() );
  stitcher.add ( "tiff", new TIF() );
  stitcher.parse ( argc, argv, &std::cout );
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

  try
  {
    ::_run ( argc, argv );
    result = 0;
  }

  catch ( const Images::GIL::Exceptions::CommandLineException &e )
  {
    std::cout << e.what() << std::endl;
    const std::string program ( Usul::File::base ( argv[0] ) );
    std::cout << "Usage: " << program << Stitcher::arguments ( "\n\t" ) << std::endl;
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
