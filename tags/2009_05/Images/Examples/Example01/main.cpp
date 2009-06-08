
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the image classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# define NOMINMAX
#endif

#include "Images/Core/Image.h"

#include "Usul/File/Find.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Plugin extension.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  #ifdef _DEBUG
  const std::string plugExt ( "plugd" );
  #else
  const std::string plugExt ( "plug" );
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Components::Manager PluginManager;
typedef Usul::CommandLine::Arguments Arguments;


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize all the image.
//
///////////////////////////////////////////////////////////////////////////////

void normalize ( const std::string &dir, const std::string &ext )
{
  // Get all files in the directory.
  typedef std::vector < std::string > Files;
  Files files;
  Usul::File::find ( dir, ext, files );

  // Sort the names.
  std::sort ( files.begin(), files.end() );

  // Initialize.
  typedef Images::Image::ValueCount ValueCountAll;
  typedef ValueCountAll::value_type ValueCountOneChannel;
  typedef ValueCountOneChannel::first_type PixelValueType;
  PixelValueType lowest  ( std::numeric_limits<PixelValueType>::max() );
  PixelValueType highest ( std::numeric_limits<PixelValueType>::min() );

  // Loop through the files.
  for ( Files::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    std::cout << "Processing: " << (*i);

    // Open the image.
    Images::Image::ValidRefPtr image = new Images::Image;
    image->read ( *i );

    // Make sure the image is grayscale.
    image->toGrayScale();

    // Get extreme values in all the channels.
    ValueCountAll low, high;
    low.push_back  ( ValueCountOneChannel ( -1, 0 ) );
    high.push_back ( ValueCountOneChannel ( -1, 0 ) );
    image->extremes ( low, high );
    std::cout << ",  low = "   << std::setw ( 5 ) << low.at(0).first 
              << " ("          << std::setw ( 4 ) << low.at(0).second 
              << "),  high = " << std::setw ( 5 ) << high.at(0).first 
              << " ("          << std::setw ( 4 ) << high.at(0).second 
              << ')';

    // Update global extrema.
    if ( low.at(0).first < lowest )
      lowest = low.at(0).first;
    if ( high.at(0).first > highest )
      highest = high.at(0).first;

    std::cout << std::endl;
  }

  std::cout << "Overall lowest = " << lowest << ", highest = " << highest << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Store arguments.
  Arguments::instance().set ( argv, argv + argc );

  // Process arguments.
  if ( 3 != argc )
  {
    std::cout << "Usage: " << argv[0] << " <directory> <extension>" << std::endl;
    return 1;
  }

  // Safely...
  try
  {
    // Load plugins.
    const std::string dir ( Arguments::instance().directory() );
    PluginManager::instance().load ( Usul::Interfaces::IPlugin::IID, dir, Detail::plugExt );

    // Feedback about plugins.
    const PluginManager::Strings names ( PluginManager::instance().names() );
    std::cout << "Found " << names.size() << " plugins: ";
    std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
    std::cout << std::endl;

    // Normalize the images.
    normalize ( argv[1], argv[2] );

    // Release all the plugins.
    PluginManager::instance().clear();
  }

  // Catch all exceptions.
  catch ( std::exception &e )
  {
    std::cout << "Standard exception caught: " << e.what() << std::endl;
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

  std::cout << "Press any key to quit" << std::endl;
  std::cin.get();
  return 1;
}
