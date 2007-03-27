
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/ReadModel.h"

#include "osgDB/Registry"
#include "osgDB/ReadFile"

#include <sstream>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Read model using osg. Throws if fails to read.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OsgTools::readModel ( const std::string& filename )
{
  // Read the file.
  osgDB::ReaderWriter::ReadResult rr ( osgDB::Registry::instance()->readNode ( filename, osgDB::Registry::instance()->getOptions() ) );

  // Return now if it worked.
  if ( rr.validNode() )
    return rr.takeNode();
  
  // Make the error message.
  std::ostringstream message;
  message << "Error 3896128824: Failed to load: " << filename;

  // Add any error reported by the reader.
  if ( rr.error() )
    message << ", Reason: " << rr.message();

  // Report the error.
  throw std::runtime_error ( message.str() );
}
