
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Starts and terminates Xerces library.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/XercesLife.h"
#include "XmlTree/Functions.h"

#include "xercesc/util/OutOfMemoryException.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#include <iostream>
#include <stdexcept>

using namespace XmlTree;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

XercesLife::XercesLife()
{
  XercesLife::initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

XercesLife::~XercesLife()
{
  try
  {
    XercesLife::terminate();
  }
  catch ( const std::exception &e )
  {
    std::cout << e.what() << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void XercesLife::initialize()
{
  try
  {
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch ( const xercesc::OutOfMemoryException & )
  {
    throw std::runtime_error ( "Error 4811502520: Ran out of memory while initializing Xerces" );
  }
  catch ( const xercesc::XMLException &e )
  {
    throw std::runtime_error ( "Error 2787644571: Failed to initialize Xerces. " + XmlTree::Functions::translate ( e.getMessage() ) );
  }
  catch ( const std::exception &e )
  {
    throw std::runtime_error ( "Error 3205772651: Standard exception caught while initializing Xerces. " + std::string ( ( ( e.what() ) ? e.what() : "" ) ) );
  }
  catch ( ... )
  {
    throw std::runtime_error ( "Error 2268037835: Unknown exception caught while initializing Xerces" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finish.
//
///////////////////////////////////////////////////////////////////////////////

void XercesLife::terminate()
{
  // Not using terminate until a thread-safe way is found.
#if 0
  try
  {
    xercesc::XMLPlatformUtils::Terminate();
  }
  catch ( const xercesc::XMLException &e )
  {
    throw std::runtime_error ( "Error 1972549100: Failed to terminate Xerces, " + XmlTree::Functions::translate ( e.getMessage() ) );
  }
  catch ( const std::exception &e )
  {
    throw std::runtime_error ( "Error 2032356942: Standard exception caught while terminating Xerces. " + std::string ( ( ( e.what() ) ? e.what() : "" ) ) );
  }
  catch ( ... )
  {
    throw std::runtime_error ( "Error 2543876004: Unknown exception caught while terminating Xerces" );
  }
#endif
}
