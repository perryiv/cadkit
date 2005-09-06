
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/ReadModel.h"
#include "OsgTools/ScopedOptions.h"
#include "osgDB/Registry"
#include "osgDB/ReadFile"

#include <sstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Read model using osg.  Throws if fails to read.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OsgTools::readModel ( const std::string& filename, const std::string& s )
{
  typedef osgDB::ReaderWriter::Options Options;

  // Save the current options
  OsgTools::ScopedOptions current;

  // Set new options, which may be an empty string.
  osg::ref_ptr<Options> options ( new Options );
  options->setOptionString ( s );
  osgDB::Registry *reg = osgDB::Registry::instance();
  reg->setOptions ( options.get() );

  // Read the file.
  osgDB::ReaderWriter::ReadResult rr ( reg->readNode ( filename, options.get() ) );

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

