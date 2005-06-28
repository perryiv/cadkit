
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the image classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Images/Core/Image.h"

#include "Usul/File/Find.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"

#include <iostream>
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Configure for single-threaded execution. This is global because the sooner 
//  we set this, the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newMutex );


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
  Usul::File::find ( dir, "*." + ext, files );

  // Sort the names.
  std::sort ( files.begin(), files.end() );

  // Loop through the files.
  for ( Files::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    std::cout << "Processing: " << (*i) << std::endl;

    // Open the image.
    Images::Image::ValidRefPtr image = new Images::Image;
    image->read ( *i );

    // Make sure they are grayscale.
    image->toGrayScale();

#if 0

    // Get the histogram.
    typedef Images::Image::Histogram Histogram;
    Histogram h;
    image->histogram ( h, std::numeric_limits<Usul::Types::Uint16>::max() );

    // Print the histogram.
    std::copy ( h.begin(), h.end(), std::ostream_iterator<Histogram::value_type> ( std::cout, "\n" ) );

#endif
  }
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
