
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "ReaderWriterSTL.h"


#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"


ReaderWriterSTL::ReaderWriterSTL()
{
}

ReaderWriterSTL::~ReaderWriterSTL()
{
}

bool ReaderWriterSTL::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "stl" );
}

const char* ReaderWriterSTL::className()
{
  return "STL Reader";
}

Result ReaderWriterSTL::readNode ( const std::string &file, const Options * )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterPDB::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterPDB::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterPDB::Result ( "Unknown exception caught" );
  }

}

osg::Group * ReaderWriterSTL::_build() const
{
  return NULL;
}

void  ReaderWriterSTL::_init()
{
}

void ReaderWriterSTL::_parse ( std::ifstream &in )
{
}

Result ReaderWriter::_read ( const std::string &, const Options * )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Configure this instance based on the options.
  this->_configure ( options );

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( file ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( file.c_str() );

  // Make sure it opened.
  if ( !in.is_open() )
    return ReadResult::ERROR_IN_READING_FILE;
  
  // Parse all the file and build internal data.
  this->_parse ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( _build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}

osgDB::RegisterReaderWriterProxy<ReaderWriterSTL> g_ReaderWriter_STL_proxy;
