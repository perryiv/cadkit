
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

#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Path.h"
#include "Usul/Math/MinMax.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Types/Types.h"

#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/io/tiff_io.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <tiffio.h>

#include <iostream>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::CommandLine::Arguments Arguments;
typedef boost::gil::rgb8_image_t ImageType;
typedef ImageType::point_t Dimensions;


///////////////////////////////////////////////////////////////////////////////
//
//  Single threaded.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Check file existance.
//
///////////////////////////////////////////////////////////////////////////////

void _checkExists ( const Arguments::Args &files )
{
  for ( Arguments::Args::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    const std::string file ( *i );
    if ( false == Usul::Predicates::FileExists::test ( file ) )
    {
      throw std::runtime_error ( Usul::Strings::format ( "File '", file, "' does not exist" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Determine the maximum dimensions of any input image.
//
///////////////////////////////////////////////////////////////////////////////

Dimensions _determineMaxInputDimensions ( const Arguments::Args &files )
{
  USUL_ASSERT_SAME_TYPE ( Dimensions, boost::gil::point2<std::ptrdiff_t> );

  // Initialize the dimensions.
  Dimensions maxDim ( 0, 0 );

  // Loop through the files.
  for ( Arguments::Args::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    const std::string file ( *i );
    Dimensions dim ( boost::gil::jpeg_read_dimensions ( file ) );
    maxDim.x = Usul::Math::maximum ( dim.x, maxDim.x );
    maxDim.y = Usul::Math::maximum ( dim.y, maxDim.y );
  }

  // Return the maximum dimensions of any input file.
  return maxDim;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the image.
//
///////////////////////////////////////////////////////////////////////////////

template < class ImageClass > inline void _readImage ( const std::string &file, ImageClass &image )
{
  boost::gil::jpeg_read_and_convert_image ( file, image );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the image.
//
///////////////////////////////////////////////////////////////////////////////

template < class ImageClass > inline void _writeImage ( const std::string &file, const ImageClass &image, int quality )
{
  boost::gil::jpeg_write_view ( file, boost::gil::const_view ( image ), quality );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the output quality.
//
///////////////////////////////////////////////////////////////////////////////

int _outputQuality()
{
  // Get the arguments.
  Arguments::Args args ( Arguments::instance().args() );

  // Initialize.
  int quality ( 100 );

  // Read the string.
  std::istringstream in ( args.at ( args.size() - 1 ) );
  in >> quality;

  // Clamp to range [0,100].
  quality = Usul::Math::maximum ( 0, Usul::Math::minimum ( quality, 100 ) );
  return quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  // GIL handels errors by throwing exceptions.
  ::TIFFSetErrorHandler ( 0x0 );
  ::TIFFSetWarningHandler ( 0x0 );

  // Get the arguments.
  Arguments::Args args ( Arguments::instance().args() );

  // Get the program name.
  const std::string program ( Usul::File::base ( Arguments::instance().program() ) );

  // Check arguments.
  if ( args.size() < 5 )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Usage:\n\t", program, " <input 1> <input 2> [input 3] ... [input n] <output> <quality>" ) );
  }

  // Get the input and output file names.
  const std::string output ( args.at ( args.size() - 2 ) );
  Arguments::Args input ( args.begin() + 1, args.begin() + ( args.size() - 3 ) );

  // Make sure all the input files exist.
  ::_checkExists ( input );

  // Get largest dimensions of any input file.
  Dimensions inputDimensions ( ::_determineMaxInputDimensions ( input ) );

  // Allocate the target image.
  ImageType target;
  Dimensions outputDimensions ( ( static_cast<Dimensions::value_type> ( input.size() ) * inputDimensions.x ), inputDimensions.y );
  std::cout << "Allocating output image of size " << outputDimensions.x << " x " << outputDimensions.y << " x 3" << std::endl;
  target.recreate ( outputDimensions );

  // Initialize the corner of the view.
  Dimensions topLeft ( 0, 0 );

  // Loop through the input images.
  for ( Arguments::Args::const_iterator i = input.begin(); i != input.end(); ++i )
  {
    // Get current inut file name.
    const std::string file ( *i );

    // Read input image.
    ImageType image;
    std::cout << "Reading file '" << file << "' ... " << std::flush;
    ::_readImage ( file, image );

    // Copy all the pixels from the input image to the current view.
    std::cout << "Copying pixels ... " << std::flush;
    boost::gil::copy_and_convert_pixels ( boost::gil::const_view ( image ), boost::gil::subimage_view ( boost::gil::view ( target ), topLeft, inputDimensions ) );
    std::cout << "Done" << std::endl;

    // Move the original of the next view.
    topLeft.x += inputDimensions.x;
  }

  // Write the final image file.
  int quality ( ::_outputQuality() );
  std::cout << "Writing file: " << output << " ... " << std::flush;
  ::_writeImage ( output, target, quality );
  std::cout << "Done" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  try
  {
    ::_run();
  }

  catch ( const std::exception &e )
  {
    std::cout << e.what() << std::endl;
    return 1;
  }

  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
    return 1;
  }

  //std::cin.get();
  return 0;
}
